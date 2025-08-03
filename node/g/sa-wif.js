require('dotenv').config();
const {google} = require('googleapis');

// スコープの定義
const SCOPES = ['https://www.googleapis.com/auth/drive.readonly'];

/**
 * Workload Identity連携を使用した認証
 * 環境変数 GOOGLE_APPLICATION_CREDENTIALS が設定されている必要があります
 */
async function authorize() {
  // Google Auth Libraryは自動的に以下の順序で認証を試みます：
  // 1. GOOGLE_APPLICATION_CREDENTIALS 環境変数が指すクレデンシャルファイル
  // 2. gcloud auth application-default login で設定された認証情報
  // 3. Compute Engine、Cloud Run、Cloud Functions等のメタデータサービス
  // 4. Workload Identity連携（GitHub Actions等で自動設定）
  
  const auth = new google.auth.GoogleAuth({
    scopes: SCOPES,
  });
  
  // 現在の認証情報を確認
  const client = await auth.getClient();
  const projectId = await auth.getProjectId().catch(() => 'プロジェクトID未設定');
  
  console.log('認証タイプ:', client.constructor.name);
  console.log('プロジェクトID:', projectId);
  
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
    q: "trashed = false"
  });
  
  const files = res.data.files;
  if (!files || files.length === 0) {
    console.log('ファイルが見つかりませんでした。');
    console.log('\n注意: Workload Identity連携でファイルにアクセスするには、');
    console.log('サービスアカウントにGoogle Driveへのアクセス権限が必要です。');
    return;
  }
  
  console.log('アクセス可能なファイル一覧:');
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
    console.log('Google Drive Workload Identity連携認証を開始します...');
    
    // 環境変数の確認
    if (process.env.GOOGLE_APPLICATION_CREDENTIALS) {
      console.log('GOOGLE_APPLICATION_CREDENTIALS:', process.env.GOOGLE_APPLICATION_CREDENTIALS);
    } else if (process.env.GITHUB_ACTIONS) {
      console.log('GitHub Actions環境を検出しました。');
    } else {
      console.log('ローカル環境で実行中です。gcloud auth application-default login を使用してください。');
    }
    
    const authClient = await authorize();
    console.log('認証に成功しました！\n');
    
    await listFiles(authClient);
  } catch (error) {
    console.error('エラーが発生しました:', error.message);
    console.error('\nWorkload Identity連携の設定を確認してください：');
    console.error('1. Google Cloud ConsoleでWorkload Identity Poolを作成');
    console.error('2. GitHub等の外部IDプロバイダーを設定');
    console.error('3. サービスアカウントにWorkload Identity Userロールを付与');
    console.error('4. CI/CD環境で適切な環境変数を設定');
  }
}

main();