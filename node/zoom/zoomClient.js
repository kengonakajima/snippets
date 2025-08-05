import axios from 'axios';
import dotenv from 'dotenv';

dotenv.config();

class ZoomClient {
  constructor() {
    this.accountId = process.env.ZOOM_ACCOUNT_ID;
    this.clientId = process.env.ZOOM_CLIENT_ID;
    this.clientSecret = process.env.ZOOM_CLIENT_SECRET;
    this.organizationDomain = process.env.ZOOM_ORGANIZATION_DOMAIN;
    this.baseUrl = 'https://api.zoom.us/v2';
    this.accessToken = null;
  }

  async getAccessToken() {
    const tokenUrl = 'https://zoom.us/oauth/token';
    const authHeader = Buffer.from(`${this.clientId}:${this.clientSecret}`).toString('base64');

    const params = new URLSearchParams();
    params.append('grant_type', 'account_credentials');
    params.append('account_id', this.accountId);

    const response = await axios.post(tokenUrl, params, {
      headers: {
        'Authorization': `Basic ${authHeader}`,
        'Content-Type': 'application/x-www-form-urlencoded'
      }
    });

    this.accessToken = response.data.access_token;
    return this.accessToken;
  }

  async makeRequest(endpoint, params = {}) {
    if (!this.accessToken) {
      await this.getAccessToken();
    }

    const url = `${this.baseUrl}${endpoint}`;
    const response = await axios.get(url, {
      headers: {
        'Authorization': `Bearer ${this.accessToken}`
      },
      params
    });

    return response.data;
  }

  async getAllPages(endpoint, params = {}) {
    let allData = [];
    let nextPageToken = '';

    do {
      const requestParams = { ...params };
      if (nextPageToken) {
        requestParams.next_page_token = nextPageToken;
      }

      const response = await this.makeRequest(endpoint, requestParams);
      
      if (response.users) {
        allData = allData.concat(response.users);
      } else if (response.meetings) {
        allData = allData.concat(response.meetings);
      } else if (response.webinars) {
        allData = allData.concat(response.webinars);
      } else if (response.groups) {
        allData = allData.concat(response.groups);
      } else if (response.rooms) {
        allData = allData.concat(response.rooms);
      }

      nextPageToken = response.next_page_token || '';
    } while (nextPageToken);

    return allData;
  }
}

export default ZoomClient;