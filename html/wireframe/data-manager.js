/**
 * データ管理システム - ローカルストレージとJSONファイルでのデータ永続化
 * 要件: 2.2, 2.3, 3.4, 4.2, 4.3
 */

class DataManager {
    constructor() {
        this.storageKeys = {
            markdown: 'project-timeline-markdown',
            posts: 'project-timeline-posts',
            config: 'project-timeline-config'
        };
        
        this.defaultMarkdown = {
            outline: `# プロジェクト概要

このプロジェクトは、チーム内での情報共有とコミュニケーションを円滑にするためのタイムラインシステムです。

## 主な機能

- リアルタイムでの投稿共有
- プロジェクト別の情報整理
- Markdownによる柔軟な記述
- タスク管理機能

## 技術スタック

- フロントエンド: HTML/CSS/JavaScript
- Markdownパーサー: marked.js
- デザイン: レスポンシブ対応`,
            
            todo: `## 今週のタスク

- [ ] ユーザー認証機能の実装
- [ ] データベース設計の見直し
- [x] UIモックアップの作成
- [ ] APIエンドポイントの設計
- [ ] テストケースの作成

## 来週の予定

- プロトタイプのレビュー
- ステークホルダーへのプレゼン
- フィードバックの収集と反映`,
            
            pinned: `## 重要な情報

### プロジェクトのゴール

> チーム全体の生産性を20%向上させる

### 連絡先

- PM: 田中太郎 (tanaka@example.com)
- Tech Lead: 佐藤花子 (sato@example.com)
- Designer: 山田次郎 (yamada@example.com)

### 参考リンク

- [プロジェクト仕様書](https://example.com/spec)
- [デザインガイドライン](https://example.com/design)
- [API ドキュメント](https://example.com/api)`
        };
        
        this.defaultConfig = {
            user: {
                name: "ユーザー",
                avatar: "U"
            },
            autoSave: true,
            maxPosts: 1000
        };
        
        this.init();
    }
    
    /**
     * データ管理システムの初期化
     */
    init() {
        try {
            // localStorage の可用性をチェック
            if (!this.isLocalStorageAvailable()) {
                console.warn('localStorage is not available. Data will not persist.');
                return;
            }
            
            // 初期データの設定
            this.initializeDefaultData();
            
            console.log('DataManager initialized successfully');
        } catch (error) {
            console.error('Failed to initialize DataManager:', error);
        }
    }
    
    /**
     * localStorage の可用性をチェック
     */
    isLocalStorageAvailable() {
        try {
            const test = '__localStorage_test__';
            localStorage.setItem(test, test);
            localStorage.removeItem(test);
            return true;
        } catch (e) {
            return false;
        }
    }
    
    /**
     * 初期データの設定
     */
    initializeDefaultData() {
        // Markdownコンテンツの初期化
        if (!localStorage.getItem(this.storageKeys.markdown)) {
            this.saveMarkdownContent('all', this.defaultMarkdown);
        }
        
        // 投稿データの初期化
        if (!localStorage.getItem(this.storageKeys.posts)) {
            this.savePosts([]);
        }
        
        // 設定データの初期化
        if (!localStorage.getItem(this.storageKeys.config)) {
            this.saveConfig(this.defaultConfig);
        }
    }
    
    /**
     * Markdownコンテンツの保存
     * @param {string} pane - 'outline', 'todo', 'pinned', または 'all'
     * @param {string|object} content - 保存するコンテンツ
     */
    saveMarkdownContent(pane, content) {
        try {
            let currentContent = this.loadMarkdownContent('all') || this.defaultMarkdown;
            
            if (pane === 'all') {
                currentContent = typeof content === 'object' ? content : this.defaultMarkdown;
            } else if (['outline', 'todo', 'pinned'].includes(pane)) {
                currentContent[pane] = content;
            } else {
                throw new Error(`Invalid pane: ${pane}`);
            }
            
            localStorage.setItem(this.storageKeys.markdown, JSON.stringify(currentContent));
            
            // JSONファイルへの永続化も試行（サーバーサイドで処理）
            this.persistToJSON('markdown', currentContent);
            
            return true;
        } catch (error) {
            console.error('Failed to save markdown content:', error);
            return false;
        }
    }
    
