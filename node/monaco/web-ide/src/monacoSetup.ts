import EditorWorker from 'monaco-editor/esm/vs/editor/editor.worker?worker'

type MonacoEnvironment = {
  getWorker(moduleId: string, label: string): Worker
}

const globalScope = self as unknown as {
  MonacoEnvironment?: MonacoEnvironment
}

if (!globalScope.MonacoEnvironment) {
  globalScope.MonacoEnvironment = {
    getWorker(_moduleId, _label) {
      return new EditorWorker()
    },
  }
}
