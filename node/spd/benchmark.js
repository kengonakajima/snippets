#!/usr/bin/env node

const axios = require('axios');
const { Command } = require('commander');
const fs = require('fs');
const path = require('path');
require('dotenv').config();

const program = new Command();

program
  .name('llm-speed-test')
  .description('LLM API Speed Benchmark Tool')
  .version('1.0.0')
  .requiredOption('-m, --model <model>', 'Model name (e.g., gpt-3.5-turbo, gpt-4)')
  .requiredOption('-e, --endpoint <endpoint>', 'API endpoint URL')
  .option('-k, --key-env <env>', 'Environment variable name for API key (default: OPENAI_API_KEY)', 'OPENAI_API_KEY')
  .option('-n, --iterations <number>', 'Number of test iterations', '100')
  .option('-l, --log', 'Enable detailed logging to file')
  .option('--log-dir <dir>', 'Directory for log files', './logs')
  .parse(process.argv);

const options = program.opts();

function generateRandomNumbers() {
  const count = Math.floor(Math.random() * 21) + 10; // 10-30個
  const numbers = [];
  for (let i = 0; i < count; i++) {
    const digits = Math.floor(Math.random() * 5) + 1; // 1-5桁
    const max = Math.pow(10, digits) - 1;
    const min = Math.pow(10, digits - 1);
    const number = Math.floor(Math.random() * (max - min + 1)) + min;
    numbers.push(number);
  }
  return numbers;
}

function validateResponse(inputNumbers, responseText) {
  // 期待される奇数の個数
  const expectedCount = inputNumbers.filter(n => n % 2 !== 0).length;

  let parsedResponse;
  try {
    // レスポンスから数値を抽出（数値のみ、またはJSON形式の数値）
    const numberMatch = responseText.match(/\d+/);
    if (!numberMatch) {
      return { valid: false, error: 'No number found in response' };
    }
    parsedResponse = parseInt(numberMatch[0]);
  } catch (error) {
    return { valid: false, error: 'Failed to parse response' };
  }

  // 結果の検証
  if (parsedResponse !== expectedCount) {
    return {
      valid: false,
      error: `Wrong count: expected ${expectedCount}, got ${parsedResponse}`
    };
  }

  return { valid: true };
}

async function runSingleTest(apiKey, endpoint, model, testNumber, logger) {
  const numbers = generateRandomNumbers();
  const prompt = `Count the odd numbers in the following list and return only the number. No explanation needed, just the number.\n\n${JSON.stringify(numbers)}`;

  const startTime = Date.now();

  try {
    const response = await axios.post(
      endpoint,
      {
        model: model,
        messages: [
          {
            role: 'system',
            content: 'You are an assistant that processes numbers accurately. Return only the number as the answer.'
          },
          {
            role: 'user',
            content: prompt
          }
        ],
        temperature: 0,
        max_tokens: 1000
      },
      {
        headers: {
          'Authorization': `Bearer ${apiKey}`,
          'Content-Type': 'application/json'
        },
        timeout: 30000
      }
    );

    const endTime = Date.now();
    const responseTime = endTime - startTime;

    const content = response.data.choices[0].message.content;
    const validation = validateResponse(numbers, content);

    const testResult = {
      success: true,
      responseTime,
      valid: validation.valid,
      error: validation.error,
      inputCount: numbers.length,
      oddCount: numbers.filter(n => n % 2 !== 0).length,
      input: numbers,
      expectedOutput: numbers.filter(n => n % 2 !== 0).length,
      actualOutput: content,
      prompt: prompt
    };

    if (logger) {
      logger.logTest(testNumber, testResult);
    }

    return testResult;

  } catch (error) {
    const errorResult = {
      success: false,
      error: error.response?.data?.error?.message || error.message,
      responseTime: Date.now() - startTime,
      input: numbers,
      expectedOutput: numbers.filter(n => n % 2 !== 0).length,
      prompt: prompt
    };

    if (logger) {
      logger.logTest(testNumber, errorResult);
    }

    return errorResult;
  }
}

class Logger {
  constructor(logDir, model, endpoint) {
    this.logDir = logDir;
    this.model = model;
    this.endpoint = endpoint;
    this.timestamp = new Date().toISOString().replace(/[:.]/g, '-');
    this.logFile = null;
    this.stream = null;
  }

  init() {
    if (!fs.existsSync(this.logDir)) {
      fs.mkdirSync(this.logDir, { recursive: true });
    }

    const filename = `benchmark_${this.model.replace(/\//g, '_')}_${this.timestamp}.log`;
    this.logFile = path.join(this.logDir, filename);
    this.stream = fs.createWriteStream(this.logFile, { flags: 'a' });

    this.stream.write('='.repeat(80) + '\n');
    this.stream.write('LLM API Speed Benchmark - Detailed Log\n');
    this.stream.write('='.repeat(80) + '\n');
    this.stream.write(`Timestamp: ${new Date().toISOString()}\n`);
    this.stream.write(`Model: ${this.model}\n`);
    this.stream.write(`Endpoint: ${this.endpoint}\n`);
    this.stream.write('='.repeat(80) + '\n\n');
  }

