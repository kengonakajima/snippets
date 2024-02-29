// WebRTC to WebSockets converter

const WebSocket = require('ws');


function uuid4() {
  var temp_url = URL.createObjectURL(new Blob());
  var uuid = temp_url.toString();
  URL.revokeObjectURL(temp_url);
  return uuid.split(/[:/]/g).pop().toLowerCase(); // remove prefixes
}


const url=process.argv[2];
if(!url) {
  console.log("invalid arg. need ws url");
  process.exit(1);
}
const g_ws = new WebSocket(url, {
  perMessageDeflate: false
});

var g_pc;

g_ws.on('open', function open() {
  const o={};
  this.send(JSON.stringify(o));

  const connectionId=uuid4();
  const sendJson = JSON.stringify({ type: "connect", connectionId });
  console.log("onopen: sending:",sendJson);
  this.send(sendJson);

  //

  const configuration = {
    iceServers: [
      {
        url: 'stun:stun2.1.google.com:19302', // work in chrome
        urls: 'stun:stun2.1.google.com:19302', // work in safari
      },
      {
        urls: [
          'stun:stun2.1.google.com:19302' 
        ]
      }
    ]
  }
  console.log("configuration:",configuration);
  g_pc = new RTCPeerConnection(configuration)
});

g_ws.on('message', function incoming(data) {
  console.log("onmessage:",data);
  const msg = JSON.parse(data)

  switch (msg.type) {
    case 'offer':
      handleOffer(data.offer)
      break
    case 'answer':
      handleAnswer(data.answer)
      break
    case 'candidate':
      handleCandidate(data.candidate)
      break
    case 'close':
      handleClose()
      break
    default:
      break
  }
});

g_ws.on('close', function close() {
  console.log('onclose');
});

g_ws.on('error', function error(error) {
  console.error('WebSocket error:', error);
});


//////////////////
function handleOffer(offer) {  
  otherUsername = username
  g_pc.setRemoteDescription(new RTCSessionDescription(offer));
  g_pc.createAnswer()
    .then (answer => {
      g_pc.setLocalDescription(answer)
      g_ws.send(JSON.stringify({type: 'answer',  answer }));
    })
    .catch(error => {
      alert('Error when creating an answer')
      console.error(error)
    });
}
