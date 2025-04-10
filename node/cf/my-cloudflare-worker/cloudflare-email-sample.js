const axios = require('axios');
const fs = require('fs');

// config.jsonから設定を読み込む
const config = JSON.parse(fs.readFileSync('config.json', 'utf8'));
const CLOUDFLARE_API_TOKEN = config.CLOUDFLARE_API_TOKEN;
const CLOUDFLARE_ACCOUNT_ID = config.CLOUDFLARE_ACCOUNT_ID;

// Email details
const emailData = {
  personalizations: [
    {
      to: [{ email: 'recipient@example.com' }],
      subject: 'Hello from Cloudflare Email API',
    },
  ],
  from: { email: 'sender@example.com' },
  content: [
    {
      type: 'text/plain',
      value: 'This is a test email sent using Cloudflare Email API.',
    },
  ],
};

// Send email function
async function sendEmail() {
  try {
    const response = await axios.post(
      `https://api.cloudflare.com/client/v4/accounts/${CLOUDFLARE_ACCOUNT_ID}/email/routing/v1/messages`,
      emailData,
      {
        headers: {
          Authorization: `Bearer ${CLOUDFLARE_API_TOKEN}`,
          'Content-Type': 'application/json',
        },
      }
    );
    console.log('Email sent successfully:', response.data);
  } catch (error) {
    console.error('Error sending email:', error.response.data);
  }
}

// Call the function to send the email
sendEmail();
