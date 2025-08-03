import dotenv from 'dotenv';
import { URL, URLSearchParams } from 'url';

dotenv.config();

const GITLAB_URL = process.env.GITLAB_URL || 'https://gitlab.com';
const GITLAB_ACCESS_TOKEN = process.env.GITLAB_ACCESS_TOKEN || process.env.GITLAB_PRIVATE_TOKEN;
const GITLAB_GROUP_ID = process.env.GITLAB_GROUP_ID;

if (!GITLAB_ACCESS_TOKEN) {
  console.error('Error: GITLAB_ACCESS_TOKEN (or GITLAB_PRIVATE_TOKEN) is not set in .env file');
  console.error('Note: Feed tokens cannot be used for API access. You need a Personal Access Token.');
  console.error('Get one from: GitLab -> User Settings -> Access Tokens');
  process.exit(1);
}

if (!GITLAB_GROUP_ID) {
  console.error('Error: GITLAB_GROUP_ID is not set in .env file');
  process.exit(1);
}

async function fetchGroupProjects(groupId, accessToken, perPage = 100) {
  const projects = [];
  let page = 1;
  let hasNextPage = true;

  while (hasNextPage) {
    const url = new URL(`${GITLAB_URL}/api/v4/groups/${encodeURIComponent(groupId)}/projects`);
    const params = new URLSearchParams({
      'per_page': perPage.toString(),
      'page': page.toString(),
      'include_subgroups': 'true',
      'order_by': 'name',
      'sort': 'asc'
    });
    
    url.search = params.toString();

    console.log(`Fetching page ${page}...`);
    console.log(`URL: ${url.toString()}`);
    
    const response = await fetch(url.toString(), {
      headers: {
        'PRIVATE-TOKEN': accessToken
      }
    });
    
    if (!response.ok) {
      const errorText = await response.text();
      console.error(`Response status: ${response.status}`);
      console.error(`Response body: ${errorText}`);
      throw new Error(`HTTP error! status: ${response.status} - ${errorText}`);
    }
    
    const data = await response.json();
    
    if (data.length === 0) {
      hasNextPage = false;
    } else {
      projects.push(...data);
      page++;
    }
    
    const totalPages = response.headers.get('x-total-pages');
    if (totalPages && page > parseInt(totalPages)) {
      hasNextPage = false;
    }
  }
  
  return projects;
}

async function main() {
  console.log(`Fetching repositories for group: ${GITLAB_GROUP_ID}`);
  console.log(`Using GitLab URL: ${GITLAB_URL}`);
  console.log('---');
  
  const projects = await fetchGroupProjects(GITLAB_GROUP_ID, GITLAB_ACCESS_TOKEN);
  
  console.log(`\nFound ${projects.length} repositories:\n`);
  
  projects.forEach((project, index) => {
    console.log(`${index + 1}. ${project.name}`);
    console.log(`   Path: ${project.path_with_namespace}`);
    console.log(`   URL: ${project.web_url}`);
    console.log(`   Description: ${project.description || 'No description'}`);
    console.log(`   Created: ${new Date(project.created_at).toLocaleDateString()}`);
    console.log(`   Last activity: ${new Date(project.last_activity_at).toLocaleDateString()}`);
    console.log('');
  });
  
  console.log('\nSummary:');
  console.log(`Total repositories: ${projects.length}`);
  
  const languages = {};
  projects.forEach(project => {
    if (project.language) {
      languages[project.language] = (languages[project.language] || 0) + 1;
    }
  });
  
  console.log('\nLanguages used:');
  Object.entries(languages)
    .sort(([, a], [, b]) => b - a)
    .forEach(([lang, count]) => {
      console.log(`  ${lang}: ${count} project(s)`);
    });
}

main().catch(error => {
  console.error('Error:', error.message);
  process.exit(1);
});