    /**
     * Markdownコンテンツの読み込み
     * @param {string} pane - 'outline', 'todo', 'pinned', または 'all'
     * @returns {string|object|null} 読み込んだコンテンツ
     */
    loadMarkdownContent(pane = 'all') {
        try {
            const stored = localStorage.getItem(this.storageKeys.markdown);
            const content = stored ? JSON.parse(stored) : this.defaultMarkdown;
            
            if (pane === 'all') {
                return content;
            } else if (['outline', 'todo', 'pinned'].includes(pane)) {
                return content[pane] || this.defaultMarkdown[pane];
            } else {
                throw new Error(`Invalid pane: ${pane}`);
            }
        } catch (error) {
            console.error('Failed to load markdown content:', error);
            return pane === 'all' ? this.defaultMarkdown : this.defaultMarkdown[pane];
        }
    }
    
    /**
     * 新しい投稿の保存
     * @param {object} post - 投稿オブジェクト
     * @returns {boolean} 保存成功の可否
     */
    savePost(post) {
        try {
            const posts = this.loadPosts();
            
            // 投稿オブジェクトの検証と補完
            const newPost = {
                id: post.id || this.generateId(),
                content: post.content || '',
                author: post.author || this.getConfig().user.name,
                avatar: post.avatar || this.getConfig().user.avatar,
                timestamp: post.timestamp || new Date().toISOString(),
                likes: post.likes || 0,
                comments: post.comments || 0,
                shares: post.shares || 0
            };
            
            // 新しい投稿を先頭に追加
            posts.unshift(newPost);
            
            // 最大投稿数の制限
            const maxPosts = this.getConfig().maxPosts || 1000;
            if (posts.length > maxPosts) {
                posts.splice(maxPosts);
            }
            
            return this.savePosts(posts);
        } catch (error) {
            console.error('Failed to save post:', error);
            return false;
        }
    }
    
    /**
     * 全投稿の保存
     * @param {array} posts - 投稿配列
     * @returns {boolean} 保存成功の可否
     */
    savePosts(posts) {
        try {
            localStorage.setItem(this.storageKeys.posts, JSON.stringify(posts));
            
            // JSONファイルへの永続化も試行
            this.persistToJSON('posts', posts);
            
            return true;
        } catch (error) {
            console.error('Failed to save posts:', error);
            return false;
        }
    }
    
    /**
     * 投稿一覧の読み込み
     * @returns {array} 投稿配列
     */
    loadPosts() {
        try {
            const stored = localStorage.getItem(this.storageKeys.posts);
            return stored ? JSON.parse(stored) : [];
        } catch (error) {
            console.error('Failed to load posts:', error);
            return [];
        }
    }
    
    /**
     * 特定の投稿を削除
     * @param {string} postId - 削除する投稿のID
     * @returns {boolean} 削除成功の可否
     */
    deletePost(postId) {
        try {
            const posts = this.loadPosts();
            const filteredPosts = posts.filter(post => post.id !== postId);
            
            if (filteredPosts.length === posts.length) {
                console.warn(`Post with ID ${postId} not found`);
                return false;
            }
            
            return this.savePosts(filteredPosts);
        } catch (error) {
            console.error('Failed to delete post:', error);
            return false;
        }
    }
    
    /**
     * 投稿の更新
     * @param {string} postId - 更新する投稿のID
     * @param {object} updates - 更新内容
     * @returns {boolean} 更新成功の可否
     */
    updatePost(postId, updates) {
        try {
            const posts = this.loadPosts();
            const postIndex = posts.findIndex(post => post.id === postId);
            
            if (postIndex === -1) {
                console.warn(`Post with ID ${postId} not found`);
                return false;
            }
            
            posts[postIndex] = { ...posts[postIndex], ...updates };
            return this.savePosts(posts);
        } catch (error) {
            console.error('Failed to update post:', error);
            return false;
        }
    }
    
    /**
     * 設定の保存
     * @param {object} config - 設定オブジェクト
     * @returns {boolean} 保存成功の可否
     */
    saveConfig(config) {
        try {
            const mergedConfig = { ...this.defaultConfig, ...config };
            localStorage.setItem(this.storageKeys.config, JSON.stringify(mergedConfig));
            
            // JSONファイルへの永続化も試行
            this.persistToJSON('config', mergedConfig);
            
            return true;
        } catch (error) {
            console.error('Failed to save config:', error);
            return false;
        }
    }
    
