import { useCallback, useEffect, useMemo, useRef, useState } from 'react'
import { Tree, type NodeApi, type NodeRendererProps } from 'react-arborist'
import * as monaco from 'monaco-editor'
import './monacoSetup'
import './App.css'

type EntryHandle = FileSystemDirectoryHandle | FileSystemFileHandle

type FileNode = {
  id: string
  name: string
  kind: 'file' | 'directory'
  handle?: EntryHandle
  path: string
  children?: FileNode[]
}

type PermissionMode = 'read' | 'readwrite'

type DirectoryHandleWithEntries = FileSystemDirectoryHandle & {
  entries?: () => AsyncIterableIterator<[string, EntryHandle]>
  values?: () => AsyncIterableIterator<FileSystemHandle>
}

type FileSystemHandleWithPermissions = FileSystemHandle & {
  queryPermission?: (descriptor?: { mode?: PermissionMode }) => Promise<PermissionState>
  requestPermission?: (descriptor?: { mode?: PermissionMode }) => Promise<PermissionState>
}

const MIN_TREE_WIDTH = 200
const MIN_TREE_HEIGHT = 200

function App() {
  const [treeData, setTreeData] = useState<FileNode[]>([])
  const [selectedId, setSelectedId] = useState<string>()
  const [loading, setLoading] = useState(false)
  const [statusMessage, setStatusMessage] = useState('ディレクトリを選択してください')
  const [errorMessage, setErrorMessage] = useState<string | null>(null)
  const [currentFilePath, setCurrentFilePath] = useState<string>()
  const [currentLanguage, setCurrentLanguage] = useState<'markdown' | 'plaintext'>('markdown')
  const [fileContent, setFileContent] = useState('')
  const [hasUnsavedChanges, setHasUnsavedChanges] = useState(false)
  const [dataSource, setDataSource] = useState<'none' | 'filesystem' | 'random'>('none')

  const treeContainerRef = useRef<HTMLDivElement | null>(null)
  const [treeSize, setTreeSize] = useState<{ width: number; height: number }>({
    width: MIN_TREE_WIDTH,
    height: MIN_TREE_HEIGHT,
  })

  const editorContainerRef = useRef<HTMLDivElement | null>(null)
  const editorRef = useRef<monaco.editor.IStandaloneCodeEditor | null>(null)
  const currentFileHandleRef = useRef<FileSystemFileHandle | null>(null)
  const suppressModelUpdateRef = useRef(false)

  const isMarkdownFile = useMemo(
    () => (currentFilePath ? isMarkdown(currentFilePath) : false),
    [currentFilePath],
  )

  useEffect(() => {
    const element = treeContainerRef.current
    if (!element) {
      return
    }

    const observer = new ResizeObserver((entries) => {
      const entry = entries[0]
      if (!entry) {
        return
      }

      const { width, height } = entry.contentRect
      setTreeSize({
        width: Math.max(width, MIN_TREE_WIDTH),
        height: Math.max(height, MIN_TREE_HEIGHT),
      })
    })

    observer.observe(element)
    return () => observer.disconnect()
  }, [])

  useEffect(() => {
    const container = editorContainerRef.current
    if (!container || editorRef.current) {
      return
    }

    const editor = monaco.editor.create(container, {
      value: '',
      language: 'markdown',
      theme: 'vs-dark',
      automaticLayout: true,
      minimap: { enabled: false },
      fontSize: 14,
      lineNumbers: 'on',
      readOnly: true,
    })

    const listener = editor.onDidChangeModelContent(() => {
      if (suppressModelUpdateRef.current) {
        return
      }

      const model = editor.getModel()
      if (!model) {
        return
      }

      setFileContent(model.getValue())
      setHasUnsavedChanges(true)
    })

    editorRef.current = editor

    return () => {
      listener.dispose()
      editor.dispose()
      if (editorRef.current === editor) {
        editorRef.current = null
      }
    }
  }, [])

  useEffect(() => {
    const editor = editorRef.current
    if (!editor) {
      return
    }

    if (!currentFilePath) {
      editor.setModel(null)
      return
    }

    const uri = toModelUri(currentFilePath)
    let model = monaco.editor.getModel(uri)

    if (!model) {
      model = monaco.editor.createModel(fileContent, currentLanguage, uri)
    } else {
      if (model.getLanguageId() !== currentLanguage) {
        monaco.editor.setModelLanguage(model, currentLanguage)
      }

      if (model.getValue() !== fileContent) {
        suppressModelUpdateRef.current = true
        model.setValue(fileContent)
        suppressModelUpdateRef.current = false
      }
    }

    editor.setModel(model)
    editor.updateOptions({ readOnly: !isMarkdownFile })
  }, [currentFilePath, currentLanguage, fileContent, isMarkdownFile])

  useEffect(() => {
    const container = editorContainerRef.current
    if (!container) {
      return
    }

    const handleDragOver = (event: DragEvent) => {
      if (!isMarkdownFile) {
        return
      }

      if (event.dataTransfer?.types.includes('Files')) {
        event.preventDefault()
        event.dataTransfer.dropEffect = 'copy'
      }
    }

    const handleDrop = async (event: DragEvent) => {
      if (!isMarkdownFile) {
        return
      }

      if (!editorRef.current) {
        return
      }

      const files = event.dataTransfer?.files
      if (!files || files.length === 0) {
        return
      }

      event.preventDefault()

      const editor = editorRef.current
      const model = editor?.getModel()
      if (!model) {
        return
      }

      for (const file of Array.from(files)) {
        if (!file.type.startsWith('image/')) {
          continue
        }

        try {
          const dataUrl = await fileToDataUrl(file)
          const selection = editor.getSelection() ?? model.getFullModelRange()
          const markdownImage = `![${file.name}](${dataUrl})`

          editor.executeEdits('drop-image', [
            {
              range: selection,
              text: markdownImage,
              forceMoveMarkers: true,
            },
          ])

          editor.focus()
          setHasUnsavedChanges(true)
        } catch (error) {
          setErrorMessage(getErrorMessage(error))
        }
      }
    }

    container.addEventListener('dragover', handleDragOver)
    container.addEventListener('drop', handleDrop)

    return () => {
      container.removeEventListener('dragover', handleDragOver)
      container.removeEventListener('drop', handleDrop)
    }
  }, [isMarkdownFile])

  const handleOpenDirectory = useCallback(async () => {
    setErrorMessage(null)

    if (!('showDirectoryPicker' in window)) {
      setErrorMessage('ブラウザがFile System Access APIに対応していません。')
      return
    }

    try {
      setLoading(true)
      const picker = window as Window & typeof globalThis & {
        showDirectoryPicker: () => Promise<FileSystemDirectoryHandle>
      }

      const directoryHandle = await picker.showDirectoryPicker()
      const granted = await ensurePermission(directoryHandle, 'read')

      if (!granted) {
        setErrorMessage('ディレクトリの読み取りが許可されませんでした。')
        return
      }

      const rootNode = await buildTreeFromDirectory(directoryHandle, directoryHandle.name || 'root')
      setTreeData([rootNode])
      setSelectedId(undefined)
      setStatusMessage(`開いたディレクトリ: ${rootNode.name}`)
      setCurrentFilePath(undefined)
      currentFileHandleRef.current = null
      setFileContent('')
      setHasUnsavedChanges(false)
      setDataSource('filesystem')
    } catch (error) {
      setErrorMessage(getErrorMessage(error))
    } finally {
      setLoading(false)
    }
  }, [])

  const handleGenerateRandomTree = useCallback(() => {
    const randomRoot = generateRandomTree()
    setTreeData([randomRoot])
    setSelectedId(undefined)
    setStatusMessage('ランダムツリーを表示中')
    setCurrentFilePath(undefined)
    currentFileHandleRef.current = null
    setFileContent('')
    setHasUnsavedChanges(false)
    setErrorMessage(null)
    setDataSource('random')
  }, [])

  const openFile = useCallback(async (node: FileNode) => {
    if (node.kind !== 'file' || !node.handle) {
      return
    }

    const fileHandle = node.handle as FileSystemFileHandle

    try {
      const granted = await ensurePermission(fileHandle, 'readwrite')
      if (!granted) {
        setErrorMessage('ファイルの読み書きが許可されませんでした。')
        return
      }

      const file = await fileHandle.getFile()
      const text = await file.text()
      currentFileHandleRef.current = fileHandle
      setCurrentFilePath(node.path)
      const language = isMarkdown(node.name) ? 'markdown' : 'plaintext'
      setCurrentLanguage(language)
      setFileContent(text)
      setHasUnsavedChanges(false)
      setStatusMessage(`編集中: ${node.path}`)
      setErrorMessage(null)
    } catch (error) {
      setErrorMessage(getErrorMessage(error))
    }
  }, [])

  const handleSelect = useCallback(
    (nodes: NodeApi<FileNode>[]) => {
      const target = nodes[0]
      if (!target) {
        return
      }

      setSelectedId(target.id)

      if (dataSource !== 'random' && target.data.kind === 'file') {
        void openFile(target.data)
      } else {
        setCurrentFilePath(undefined)
        currentFileHandleRef.current = null
        setFileContent('')
        setHasUnsavedChanges(false)
        const label = target.data.kind === 'file' ? 'ノード' : 'フォルダ'
        setStatusMessage(`${label}: ${target.data.path}`)
      }
    },
    [dataSource, openFile],
  )

  const handleActivate = useCallback(
    (node: NodeApi<FileNode>) => {
      if (node.data.kind === 'directory' || dataSource === 'random') {
        node.toggle()
        return
      }

      setSelectedId(node.id)
      void openFile(node.data)
    },
    [dataSource, openFile],
  )

  const handleSave = useCallback(async () => {
    if (!isMarkdownFile || !currentFileHandleRef.current) {
      return
    }

    try {
      const granted = await ensurePermission(currentFileHandleRef.current, 'readwrite')
      if (!granted) {
        setErrorMessage('保存するには書き込み権限が必要です。')
        return
      }

      const writable = await currentFileHandleRef.current.createWritable()
      await writable.write(fileContent)
      await writable.close()
      setHasUnsavedChanges(false)
      setStatusMessage(`保存しました: ${currentFilePath}`)
      setErrorMessage(null)
    } catch (error) {
      setErrorMessage(getErrorMessage(error))
    }
  }, [currentFilePath, fileContent, isMarkdownFile])

  const helperMessage = useMemo(() => {
    if (errorMessage) {
      return errorMessage
    }

    if (dataSource === 'random') {
      return 'ランダムツリーは表示のみです'
    }

    if (!currentFilePath) {
      return 'ツリーからMarkdownファイルを選択してください'
    }

    if (!isMarkdownFile) {
      return '.mdファイルのみ編集可能です'
    }

    return hasUnsavedChanges ? '未保存の変更があります' : '保存済みです'
  }, [currentFilePath, errorMessage, hasUnsavedChanges, isMarkdownFile])

  return (
    <div className="app-root">
      <header className="app-header">
        <div className="header-left">
          <h1 className="app-title">Web IDE プロトタイプ</h1>
          <p className="app-subtitle">React Arborist + Monaco Editor</p>
        </div>
        <div className="header-actions">
          <button className="primary" onClick={handleOpenDirectory} disabled={loading}>
            {loading ? '読込中...' : 'ディレクトリを開く'}
          </button>
          <button onClick={handleGenerateRandomTree}>ランダムツリー</button>
          <button
            onClick={handleSave}
            disabled={!isMarkdownFile || !currentFileHandleRef.current || !hasUnsavedChanges}
          >
            保存
          </button>
        </div>
      </header>

      <div className="status-bar">
        <span>{statusMessage}</span>
        <span className={errorMessage ? 'status-helper error' : 'status-helper'}>{helperMessage}</span>
      </div>

      <div className="workspace">
        <aside className="tree-panel" ref={treeContainerRef}>
          {treeData.length > 0 ? (
            <Tree
              data={treeData}
              selection={selectedId}
              onSelect={handleSelect}
              onActivate={handleActivate}
              openByDefault
              width={treeSize.width}
              height={treeSize.height}
              rowHeight={28}
              indent={18}
              className="file-tree"
            >
              {TreeNode}
            </Tree>
          ) : (
            <div className="tree-placeholder">
              <p>左上のボタンからディレクトリを選択してください。</p>
            </div>
          )}
        </aside>

        <section className="editor-panel">
          <div className="editor-meta">
            <span className="editor-path">{currentFilePath ?? 'ファイルが選択されていません'}</span>
            {isMarkdownFile ? <span className="editor-badge">Markdown</span> : null}
            {!isMarkdownFile && currentFilePath ? (
              <span className="editor-badge mute">読み取り専用</span>
            ) : null}
          </div>
          <div className="editor-container" ref={editorContainerRef} />
        </section>
      </div>
    </div>
  )
}

