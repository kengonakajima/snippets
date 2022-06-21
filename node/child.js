const { exec } = require('child_process')

exec('sleep 2; echo hoge', (err, stdout, stderr) => {
    if (err) {
      console.log(`stderr: ${stderr}`)
      return
    }
    console.log(`stdout: ${stdout}`)
  }
)
console.log("poo");
