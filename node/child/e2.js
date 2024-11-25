const { execSync } = require('child_process');


try {
  const result = execSync('python hoge.py');
  console.log('Success:', result);
  console.log('result.toString():', result.toString());  
} catch (error) {
  console.error('Error occurred:',error);
  if (error.stderr) {
    console.error('stderr:', error.stderr.toString());
  }
  if (error.stdout) {
    console.error('stdout:', error.stdout.toString());
  }
  console.error('Status code:', error.status);
  console.error('Signal:', error.signal);
}
