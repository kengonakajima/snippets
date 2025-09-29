import WebSocket from 'ws';
import dotenv from 'dotenv';
import { performance } from 'perf_hooks';

dotenv.config();

const API_KEY = process.env.OPENAI_API_KEY;
const REALTIME_API_URL = 'wss://api.openai.com/v1/realtime';
const MODEL = 'gpt-realtime';
const TEST_ROUNDS = 100;

class RealtimeSpeedTest {
  constructor() {
    this.ws = null;
    this.results = [];
    this.currentRound = 0;
    this.testData = null;
    this.expectedOdds = null;
    this.startTime = null;
    this.resolveResponse = null;
    this.responseText = '';
  }

  generateTestData() {
    const count = Math.floor(Math.random() * 21) + 10;
    const numbers = [];
    const odds = [];

    for (let i = 0; i < count; i++) {
      const num = Math.floor(Math.random() * 100000);
      numbers.push(num);
      if (num % 2 === 1) {
        odds.push(num);
      }
    }

    this.testData = numbers;
    this.expectedOdds = odds;

    return { numbers, expectedOdds: odds };
  }

  async connect() {
    return new Promise((resolve, reject) => {
      const headers = {
        'Authorization': `Bearer ${API_KEY}`,
        'OpenAI-Beta': 'realtime=v1'
      };

      this.ws = new WebSocket(`${REALTIME_API_URL}?model=${MODEL}`, {
        headers
      });

      this.ws.on('open', () => {
        console.log('WebSocket connection established');
        this.configureSession();
        resolve();
      });

      this.ws.on('error', (error) => {
        console.error('WebSocket error:', error);
        reject(error);
      });

      this.ws.on('message', (data) => {
        const event = JSON.parse(data.toString());
        this.handleEvent(event);
      });

      this.ws.on('close', () => {
        console.log('WebSocket connection closed');
      });
    });
  }

  configureSession() {
    const sessionConfig = {
      type: 'session.update',
      session: {
        modalities: ['text'],
        instructions: 'You will receive a JSON array of integers. Your task is to select only the odd numbers from the array and return them as a JSON array. Return only the JSON array, no explanation or additional text.',
        temperature: 0.6,
        max_response_output_tokens: 4096
      }
    };

    this.ws.send(JSON.stringify(sessionConfig));
  }

  handleEvent(event) {
    switch (event.type) {
      case 'session.updated':
        console.log('Session configured successfully');
        break;

      case 'conversation.item.created':
        if (event.item.role === 'assistant') {
          this.responseText = '';
        }
        break;

      case 'response.text.delta':
        if (event.delta) {
          this.responseText += event.delta;
        }
        break;

      case 'response.audio_transcript.delta':
        if (event.delta) {
          this.responseText += event.delta;
        }
        break;

      case 'response.done':
      case 'response.audio_transcript.done':
        if (this.resolveResponse && this.responseText) {
          this.resolveResponse(this.responseText);
          this.resolveResponse = null;
        }
        break;

      case 'error':
        console.error('API Error:', event.error);
        if (this.resolveResponse) {
          this.resolveResponse(null);
          this.resolveResponse = null;
        }
        break;
    }
  }

  async sendTestMessage() {
    return new Promise((resolve) => {
      this.resolveResponse = resolve;

      const message = {
        type: 'conversation.item.create',
        item: {
          type: 'message',
          role: 'user',
          content: [{
            type: 'input_text',
            text: JSON.stringify(this.testData)
          }]
        }
      };

      this.ws.send(JSON.stringify(message));

      const createResponse = {
        type: 'response.create',
        response: {
          modalities: ['text']
        }
      };
      this.ws.send(JSON.stringify(createResponse));

      this.startTime = performance.now();
    });
  }

  validateResponse(responseText) {
    const endTime = performance.now();
    const responseTime = endTime - this.startTime;

    let parsedResponse;
    let isCorrect = false;
    let error = null;

    if (!responseText) {
      error = 'No response received';
    } else {
      const cleanedText = responseText.trim();
      const jsonMatch = cleanedText.match(/\[[\d,\s]*\]/);

      if (jsonMatch) {
        const jsonStr = jsonMatch[0];
        parsedResponse = JSON.parse(jsonStr);

        if (Array.isArray(parsedResponse)) {
          const sortedExpected = [...this.expectedOdds].sort((a, b) => a - b);
          const sortedResponse = [...parsedResponse].sort((a, b) => a - b);

          isCorrect =
            sortedExpected.length === sortedResponse.length &&
            sortedExpected.every((val, idx) => val === sortedResponse[idx]);
        } else {
          error = 'Response is not an array';
        }
      } else {
        error = 'Could not parse JSON from response';
      }
    }

    return {
      round: this.currentRound + 1,
      responseTime,
      isCorrect,
      error,
      inputCount: this.testData.length,
      expectedCount: this.expectedOdds.length,
      responseCount: parsedResponse ? parsedResponse.length : 0
    };
  }

