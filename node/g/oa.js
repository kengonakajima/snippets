require('dotenv').config();
const fs = require('fs').promises;
const path = require('path');
const process = require('process');
const {google} = require('googleapis');
const {OAuth2Client} = require('google-auth-library');
const http = require('http');
const url = require('url');
const { exec } = require('child_process');

// スコープの定義
const SCOPES = ['https://www.googleapis.com/auth/drive.readonly'];
// トークンを保存するファイルパス
const TOKEN_PATH = process.env.TOKEN_PATH || path.join(process.cwd(), 'token.json');

/**
 * OAuth2クライアントを作成
 */
// ローカルサーバーのポート
const PORT = 3000;

/**
 * OAuth2クライアントを作成
 */
function createOAuth2Client() {
  const clientId = process.env.GOOGLE_CLIENT_ID;
  const clientSecret = process.env.GOOGLE_CLIENT_SECRET;
  const redirectUri = process.env.GOOGLE_REDIRECT_URI || `http://localhost:${PORT}`;
  
  if (!clientId || !clientSecret) {
    throw new Error('GOOGLE_CLIENT_ID と GOOGLE_CLIENT_SECRET を .env ファイルに設定してください');
  }
  
  return new OAuth2Client(clientId, clientSecret, redirectUri);
}

/**
 * 保存されたトークンがある場合は読み込む
 */
async function loadSavedCredentialsIfExist() {
  try {
    const content = await fs.readFile(TOKEN_PATH);
    const token = JSON.parse(content);
    const oAuth2Client = createOAuth2Client();
    oAuth2Client.setCredentials(token);
    return oAuth2Client;
  } catch (err) {
    return null;
  }
}

/**
 * 認証情報をファイルに保存する
 */
async function saveCredentials(client) {
  const payload = JSON.stringify(client.credentials);
  await fs.writeFile(TOKEN_PATH, payload);
}

/**
 * 認証URLを取得してユーザーに認証を促す
 */
async function getNewToken(oAuth2Client) {
  return new Promise((resolve, reject) => {
    // 認証コードを受け取るためのローカルサーバーを作成
    const server = http.createServer(async (req, res) => {
      try {
        const queryObject = url.parse(req.url, true).query;
        if (queryObject.code) {
          res.writeHead(200, { 'Content-Type': 'text/html; charset=utf-8' });
          res.end('<h1>認証成功！</h1><p>このウィンドウを閉じて、ターミナルに戻ってください。</p>');
          server.close();
          
          // 認証コードをトークンに交換
          const {tokens} = await oAuth2Client.getToken(queryObject.code);
          oAuth2Client.setCredentials(tokens);
          await saveCredentials(oAuth2Client);
          resolve(oAuth2Client);
        } else if (queryObject.error) {
          res.writeHead(400, { 'Content-Type': 'text/html; charset=utf-8' });
          res.end(`<h1>認証エラー</h1><p>${queryObject.error}</p>`);
          server.close();
          reject(new Error(queryObject.error));
        }
      } catch (err) {
        server.close();
        reject(err);
      }
    });
    
    server.listen(PORT, 'localhost', () => {
      const authUrl = oAuth2Client.generateAuthUrl({
        access_type: 'offline',
        scope: SCOPES,
      });
      
      console.log(`認証用のローカルサーバーをポート ${PORT} で起動しました。`);
      console.log('ブラウザで認証画面を開いています...');
      console.log('\nデバッグ情報:');
      console.log('Client ID:', process.env.GOOGLE_CLIENT_ID ? '設定済み' : '未設定');
      console.log('Redirect URI:', oAuth2Client.redirectUri);
      
      // macOSのopenコマンドでブラウザを開く
      exec(`open "${authUrl}"`, (err) => {
        if (err) {
          console.log('\n自動的にブラウザを開けませんでした。');
          console.log('以下のURLを手動でブラウザで開いてください:');
          console.log(authUrl);
        }
      });
    });
  });
}

/**
 * OAuth2認証を行い、認証されたクライアントを返す
 */
async function authorize() {
  let client = await loadSavedCredentialsIfExist();
  if (client) {
    return client;
  }
  
  const oAuth2Client = createOAuth2Client();
  client = await getNewToken(oAuth2Client);
  return client;
}

/**
 * Google Driveのファイル一覧を取得する
 */
async function listFiles(authClient) {
  const drive = google.drive({version: 'v3', auth: authClient});
  const res = await drive.files.list({
    pageSize: 100,
    fields: 'nextPageToken, files(id, name, mimeType, size, modifiedTime)',
  });
  const files = res.data.files;
  if (!files || files.length === 0) {
    console.log('ファイルが見つかりませんでした。');
    return;
  }
  console.log('Google Driveのファイル一覧:');
  console.log('=====================================');
  files.forEach((file) => {
    console.log(`📄 ${file.name}`);
    console.log(`  ID: ${file.id}`);
    console.log(`  タイプ: ${file.mimeType}`);
    console.log(`  サイズ: ${file.size ? `${(file.size / 1024 / 1024).toFixed(2)} MB` : 'N/A'}`);
    console.log(`  更新日時: ${file.modifiedTime}`);
    console.log('');
  });
}

/**
 * メイン関数
 */
async function main() {
  try {
    console.log('Google Drive OAuth認証を開始します...');
    const authClient = await authorize();
    console.log('認証に成功しました！');
    console.log('');
    await listFiles(authClient);
  } catch (error) {
    console.error('エラーが発生しました:', error.message);
    if (error.message.includes('.env')) {
      console.error('\n設定手順:');
      console.error('1. .env.sample を .env にコピー');
      console.error('2. Google Cloud Consoleで OAuth 2.0 クライアントIDを作成');
      console.error('3. クライアントIDとシークレットを .env に設定');
    }
  }
}

main();