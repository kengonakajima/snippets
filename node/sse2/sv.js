const http = require("http");

function sendServerSendEvent(req, res) {
  res.writeHead(200, {
    'Content-Type' : 'text/event-stream',
    'Cache-Control' : 'no-cache',
    'Connection' : 'keep-alive'
  });

  var sseId = (new Date()).toLocaleTimeString();

  setInterval(function() {
    writeServerSendEvent(res, sseId, (new Date()).toLocaleTimeString());
  }, 500);

  writeServerSendEvent(res, sseId, (new Date()).toLocaleTimeString());
}

function writeServerSendEvent(res, sseId, data) {
  res.write('id: ' + sseId + '\n');
  res.write("data: new server event " + data + '\n\n');
}

http.createServer(function(req, res) {
  console.log("req.",req);
  if (req.headers.accept && req.headers.accept == 'text/event-stream') {
    if (req.url == '/talk') {
      console.log("talk");
      sendServerSendEvent(req, res);
    } else {
      console.log("404");      
      res.writeHead(404);
      res.end();
    }
  } else {
    console.log("200");          
    res.writeHead(200, {
      'Content-Type' : 'text/html'
    });
    res.write(fs.readFileSync(__dirname + '/index.html'));
    res.end();
  }
}).listen(9999);