    /**
     * 設定の読み込み
     * @returns {object} 設定オブジェクト
     */
    getConfig() {
        try {
            const stored = localStorage.getItem(this.storageKeys.config);
            return stored ? JSON.parse(stored) : this.defaultConfig;
        } catch (error) {
            console.error('Failed to load config:', error);
            return this.defaultConfig;
        }
    }
    
    /**
     * 全データのエクスポート
     * @returns {object} 全データ
     */
    exportAllData() {
        return {
            markdown: this.loadMarkdownContent('all'),
            posts: this.loadPosts(),
            config: this.getConfig(),
            exportDate: new Date().toISOString()
        };
    }
    
    /**
     * データのインポート
     * @param {object} data - インポートするデータ
     * @returns {boolean} インポート成功の可否
     */
    importData(data) {
        try {
            if (data.markdown) {
                this.saveMarkdownContent('all', data.markdown);
            }
            
            if (data.posts && Array.isArray(data.posts)) {
                this.savePosts(data.posts);
            }
            
            if (data.config) {
                this.saveConfig(data.config);
            }
            
            console.log('Data imported successfully');
            return true;
        } catch (error) {
            console.error('Failed to import data:', error);
            return false;
        }
    }
    
    /**
     * 全データのクリア
     * @returns {boolean} クリア成功の可否
     */
    clearAllData() {
        try {
            Object.values(this.storageKeys).forEach(key => {
                localStorage.removeItem(key);
            });
            
            // デフォルトデータで再初期化
            this.initializeDefaultData();
            
            console.log('All data cleared and reinitialized');
            return true;
        } catch (error) {
            console.error('Failed to clear data:', error);
            return false;
        }
    }
    
    /**
     * ストレージ使用量の取得
     * @returns {object} ストレージ使用量情報
     */
    getStorageInfo() {
        try {
            let totalSize = 0;
            const details = {};
            
            Object.entries(this.storageKeys).forEach(([name, key]) => {
                const data = localStorage.getItem(key);
                const size = data ? new Blob([data]).size : 0;
                details[name] = {
                    size: size,
                    sizeFormatted: this.formatBytes(size)
                };
                totalSize += size;
            });
            
            return {
                total: {
                    size: totalSize,
                    sizeFormatted: this.formatBytes(totalSize)
                },
                details: details,
                available: this.isLocalStorageAvailable()
            };
        } catch (error) {
            console.error('Failed to get storage info:', error);
            return null;
        }
    }
    
    /**
     * JSONファイルへの永続化（サーバーサイドAPI呼び出し）
     * @param {string} type - データタイプ
     * @param {any} data - 保存するデータ
     */
    async persistToJSON(type, data) {
        try {
            // サーバーサイドのAPIエンドポイントに送信
            const response = await fetch('/api/save-data', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify({
                    type: type,
                    data: data,
                    timestamp: new Date().toISOString()
                })
            });
            
            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }
            
            console.log(`Data persisted to JSON file: ${type}`);
        } catch (error) {
            // JSONファイル永続化の失敗は警告レベル（localStorageは成功している）
            console.warn('Failed to persist to JSON file:', error.message);
        }
    }
    
    /**
     * ユニークIDの生成
     * @returns {string} ユニークID
     */
    generateId() {
        return Date.now().toString(36) + Math.random().toString(36).substr(2);
    }
    
    /**
     * バイト数のフォーマット
     * @param {number} bytes - バイト数
     * @returns {string} フォーマットされた文字列
     */
    formatBytes(bytes) {
        if (bytes === 0) return '0 Bytes';
        
        const k = 1024;
        const sizes = ['Bytes', 'KB', 'MB', 'GB'];
        const i = Math.floor(Math.log(bytes) / Math.log(k));
        
        return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i];
    }
}

// グローバルインスタンスの作成
const dataManager = new DataManager();

// モジュールとしてエクスポート（Node.js環境用）
if (typeof module !== 'undefined' && module.exports) {
    module.exports = DataManager;
}

// ブラウザ環境用のグローバル変数
if (typeof window !== 'undefined') {
    window.DataManager = DataManager;
    window.dataManager = dataManager;
}