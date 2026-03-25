const { app, BrowserWindow, desktopCapturer, session } = require('electron');
const path = require('path');

// macOSでシステムオーディオループバックを有効化（app.whenReady前に呼ぶ必要がある）
app.commandLine.appendSwitch('enable-features',
  'MacLoopbackAudioForScreenShare,MacSckSystemAudioLoopbackOverride');
// CoreAudio Tap方式を無効化（Electron 41のバグ回避）
app.commandLine.appendSwitch('disable-features',
  'MacCatapLoopbackAudioForScreenShare');

function createWindow() {
  const win = new BrowserWindow({
    width: 400,
    height: 300,
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
    },
  });

  // setDisplayMediaRequestHandler で audio: 'loopback' を指定
  session.defaultSession.setDisplayMediaRequestHandler(async (request, callback) => {
    const sources = await desktopCapturer.getSources({ types: ['screen'] });
    callback({ video: sources[0], audio: 'loopback' });
  }, { useSystemPicker: false });

  win.loadFile('index.html');
}

app.whenReady().then(createWindow);

app.on('window-all-closed', () => {
  app.quit();
});