function TreeNode({ node, style, dragHandle }: NodeRendererProps<FileNode>) {
  const icon = node.data.kind === 'directory' ? (node.isOpen ? '📂' : '📁') : '📄'

  return (
    <div
      className={`tree-node ${node.isSelected ? 'selected' : ''}`}
      style={style}
      ref={dragHandle}
    >
      <span className="tree-node-icon">{icon}</span>
      <span className="tree-node-name">{node.data.name}</span>
    </div>
  )
}

async function buildTreeFromDirectory(
  directoryHandle: FileSystemDirectoryHandle,
  parentPath: string,
): Promise<FileNode> {
  const children: FileNode[] = []

  const entries = await listDirectoryEntries(directoryHandle)

  for (const [name, handle] of entries) {
    const entryPath = `${parentPath}/${name}`

    if (handle.kind === 'directory') {
      const childNode = await buildTreeFromDirectory(handle, entryPath)
      children.push(childNode)
    } else {
      children.push({
        id: entryPath,
        name,
        kind: 'file',
        handle,
        path: entryPath,
      })
    }
  }

  children.sort(sortNodes)

  return {
    id: parentPath,
    name: directoryHandle.name || parentPath,
    kind: 'directory',
    handle: directoryHandle,
    path: parentPath,
    children,
  }
}

