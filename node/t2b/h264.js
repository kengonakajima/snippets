const fs = require('fs');

const text=fs.readFileSync(process.argv[2]);
console.log("text:",text);

const spl=text.toString().split(" ");
const ary=spl.map( (x)=> parseInt(x));
console.log(ary);

const buffer = Buffer.from(ary);

fs.writeFile('output.bin', buffer, (err) => {
    if (err) throw err;
    console.log('The file has been saved!');
});

