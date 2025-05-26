const express = require('express');
const app = express();

let count = 0;

app.get('/count', (req, res) => {
  res.json({ count: ++count });
});

app.listen(3300, () => {
  console.log('Server running on port 3300');
});
