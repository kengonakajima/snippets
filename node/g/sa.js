require('dotenv').config();
const fs = require('fs').promises;
const path = require('path');
const {google} = require('googleapis');

// スコープの定義
const SCOPES = ['https://www.googleapis.com/auth/drive.readonly'];

/**
 * サービスアカウント認証を行う
 */
async function authorize() {
  const keyPath = process.env.GOOGLE_SERVICE_ACCOUNT_KEY_PATH;
  
  if (!keyPath) {
    throw new Error('GOOGLE_SERVICE_ACCOUNT_KEY_PATH を .env ファイルに設定してください');
  }
  
  // サービスアカウントキーファイルを読み込む
  const keyFile = await fs.readFile(path.resolve(keyPath), 'utf8');
  const key = JSON.parse(keyFile);
  
  // 認証クライアントを作成
  const auth = new google.auth.GoogleAuth({
    credentials: key,
    scopes: SCOPES,
  });
  
  return auth.getClient();
}

/**
 * Google Driveのファイル一覧を取得する
 */
async function listFiles(authClient) {
  const drive = google.drive({version: 'v3', auth: authClient});
  
  // サービスアカウントがアクセスできるファイルのみ取得
  // 注: サービスアカウントでアクセスするには、ファイルをサービスアカウントのメールアドレスと共有する必要があります
  const res = await drive.files.list({
    pageSize: 100,
    fields: 'nextPageToken, files(id, name, mimeType, size, modifiedTime, owners)',
    // サービスアカウントと共有されているファイルのみ表示
    q: "trashed = false"
  });
  
  const files = res.data.files;
  if (!files || files.length === 0) {
    console.log('ファイルが見つかりませんでした。');
    console.log('\n注意: サービスアカウントでファイルにアクセスするには、');
    console.log('Google Driveでファイルまたはフォルダをサービスアカウントのメールアドレスと共有する必要があります。');
    console.log('サービスアカウントのメールアドレスは、サービスアカウントキーファイル内の "client_email" です。');
    return;
  }
  
  console.log('サービスアカウントがアクセス可能なファイル一覧:');
  console.log('=====================================');
  files.forEach((file) => {
    console.log(`📄 ${file.name}`);
    console.log(`  ID: ${file.id}`);
    console.log(`  タイプ: ${file.mimeType}`);
    console.log(`  サイズ: ${file.size ? `${(file.size / 1024 / 1024).toFixed(2)} MB` : 'N/A'}`);
    console.log(`  更新日時: ${file.modifiedTime}`);
    if (file.owners && file.owners.length > 0) {
      console.log(`  オーナー: ${file.owners[0].displayName || file.owners[0].emailAddress}`);
    }
    console.log('');
  });
}

/**
 * サービスアカウントのメールアドレスを表示
 */
async function showServiceAccountEmail() {
  const keyPath = process.env.GOOGLE_SERVICE_ACCOUNT_KEY_PATH;
  if (keyPath) {
    try {
      const keyFile = await fs.readFile(path.resolve(keyPath), 'utf8');
      const key = JSON.parse(keyFile);
      console.log(`サービスアカウントのメールアドレス: ${key.client_email}`);
      console.log('このメールアドレスとGoogle Driveのファイルを共有してください。\n');
    } catch (err) {
      console.error('サービスアカウントキーファイルの読み込みに失敗しました。');
    }
  }
}

/**
 * メイン関数
 */
async function main() {
  try {
    console.log('Google Drive サービスアカウント認証を開始します...');
    
    // サービスアカウントのメールアドレスを表示
    await showServiceAccountEmail();
    
    const authClient = await authorize();
    console.log('認証に成功しました！\n');
    
    await listFiles(authClient);
  } catch (error) {
    console.error('エラーが発生しました:', error.message);
    if (error.message.includes('GOOGLE_SERVICE_ACCOUNT_KEY_PATH')) {
      console.error('\n設定手順:');
      console.error('1. Google Cloud Consoleでサービスアカウントを作成');
      console.error('2. サービスアカウントキーをJSONファイルとしてダウンロード');
      console.error('3. .envファイルに GOOGLE_SERVICE_ACCOUNT_KEY_PATH を設定');
      console.error('   例: GOOGLE_SERVICE_ACCOUNT_KEY_PATH=./service-account-key.json');
    }
  }
}

main();