var http = require("http")
var fs = require("fs")

const port = 1027


http.createServer(function (req, res) {
  const index = "./sse.html"
  var fileName
  var interval
  console.log ("req.url = " + req.url)

  if (req.url === "/")
    fileName = index
  else
    fileName = "." + req.url

  if (fileName === "./stream") {
    res.writeHead(200, {"Content-Type":"text/event-stream", "Connection":"keep-alive"})

    res.write("retry: 10000\n")
    res.write("event: connecttime\n")
    var cnt=1
    res.write("data: " + (new Date()) + " "+cnt+ "\n\n")
//    res.write("data: " + (new Date()) + "\n\n")

    interval = setInterval(function() {
      res.write("data: " + (new Date()) +" "+cnt+ "\n\n")
      cnt++;
    }, 500)
    req.connection.addListener("close", function () {
      clearInterval(interval)
    }, false)
  } else if (fileName === index) {
    fs.exists(fileName, function(exists) {
      if (exists) {
        fs.readFile(fileName, function(error, content) {
          if (error) {
            res.writeHead(500)
            res.end()
          } else {
            res.writeHead(200, {"Content-Type":"text/html"})
            res.end(content, "utf-8")
          }
        })
      } else {
        res.writeHead(404)
        res.end()
      }
    })
  } else {
    res.writeHead(404)
    res.end()
  }

}).listen (port, "0.0.0.0")

console.log("Server running at http://127.0.0.1:" + port + "/")
