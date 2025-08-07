const chokidar = require('chokidar');
const path = require('path');

// 現在のディレクトリを監視対象に設定
const watcher = chokidar.watch('.', {
  ignored: /(^|[\/\\])\../, // dotファイルを無視
  persistent: true,
  ignoreInitial: true, // 初回のファイル読み込みイベントを無視
  awaitWriteFinish: {
    stabilityThreshold: 300,
    pollInterval: 100
  }
});

// イベントの監視
watcher
  .on('add', filePath => {
    console.log(`[追加] ${path.resolve(filePath)}`);
  })
  .on('change', filePath => {
    console.log(`[変更] ${path.resolve(filePath)}`);
  })
  .on('unlink', filePath => {
    console.log(`[削除] ${path.resolve(filePath)}`);
  })
  .on('addDir', dirPath => {
    console.log(`[ディレクトリ追加] ${path.resolve(dirPath)}`);
  })
  .on('unlinkDir', dirPath => {
    console.log(`[ディレクトリ削除] ${path.resolve(dirPath)}`);
  })
  .on('error', error => {
    console.error(`[エラー] ${error}`);
  })
  .on('ready', () => {
    console.log('ファイル監視を開始しました。変更を監視中...');
    console.log('終了するには Ctrl+C を押してください。');
  });

// プロセス終了時のクリーンアップ
process.on('SIGINT', () => {
  console.log('\n監視を終了します...');
  watcher.close().then(() => {
    console.log('監視を終了しました。');
    process.exit(0);
  });
});