require('dotenv').config();

const { App } = require("@octokit/app");
const { Octokit } = require("@octokit/rest");

// GitHub Appの認証情報
const APP_ID = process.env.GITHUB_APP_ID;
const PRIVATE_KEY = process.env.GITHUB_APP_PRIVATE_KEY;
const ORGANIZATION = process.env.GITHUB_ORG || 'your-org-name';

async function getOrgRepositories() {
  // GitHub Appインスタンスを作成
  const app = new App({
    appId: APP_ID,
    privateKey: PRIVATE_KEY,
  });

  // 組織のインストール情報を取得
  const { data: installations } = await app.octokit.request('GET /app/installations');
  
  // 対象組織のインストールを検索
  const targetInstallation = installations.find(
    installation => installation.account.login === ORGANIZATION
  );

  if (!targetInstallation) {
    throw new Error(`GitHub App is not installed for organization: ${ORGANIZATION}`);
  }

  // Installation用のOctokitインスタンスを取得
  const octokit = await app.getInstallationOctokit(targetInstallation.id);

  // 組織のリポジトリリストを取得（ページネーション対応）
  const repositories = await octokit.paginate(octokit.rest.repos.listForOrg, {
    org: ORGANIZATION,
    type: 'all', // 'all', 'public', 'private', 'forks', 'sources', 'member'
    sort: 'updated', // 'created', 'updated', 'pushed', 'full_name'
    per_page: 100,
  });

  return repositories;
}

// メイン実行
async function main() {
  try {
    // 必須の環境変数チェック
    if (!APP_ID || !PRIVATE_KEY) {
      throw new Error('Missing required environment variables. Please check your .env file.');
    }
    
    console.log(`Fetching repositories for organization: ${ORGANIZATION}`);
    
    const repos = await getOrgRepositories();
    
    console.log(`\nFound ${repos.length} repositories:\n`);
    
    // リポジトリ情報を表示
    repos.forEach((repo, index) => {
      console.log(`${index + 1}. ${repo.full_name}`);
      console.log(`   - Description: ${repo.description || 'No description'}`);
      console.log(`   - Private: ${repo.private}`);
      console.log(`   - Updated: ${new Date(repo.updated_at).toLocaleDateString()}`);
      console.log(`   - Language: ${repo.language || 'Not specified'}`);
      console.log(`   - Stars: ${repo.stargazers_count}`);
      console.log('');
    });

    // 統計情報
    const privateRepos = repos.filter(repo => repo.private).length;
    const publicRepos = repos.filter(repo => !repo.private).length;
    
    console.log('Summary:');
    console.log(`- Total repositories: ${repos.length}`);
    console.log(`- Public repositories: ${publicRepos}`);
    console.log(`- Private repositories: ${privateRepos}`);
    
  } catch (error) {
    console.error('Error:', error.message);
    if (error.response) {
      console.error('API Response:', error.response.data);
    }
  }
}

// エラーハンドリング付きで実行
if (require.main === module) {
  main().catch(console.error);
}

module.exports = { getOrgRepositories };