  logTest(testNumber, result) {
    this.stream.write(`\n--- Test #${testNumber} ---\n`);
    this.stream.write(`Success: ${result.success}\n`);
    this.stream.write(`Valid: ${result.valid || false}\n`);
    this.stream.write(`Response Time: ${result.responseTime}ms\n`);

    if (result.prompt) {
      this.stream.write(`\nPrompt:\n${result.prompt}\n`);
    }

    if (result.input) {
      this.stream.write(`\nInput Numbers (${result.input.length} items):\n${JSON.stringify(result.input)}\n`);
    }

    if (result.expectedOutput !== undefined) {
      this.stream.write(`\nExpected Output (count of odd numbers):\n${result.expectedOutput}\n`);
    }

    if (result.actualOutput) {
      this.stream.write(`\nActual Output:\n${result.actualOutput}\n`);
    }

    if (result.error) {
      this.stream.write(`\nError: ${result.error}\n`);
    }

    this.stream.write('-'.repeat(40) + '\n');
  }

  logSummary(summary) {
    this.stream.write('\n' + '='.repeat(80) + '\n');
    this.stream.write('SUMMARY\n');
    this.stream.write('='.repeat(80) + '\n');
    this.stream.write(JSON.stringify(summary, null, 2) + '\n');
  }

  close() {
    if (this.stream) {
      this.stream.end();
      console.log(`\nDetailed log saved to: ${this.logFile}`);
    }
  }
}

async function runBenchmark() {
  const apiKey = process.env[options.keyEnv];
  if (!apiKey) {
    console.error(`Error: API key not found in environment variable ${options.keyEnv}`);
    process.exit(1);
  }

  const iterations = parseInt(options.iterations);
  const model = options.model;
  const endpoint = options.endpoint;

  let logger = null;
  if (options.log) {
    logger = new Logger(options.logDir, model, endpoint);
    logger.init();
  }

  console.log('='.repeat(60));
  console.log('LLM API Speed Benchmark');
  console.log('='.repeat(60));
  console.log(`Model: ${model}`);
  console.log(`Endpoint: ${endpoint}`);
  console.log(`Iterations: ${iterations}`);
  console.log('='.repeat(60));

  const results = [];
  let successCount = 0;
  let validCount = 0;
  let totalResponseTime = 0;

  console.log('Running tests...\n');

  for (let i = 0; i < iterations; i++) {
    process.stdout.write(`\rProgress: ${i + 1}/${iterations}`);

    const result = await runSingleTest(apiKey, endpoint, model, i + 1, logger);
    results.push(result);

    if (result.success) {
      successCount++;
      totalResponseTime += result.responseTime;
      if (result.valid) {
        validCount++;
      }
    }

    // レート制限を避けるため少し待機
    await new Promise(resolve => setTimeout(resolve, 100));
  }

  console.log('\n');
  console.log('='.repeat(60));
  console.log('Results');
  console.log('='.repeat(60));

  const avgResponseTime = successCount > 0 ? totalResponseTime / successCount : 0;
  const successRate = (successCount / iterations * 100).toFixed(2);
  const accuracyRate = successCount > 0 ? (validCount / successCount * 100).toFixed(2) : '0.00';

  console.log(`Total Iterations: ${iterations}`);
  console.log(`Successful Requests: ${successCount}/${iterations} (${successRate}%)`);
  console.log(`Correct Responses: ${validCount}/${successCount} (${accuracyRate}%)`);
  console.log(`Average Response Time: ${avgResponseTime.toFixed(0)}ms`);

  if (successCount > 0) {
    const responseTimes = results.filter(r => r.success).map(r => r.responseTime);
    responseTimes.sort((a, b) => a - b);

    const median = responseTimes[Math.floor(responseTimes.length / 2)];
    const p95 = responseTimes[Math.floor(responseTimes.length * 0.95)];
    const min = Math.min(...responseTimes);
    const max = Math.max(...responseTimes);

    console.log(`Median Response Time: ${median}ms`);
    console.log(`95th Percentile: ${p95}ms`);
    console.log(`Min Response Time: ${min}ms`);
    console.log(`Max Response Time: ${max}ms`);
  }

  // エラーの集計
  const errors = results.filter(r => !r.success || !r.valid);
  if (errors.length > 0) {
    console.log('\n' + '='.repeat(60));
    console.log('Error Summary');
    console.log('='.repeat(60));

    const errorCounts = {};
    errors.forEach(e => {
      const msg = e.error || 'Unknown error';
      errorCounts[msg] = (errorCounts[msg] || 0) + 1;
    });

    Object.entries(errorCounts)
      .sort((a, b) => b[1] - a[1])
      .slice(0, 5)
      .forEach(([error, count]) => {
        console.log(`- ${error}: ${count} times`);
      });
  }

  console.log('='.repeat(60));

  if (logger) {
    const summary = {
      totalIterations: iterations,
      successfulRequests: successCount,
      correctResponses: validCount,
      successRate: successRate + '%',
      accuracyRate: accuracyRate + '%',
      averageResponseTime: avgResponseTime.toFixed(0) + 'ms',
      results: results
    };

    if (successCount > 0) {
      const responseTimes = results.filter(r => r.success).map(r => r.responseTime);
      responseTimes.sort((a, b) => a - b);
      const median = responseTimes[Math.floor(responseTimes.length / 2)];
      const p95 = responseTimes[Math.floor(responseTimes.length * 0.95)];
      const min = Math.min(...responseTimes);
      const max = Math.max(...responseTimes);

      summary.medianResponseTime = median + 'ms';
      summary.p95ResponseTime = p95 + 'ms';
      summary.minResponseTime = min + 'ms';
      summary.maxResponseTime = max + 'ms';
    }

    logger.logSummary(summary);
    logger.close();
  }
}

runBenchmark().catch(error => {
  console.error('Fatal error:', error);
  process.exit(1);
});