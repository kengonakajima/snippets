const express=require("express");
const body_parser=require("body-parser");
const helmet=require("helmet");
const url=require("url");
const add_csp=process.argv[2] || "ws://localhost:3000/";
console.log("add_csp:",add_csp);

var app=express();
app.use(helmet());
app.use(helmet.contentSecurityPolicy({
   directives: {
       defaultSrc: ["'self'"],
       connectSrc: ["'self'", add_csp ],
   },
  connectSrc: [ "'self'", add_csp ]
}));
app.use(body_parser.urlencoded({extended: true}));
app.use("/client.js", express.static("client.js"));
app.use("/index.html", express.static("index.html"));
app.get("/", function(req,res,next) {
    res.sendFile( process.env.PWD + "/index.html") ;
});
var web_server = app.listen(3000,"0.0.0.0", function() {
    console.log("web server is listening") ;
});


////////////

const WebSocket = require('ws')
const wsv = new WebSocket.Server({server:web_server});
const users = {}

const sendTo = (ws, message) => {
  ws.send(JSON.stringify(message))
}

wsv.on('connection', ws => {
  console.log('User connected')

  ws.on('message', message => {
    let data = null

    try {
      data = JSON.parse(message)
    } catch (error) {
      console.error('Invalid JSON', error)
      data = {}
    }

    switch (data.type) {
      case 'login':
        console.log('User logged', data.username)
        if (users[data.username]) {
          sendTo(ws, { type: 'login', success: false })
        } else {
          users[data.username] = ws
          ws.username = data.username
          sendTo(ws, { type: 'login', success: true })
        }
        break
      case 'offer':
        console.log('Sending offer to: ', data.otherUsername,"offer:",data.offer)
        if (users[data.otherUsername] != null) {
          ws.otherUsername = data.otherUsername
          sendTo(users[data.otherUsername], {
            type: 'offer',
            offer: data.offer,
            username: ws.username
          })
        }
        break
      case 'answer':
        console.log('Sending answer to: ', data.otherUsername,"answer:",data.answer)
        if (users[data.otherUsername] != null) {
          ws.otherUsername = data.otherUsername
          sendTo(users[data.otherUsername], {
            type: 'answer',
            answer: data.answer
          })
        }
        break
      case 'candidate':
        console.log('Sending candidate to:', data.otherUsername,"candidate:",data.candidate)
        if (users[data.otherUsername] != null) {
          sendTo(users[data.otherUsername], {
            type: 'candidate',
            candidate: data.candidate
          })
        }
        break
      case 'close':
        console.log('Disconnecting from', data.otherUsername)
        users[data.otherUsername].otherUsername = null

        if (users[data.otherUsername] != null) {
          sendTo(users[data.otherUsername], { type: 'close' })
        }

        break

      default:
        sendTo(ws, {
          type: 'error',
          message: 'Command not found: ' + data.type
        })

        break
    }
  })

  ws.on('close', () => {
    if (ws.username) {
      delete users[ws.username]

      if (ws.otherUsername) {
        console.log('Disconnecting from ', ws.otherUsername)
        users[ws.otherUsername].otherUsername = null

        if (users[ws.otherUsername] != null) {
          sendTo(users[ws.otherUsername], { type: 'close' })
        }
      }
    }
  })
})

/////////////////
