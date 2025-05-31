const express = require('express');
const app = express();

let count = 0;

app.get('/count', (req, res) => {
  count++;
  console.log(`Count accessed: ${count}`);
  console.log('Headers:', req.headers);
  res.json({ count });
});

app.listen(3300, () => {
  console.log('Server running on port 3300');
});