function sortNodes(a: FileNode, b: FileNode) {
  if (a.kind === b.kind) {
    return a.name.localeCompare(b.name)
  }

  return a.kind === 'directory' ? -1 : 1
}

function isMarkdown(path: string) {
  const lower = path.toLowerCase()
  return lower.endsWith('.md') || lower.endsWith('.markdown')
}

async function ensurePermission(handle: FileSystemHandle | undefined, mode: PermissionMode) {
  if (!handle) {
    return false
  }

  const withPermissions = handle as FileSystemHandleWithPermissions

  if (
    typeof withPermissions.queryPermission === 'function' &&
    typeof withPermissions.requestPermission === 'function'
  ) {
    const query = await withPermissions.queryPermission({ mode })
    if (query === 'granted') {
      return true
    }

    if (query === 'prompt' || query === 'denied') {
      const request = await withPermissions.requestPermission({ mode })
      return request === 'granted'
    }
  }

  return true
}

async function listDirectoryEntries(handle: FileSystemDirectoryHandle) {
  const directory = handle as DirectoryHandleWithEntries
  const result: Array<[string, EntryHandle]> = []

  if (typeof directory.entries === 'function') {
    for await (const [name, child] of directory.entries()) {
      result.push([name, child as EntryHandle])
    }
    return result
  }

  if (typeof directory.values === 'function') {
    for await (const child of directory.values()) {
      const entry = child as EntryHandle
      result.push([entry.name, entry])
    }
    return result
  }

  throw new Error('ディレクトリエントリの列挙に対応していません。')
}

