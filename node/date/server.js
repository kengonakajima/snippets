const express = require('express');
const app = express();
const port = 3000;

app.get('/', (req, res) => {
  const currentTime = new Date().toLocaleString('ja-JP', {
    timeZone: 'Asia/Tokyo',
    year: 'numeric',
    month: '2-digit',
    day: '2-digit',
    hour: '2-digit',
    minute: '2-digit',
    second: '2-digit'
  });
  res.send(currentTime);
});

app.listen(port, () => {
  console.log(`Server running at http://localhost:${port}`);
});