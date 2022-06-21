const request = require('request');
const url = 'https://staging-app.guardians.city/';
const options = {
  url: url,
  method: 'GET',
  headers: {
    'Accept': 'application/json'
  }
};

request(options, (error, response, body) => {
    console.log("error:",error, response);
});