  async runSingleTest() {
    this.generateTestData();
    const responseText = await this.sendTestMessage();
    const result = this.validateResponse(responseText);
    this.results.push(result);

    console.log(`Round ${result.round}: ${result.isCorrect ? '✓' : '✗'} ` +
                `(${result.responseTime.toFixed(2)}ms) ` +
                `Input: ${result.inputCount} numbers, ` +
                `Expected: ${result.expectedCount} odds, ` +
                `Got: ${result.responseCount}`);

    if (result.error) {
      console.log(`  Error: ${result.error}`);
    }

    return result;
  }

  async runAllTests() {
    console.log(`\nStarting ${TEST_ROUNDS} test rounds...\n`);

    for (let i = 0; i < TEST_ROUNDS; i++) {
      this.currentRound = i;
      await this.runSingleTest();

      await new Promise(resolve => setTimeout(resolve, 500));
    }

    this.generateReport();
  }

  generateReport() {
    const correctCount = this.results.filter(r => r.isCorrect).length;
    const accuracy = (correctCount / TEST_ROUNDS) * 100;

    const responseTimes = this.results.map(r => r.responseTime);
    const avgResponseTime = responseTimes.reduce((a, b) => a + b, 0) / responseTimes.length;
    const minResponseTime = Math.min(...responseTimes);
    const maxResponseTime = Math.max(...responseTimes);

    responseTimes.sort((a, b) => a - b);
    const medianResponseTime = responseTimes[Math.floor(responseTimes.length / 2)];

    console.log('\n' + '='.repeat(60));
    console.log('                    TEST REPORT');
    console.log('='.repeat(60));
    console.log(`Total Rounds:        ${TEST_ROUNDS}`);
    console.log(`Correct Responses:   ${correctCount}`);
    console.log(`Accuracy:            ${accuracy.toFixed(2)}%`);
    console.log(`\nResponse Times:`);
    console.log(`  Average:           ${avgResponseTime.toFixed(2)}ms`);
    console.log(`  Median:            ${medianResponseTime.toFixed(2)}ms`);
    console.log(`  Minimum:           ${minResponseTime.toFixed(2)}ms`);
    console.log(`  Maximum:           ${maxResponseTime.toFixed(2)}ms`);

    const errors = this.results.filter(r => r.error);
    if (errors.length > 0) {
      console.log(`\nErrors Encountered:  ${errors.length}`);
      const errorTypes = {};
      errors.forEach(e => {
        errorTypes[e.error] = (errorTypes[e.error] || 0) + 1;
      });
      Object.entries(errorTypes).forEach(([error, count]) => {
        console.log(`  ${error}: ${count}`);
      });
    }

    console.log('='.repeat(60));
    console.log('\nDetailed results saved to: results.json');

    const fs = await import('fs');
    fs.writeFileSync('results.json', JSON.stringify({
      summary: {
        totalRounds: TEST_ROUNDS,
        correctResponses: correctCount,
        accuracy: accuracy,
        avgResponseTime: avgResponseTime,
        medianResponseTime: medianResponseTime,
        minResponseTime: minResponseTime,
        maxResponseTime: maxResponseTime
      },
      details: this.results
    }, null, 2));
  }

  async close() {
    if (this.ws) {
      this.ws.close();
    }
  }
}

async function main() {
  if (!API_KEY || API_KEY === 'your-api-key-here') {
    console.error('Please set your OPENAI_API_KEY in the .env file');
    process.exit(1);
  }

  const tester = new RealtimeSpeedTest();

  console.log('OpenAI Realtime API Speed Test');
  console.log('Testing text-to-text odd number selection');
  console.log('-'.repeat(60));

  await tester.connect();

  await new Promise(resolve => setTimeout(resolve, 1000));

  await tester.runAllTests();

  await tester.close();

  console.log('\nTest completed successfully!');
}

main().catch(console.error);