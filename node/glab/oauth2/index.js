require('dotenv').config();
const express = require('express');
const axios = require('axios');
const crypto = require('crypto');

const app = express();
const port = process.env.PORT || 3000;

const gitlabBaseUrl = process.env.GITLAB_BASE_URL || 'https://gitlab.com';
const clientId = process.env.GITLAB_APP_ID;
const clientSecret = process.env.GITLAB_APP_SECRET;
const redirectUri = process.env.GITLAB_REDIRECT_URI;

if (!clientId || !clientSecret || !redirectUri) {
  console.error('Missing required environment variables. Please check your .env file.');
  process.exit(1);
}

const state = crypto.randomBytes(16).toString('hex');

app.get('/', (req, res) => {
  res.send(`
    <h1>GitLab OAuth2 Repository List Demo</h1>
    <a href="/auth">Connect to GitLab</a>
  `);
});

app.get('/auth', (req, res) => {
  console.log('Auth route accessed');
  const authUrl = `${gitlabBaseUrl}/oauth/authorize?` +
    `client_id=${clientId}&` +
    `redirect_uri=${encodeURIComponent(redirectUri)}&` +
    `response_type=code&` +
    `state=${state}&` +
    `scope=read_api`;
  
  console.log('Redirecting to:', authUrl);
  res.redirect(authUrl);
});

app.get('/callback', async (req, res) => {
  console.log('Callback route accessed');
  console.log('Query params:', req.query);
  
  const { code, state: returnedState } = req.query;
  
  if (returnedState !== state) {
    console.error('State mismatch:', { expected: state, received: returnedState });
    return res.status(400).send('State mismatch error');
  }
  
  if (!code) {
    console.error('No authorization code provided');
    return res.status(400).send('Authorization code not provided');
  }
  
  console.log('Authorization code received:', code);
  
  try {
    console.log('Exchanging code for token...');
    const tokenRequestData = {
      client_id: clientId,
      client_secret: clientSecret,
      code: code,
      grant_type: 'authorization_code',
      redirect_uri: redirectUri
    };
    console.log('Token request URL:', `${gitlabBaseUrl}/oauth/token`);
    
    const tokenResponse = await axios.post(`${gitlabBaseUrl}/oauth/token`, tokenRequestData);
    
    console.log('Token response received:', tokenResponse.data);
    const accessToken = tokenResponse.data.access_token;
    console.log('Access token obtained successfully');
    
    const projectsResponse = await axios.get(`${gitlabBaseUrl}/api/v4/projects`, {
      headers: {
        'Authorization': `Bearer ${accessToken}`
      },
      params: {
        membership: true,
        simple: false,
        per_page: 100
      }
    });
    
    console.log('API Response:', projectsResponse.data);
    console.log('Number of projects:', projectsResponse.data.length);
    
    const projects = projectsResponse.data;
    
    let html = '<h1>Your GitLab Repositories</h1>';
    
    if (projects.length === 0) {
      html += '<p>No repositories found. Make sure you have access to repositories in GitLab.</p>';
    } else {
      html += '<ul>';
      projects.forEach(project => {
        html += `<li><strong>${project.name}</strong> - ${project.web_url}<br>`;
        html += `<small>Visibility: ${project.visibility}, Path: ${project.path_with_namespace}</small></li>`;
      });
      html += '</ul>';
    }
    
    html += '<a href="/">Back to home</a>';
    
    res.send(html);
    
  } catch (error) {
    console.error('Error details:');
    console.error('Status:', error.response?.status);
    console.error('Status text:', error.response?.statusText);
    console.error('Response data:', error.response?.data);
    console.error('Error message:', error.message);
    console.error('Full error:', error);
    res.status(500).send('An error occurred while fetching repositories. Check server logs for details.');
  }
});

app.listen(port, () => {
  console.log(`Server running at http://localhost:${port}`);
  console.log(`Make sure to set up your GitLab OAuth application with redirect URI: ${redirectUri}`);
});