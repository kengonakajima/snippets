require('dotenv').config();
const axios = require('axios');

const REDMINE_URL = process.env.REDMINE_URL;
const API_KEY = process.env.REDMINE_API_KEY;

async function getAllProjects() {
  try {
    const response = await axios.get(`${REDMINE_URL}/projects.json`, {
      headers: {
        'X-Redmine-API-Key': API_KEY
      },
      params: {
        limit: 100
      }
    });
    return response.data.projects;
  } catch (error) {
    console.error('プロジェクト取得エラー:', error.message);
    throw error;
  }
}

async function getIssuesForProject(projectId, offset = 0) {
  try {
    const response = await axios.get(`${REDMINE_URL}/issues.json`, {
      headers: {
        'X-Redmine-API-Key': API_KEY
      },
      params: {
        project_id: projectId,
        limit: 100,
        offset: offset,
        status_id: '*'
      }
    });
    return response.data;
  } catch (error) {
    console.error(`プロジェクト ${projectId} のチケット取得エラー:`, error.message);
    throw error;
  }
}

async function getAllIssuesForProject(projectId) {
  let allIssues = [];
  let offset = 0;
  let hasMore = true;

  while (hasMore) {
    const data = await getIssuesForProject(projectId, offset);
    allIssues = allIssues.concat(data.issues);
    
    if (data.total_count <= offset + data.limit) {
      hasMore = false;
    } else {
      offset += data.limit;
    }
  }

  return allIssues;
}

async function getAllTickets() {
  try {
    console.log('Redmine URL:', REDMINE_URL);
    console.log('API Key:', API_KEY ? '設定済み' : '未設定');
    
    if (!REDMINE_URL || !API_KEY) {
      throw new Error('REDMINE_URLまたはREDMINE_API_KEYが設定されていません。.envファイルを確認してください。');
    }

    console.log('\nプロジェクト一覧を取得中...');
    const projects = await getAllProjects();
    console.log(`${projects.length} 個のプロジェクトが見つかりました。\n`);

    let allTickets = [];

    for (const project of projects) {
      console.log(`プロジェクト「${project.name}」(ID: ${project.id}) のチケットを取得中...`);
      const issues = await getAllIssuesForProject(project.id);
      console.log(`  → ${issues.length} 件のチケットを取得`);
      
      allTickets = allTickets.concat(issues.map(issue => ({
        ...issue,
        project_name: project.name,
        project_id: project.id
      })));
    }

    console.log(`\n合計 ${allTickets.length} 件のチケットを取得しました。`);
    
    console.log('\n取得したチケットの例（最初の5件）:');
    allTickets.slice(0, 5).forEach(ticket => {
      console.log(`- #${ticket.id}: ${ticket.subject} (プロジェクト: ${ticket.project_name})`);
    });

    return allTickets;
  } catch (error) {
    console.error('エラーが発生しました:', error.message);
    if (error.response) {
      console.error('レスポンスステータス:', error.response.status);
      console.error('レスポンスデータ:', error.response.data);
    }
    throw error;
  }
}

if (require.main === module) {
  getAllTickets()
    .then(tickets => {
      console.log('\n処理が完了しました。');
    })
    .catch(error => {
      console.error('処理中にエラーが発生しました。');
      process.exit(1);
    });
}

module.exports = {
  getAllProjects,
  getAllIssuesForProject,
  getAllTickets
};