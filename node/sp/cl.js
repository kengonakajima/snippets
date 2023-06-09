// cl.js
const wrtc = require('wrtc');
const WebSocket = require('ws');

const wrap = fn => {
  return (req, res, next) => {
    return fn(req, res, next).catch(next);
  }
};


async function startPeer(isOfferer) {
  const peer = new wrtc.RTCPeerConnection();
  const ws = new WebSocket('ws://localhost:8080');

  ws.on('message', async message => {
    const { type, data } = JSON.parse(message);

    if (type === 'offer') {
      await peer.setRemoteDescription(new wrtc.RTCSessionDescription(data));
      const answer = await peer.createAnswer();
      await peer.setLocalDescription(answer);
      ws.send(JSON.stringify({ type: 'answer', data: answer }));
    } else if (type === 'answer') {
      await peer.setRemoteDescription(new wrtc.RTCSessionDescription(data));
    } else if (type === 'candidate') {
      await peer.addIceCandidate(new wrtc.RTCIceCandidate(data));
    }
  });

  peer.onicecandidate = ({ candidate }) => {
    if (candidate) {
      ws.send(JSON.stringify({ type: 'candidate', data: candidate }));
    }
  };

  const channel = peer.createDataChannel("channel");
  channel.onopen = () => {
    wrap(async function() {
      const ret=await channel.send("hello");      
      console.log('Channel opened!',ret);
      
    })();
  }
  channel.onmessage = (e) => console.log(`Received message: ${e.data}`);

  ws.on('open', (x)=>{
    console.log("on ws open");

    if (isOfferer) {
      wrap(async function() {
        const offer = await peer.createOffer();
        await peer.setLocalDescription(offer);
        ws.send(JSON.stringify({ type: 'offer', data: offer }));        
      })();
    } else {
       peer.ondatachannel = (event) => {
         const channel = event.channel;
         channel.onmessage = (e) => {
           console.log(`Received message: ${e.data}`);
           channel.send("echo hello!");
         }
       };
    }
  });
  
}

startPeer(process.argv[2] === 'offer');
