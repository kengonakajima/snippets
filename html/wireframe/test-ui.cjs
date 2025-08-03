const { chromium } = require('playwright');

async function testUI() {
    const browser = await chromium.launch({ headless: false });
    const context = await browser.newContext();
    const page = await context.newPage();

    try {
        // コンソールメッセージをすべてキャプチャ
        page.on('console', msg => {
            console.log(`コンソール[${msg.type()}]:`, msg.text());
        });
        
        // JavaScriptエラーをキャプチャ
        page.on('pageerror', error => {
            console.log('ページエラー:', error.message);
        });

        // アプリケーションにアクセス
        await page.goto('http://localhost:3000');
        
        // ページが読み込まれるまで待機
        await page.waitForLoadState('networkidle');
        
        // 初期画面のスクリーンショット
        await page.screenshot({ path: 'screenshot-initial.png', fullPage: true });
        console.log('初期画面のスクリーンショットを保存しました: screenshot-initial.png');
        
        // 左側のペインが表示されているか確認
        const leftPanes = await page.locator('.left-panes').isVisible();
        console.log('左側ペイン表示:', leftPanes);
        
        // タイムラインが表示されているか確認
        const timeline = await page.locator('.timeline-container').isVisible();
        console.log('タイムライン表示:', timeline);
        
        // 各ペインの内容を確認
        const outlineContent = await page.locator('#outline-content').textContent();
        console.log('アウトラインコンテンツ:', outlineContent ? '表示されています' : '空です');
        
        const todoContent = await page.locator('#todo-content').textContent();
        console.log('TODOコンテンツ:', todoContent ? '表示されています' : '空です');
        
        const pinnedContent = await page.locator('#pinned-content').textContent();
        console.log('ピン止めコンテンツ:', pinnedContent ? '表示されています' : '空です');
        
        // アウトラインペインをクリックしてみる
        console.log('アウトラインペインをクリックします...');
        await page.locator('#outline-content').click();
        
        // 少し待機
        await page.waitForTimeout(1000);
        
        // 全画面エディタが表示されるか確認
        const fullscreenEditor = await page.locator('#fullscreen-editor').isVisible();
        console.log('全画面エディタ表示:', fullscreenEditor);
        
        // エディタが表示された場合のスクリーンショット
        if (fullscreenEditor) {
            await page.screenshot({ path: 'screenshot-editor.png', fullPage: true });
            console.log('エディタ画面のスクリーンショットを保存しました: screenshot-editor.png');
            
            // キャンセルボタンをクリック
            await page.locator('#editor-cancel-btn').click();
            await page.waitForTimeout(500);
            
            await page.screenshot({ path: 'screenshot-after-cancel.png', fullPage: true });
            console.log('キャンセル後のスクリーンショットを保存しました: screenshot-after-cancel.png');
        }
        
        // 5秒間待機してブラウザを開いたままにする
        console.log('ブラウザを5秒間開いたままにします...');
        await page.waitForTimeout(5000);
        
    } catch (error) {
        console.error('テスト中にエラーが発生しました:', error);
        await page.screenshot({ path: 'screenshot-error.png', fullPage: true });
    } finally {
        await browser.close();
    }
}

testUI();