function getErrorMessage(error: unknown) {
  if (error instanceof Error) {
    return error.message
  }

  return String(error)
}

async function fileToDataUrl(file: File) {
  return new Promise<string>((resolve, reject) => {
    const reader = new FileReader()

    reader.onload = () => {
      resolve(typeof reader.result === 'string' ? reader.result : '')
    }

    reader.onerror = () => {
      reject(reader.error ?? new Error('画像の読み込みに失敗しました'))
    }

    reader.readAsDataURL(file)
  })
}

function toModelUri(path: string) {
  const encoded = path
    .split('/')
    .map((segment) => encodeURIComponent(segment))
    .join('/')

  return monaco.Uri.parse(`file:///${encoded}`)
}

function generateRandomTree(): FileNode {
  const rootId = `random-${Math.random().toString(36).slice(2)}`
  return createRandomNode(rootId, rootId, 0)
}

function createRandomNode(id: string, path: string, depth: number): FileNode {
  const shouldBeDirectory = depth < 2 ? Math.random() > 0.3 : false

  if (!shouldBeDirectory) {
    return {
      id,
      name: `node-${Math.floor(Math.random() * 1000)}`,
      kind: 'file',
      path,
    }
  }

  const childCount = Math.floor(Math.random() * 4) + 1
  const children: FileNode[] = []

  for (let i = 0; i < childCount; i += 1) {
    const childId = `${id}-${i}-${Math.floor(Math.random() * 1000)}`
    const childPath = `${path}/child-${i}`
    children.push(createRandomNode(childId, childPath, depth + 1))
  }

  return {
    id,
    name: `branch-${Math.floor(Math.random() * 1000)}`,
    kind: 'directory',
    path,
    children,
  }
}

export default App
