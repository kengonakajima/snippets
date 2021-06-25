

const Tail = require('tail-file');
const mytail = new Tail("myfile.log", line => {
  console.log( line );
});
