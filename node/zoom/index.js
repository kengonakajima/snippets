import ZoomClient from './zoomClient.js';

async function fetchAllOrganizationResources() {
  console.log('Zoom Organization Resources Fetcher');
  console.log('=====================================\n');

  const client = new ZoomClient();
  
  console.log(`Organization: https://${client.organizationDomain}.zoom.us/\n`);

  const resources = {
    users: [],
    meetings: [],
    webinars: [],
    groups: [],
    rooms: []
  };

  const apiStatus = {
    authentication: { success: false, error: null },
    users: { success: false, error: null },
    groups: { success: false, error: null },
    rooms: { success: false, error: null },
    meetings: { success: false, error: null },
    webinars: { success: false, error: null }
  };

  // Authentication
  try {
    await client.getAccessToken();
    console.log('✓ Authentication successful\n');
    apiStatus.authentication.success = true;
  } catch (error) {
    console.error('✗ Authentication failed:', error.message);
    if (error.response) {
      console.error('  Response data:', error.response.data);
    }
    apiStatus.authentication.error = error.message;
    console.log('\nContinuing with other tests...\n');
  }

  // Users API
  try {
    console.log('Testing Users API...');
    resources.users = await client.getAllPages('/users', { page_size: 300 });
    console.log(`✓ Users API works! Found ${resources.users.length} users`);
    apiStatus.users.success = true;
  } catch (error) {
    console.error('✗ Users API failed:', error.message);
    if (error.response?.data) {
      console.error('  Details:', JSON.stringify(error.response.data, null, 2));
    }
    apiStatus.users.error = error.message;
  }

  // Groups API
  try {
    console.log('\nTesting Groups API...');
    resources.groups = await client.getAllPages('/groups', { page_size: 300 });
    console.log(`✓ Groups API works! Found ${resources.groups.length} groups`);
    apiStatus.groups.success = true;
  } catch (error) {
    console.error('✗ Groups API failed:', error.message);
    if (error.response?.data) {
      console.error('  Details:', JSON.stringify(error.response.data, null, 2));
    }
    apiStatus.groups.error = error.message;
  }

  // Rooms API
  try {
    console.log('\nTesting Rooms API...');
    resources.rooms = await client.getAllPages('/rooms', { page_size: 300 });
    console.log(`✓ Rooms API works! Found ${resources.rooms.length} rooms`);
    apiStatus.rooms.success = true;
  } catch (error) {
    console.error('✗ Rooms API failed:', error.message);
    if (error.response?.data) {
      console.error('  Details:', JSON.stringify(error.response.data, null, 2));
    }
    apiStatus.rooms.error = error.message;
  }

  // Meetings API (only if we have users)
  if (resources.users.length > 0) {
    try {
      console.log('\nTesting Meetings API...');
      let totalMeetings = 0;
      let testedUsers = 0;
      
      for (const user of resources.users.slice(0, 3)) { // Test with first 3 users only
        testedUsers++;
        try {
          const userMeetings = await client.makeRequest(`/users/${user.id}/meetings`, {
            type: 'scheduled',
            page_size: 300
          });
          if (userMeetings.meetings) {
            resources.meetings = resources.meetings.concat(userMeetings.meetings);
            totalMeetings += userMeetings.meetings.length;
          }
        } catch (error) {
          if (error.response?.status !== 404) {
            console.error(`  Error for user ${user.email}:`, error.message);
          }
        }
      }
      console.log(`✓ Meetings API works! Found ${totalMeetings} meetings from ${testedUsers} users`);
      apiStatus.meetings.success = true;
    } catch (error) {
      console.error('✗ Meetings API failed:', error.message);
      apiStatus.meetings.error = error.message;
    }
  }

  // Webinars API (only if we have users)
  if (resources.users.length > 0) {
    try {
      console.log('\nTesting Webinars API...');
      let totalWebinars = 0;
      let testedUsers = 0;
      
      for (const user of resources.users.slice(0, 3)) { // Test with first 3 users only
        testedUsers++;
        try {
          const userWebinars = await client.makeRequest(`/users/${user.id}/webinars`, {
            page_size: 300
          });
          if (userWebinars.webinars) {
            resources.webinars = resources.webinars.concat(userWebinars.webinars);
            totalWebinars += userWebinars.webinars.length;
          }
        } catch (error) {
          if (error.response?.status !== 404 && error.response?.status !== 400) {
            console.error(`  Error for user ${user.email}:`, error.message);
          }
        }
      }
      console.log(`✓ Webinars API works! Found ${totalWebinars} webinars from ${testedUsers} users`);
      apiStatus.webinars.success = true;
    } catch (error) {
      console.error('✗ Webinars API failed:', error.message);
      apiStatus.webinars.error = error.message;
    }
  }

  // API Status Summary
  console.log('\n=====================================');
  console.log('API Status Summary:');
  console.log('=====================================');
  
  Object.entries(apiStatus).forEach(([api, status]) => {
    const statusIcon = status.success ? '✓' : '✗';
    const statusText = status.success ? 'Working' : 'Failed';
    console.log(`${statusIcon} ${api.charAt(0).toUpperCase() + api.slice(1)}: ${statusText}`);
    if (!status.success && status.error) {
      console.log(`  Error: ${status.error}`);
    }
  });

  // Resource Summary
  console.log('\n=====================================');
  console.log('Resources Found:');
  console.log('=====================================');
  console.log(`- Users: ${resources.users.length}`);
  console.log(`- Meetings: ${resources.meetings.length}`);
  console.log(`- Webinars: ${resources.webinars.length}`);
  console.log(`- Groups: ${resources.groups.length}`);
  console.log(`- Rooms: ${resources.rooms.length}`);

  // Detailed Resources (only if found)
  console.log('\n=====================================');
  console.log('Detailed Resources:\n');

  if (resources.users.length > 0) {
    console.log('USERS:');
    resources.users.slice(0, 5).forEach(user => {
      console.log(`  - ${user.email} (${user.first_name} ${user.last_name}) - Type: ${user.type}, Status: ${user.status}`);
    });
    if (resources.users.length > 5) {
      console.log(`  ... and ${resources.users.length - 5} more users`);
    }
  }

  if (resources.meetings.length > 0) {
    console.log('\nMEETINGS:');
    resources.meetings.slice(0, 5).forEach(meeting => {
      console.log(`  - "${meeting.topic}" - Host: ${meeting.host_email}, Start: ${meeting.start_time}`);
    });
    if (resources.meetings.length > 5) {
      console.log(`  ... and ${resources.meetings.length - 5} more meetings`);
    }
  }

  if (resources.webinars.length > 0) {
    console.log('\nWEBINARS:');
    resources.webinars.slice(0, 5).forEach(webinar => {
      console.log(`  - "${webinar.topic}" - Host: ${webinar.host_email}, Start: ${webinar.start_time}`);
    });
    if (resources.webinars.length > 5) {
      console.log(`  ... and ${resources.webinars.length - 5} more webinars`);
    }
  }

  if (resources.groups.length > 0) {
    console.log('\nGROUPS:');
    resources.groups.slice(0, 5).forEach(group => {
      console.log(`  - ${group.name} - Members: ${group.total_members}`);
    });
    if (resources.groups.length > 5) {
      console.log(`  ... and ${resources.groups.length - 5} more groups`);
    }
  }

  if (resources.rooms.length > 0) {
    console.log('\nROOMS:');
    resources.rooms.slice(0, 5).forEach(room => {
      console.log(`  - ${room.name} - Location: ${room.location_id}`);
    });
    if (resources.rooms.length > 5) {
      console.log(`  ... and ${resources.rooms.length - 5} more rooms`);
    }
  }

  return { resources, apiStatus };
}

fetchAllOrganizationResources()
  .then(({ apiStatus }) => {
    const workingAPIs = Object.entries(apiStatus).filter(([_, status]) => status.success).length;
    const totalAPIs = Object.keys(apiStatus).length;
    
    console.log('\n=====================================');
    console.log(`Summary: ${workingAPIs}/${totalAPIs} APIs are working`);
    console.log('=====================================');
    
    if (workingAPIs === 0) {
      console.log('\n✗ No APIs are working. Please check your scopes and permissions.');
    } else if (workingAPIs < totalAPIs) {
      console.log('\n⚠ Some APIs are working. Check the errors above for missing scopes.');
    } else {
      console.log('\n✓ All APIs are working successfully!');
    }
  })
  .catch(error => {
    console.error('\n✗ Unexpected error:', error.message);
  });