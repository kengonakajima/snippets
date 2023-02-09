const http = require("http");
const fs=require("fs");

function sendServerSendEvent(req, res) {
  res.writeHead(200, {
    'Content-Type' : 'text/event-stream',
    'Cache-Control' : 'no-cache',
    'Connection' : 'keep-alive'
  });

  const sseId = (new Date()).toLocaleTimeString();

  setInterval(function() {
    writeServerSendEvent(res, sseId, (new Date()).toLocaleTimeString());
  }, 500);

  writeServerSendEvent(res, sseId, (new Date()).toLocaleTimeString());
}

function writeServerSendEvent(res, sseId, data) {
  res.write('id: ' + sseId + '\n');
  res.write("data: new server event " + data + '\n\n');
}
function writeSSE(res,data) {
  const sseId = (new Date()).toLocaleTimeString();
  res.write('id: ' + sseId + '\n');
  res.write("data: new server event " + data + '\n\n');  
}


http.createServer(function(req, res) {
  req.setEncoding("utf-8");
  console.log("req.",req.method,req.url);
  if (req.headers.accept && req.headers.accept == 'text/event-stream') {
    if (req.url == '/talk') {
      console.log("talk");
      sendServerSendEvent(req, res);
    } else {
      console.log("404");      
      res.writeHead(404);
      res.end();
    }
  } else if(req.method=="GET"){
    console.log("200");          
    res.writeHead(200, {
      'Content-Type' : 'text/html'
    });
    res.write(fs.readFileSync(__dirname + '/index.html'));
    res.end();
  } else if(req.method=="POST"){
    console.log("post!");
    sendServerSendEvent(req,res);
    req.on("data", chunk=>{
      console.log("data:",chunk.toString());
      writeSSE(res,chunk.toString());
    });    
  }
}).listen(9999);
