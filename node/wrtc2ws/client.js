// WebRTC to WebSockets converter


const ws_url="ws://127.0.0.1";

const g_ws = new WebSocket(ws_url);

g_ws.onopen = () => {
  console.log('Connected to the signaling server')
}

g_ws.onerror = err => {
  console.error(err)
}

g_ws.onmessage = msg => {
  console.log('Got message on ws', msg.data)

  const data = JSON.parse(msg.data)

  switch (data.type) {
    case 'login':
      handleLogin(data.success)
      break
    case 'offer':
      handleOffer(data.offer, data.username)
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
}

let g_pc = null



const handleLogin = async success => {
  if (success === false) {
    alert('😞 Username already taken')
  } else {
    document.querySelector('div#login').style.display = 'none'
    document.querySelector('div#call').style.display = 'block'
    
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

    connection = new RTCPeerConnection(configuration)


    var dc=connection.createDataChannel("mylabel", {ordered:false, maxRetransmitTime: 3000 });
    console.log("createDataChannel:",dc);
    dc.onopen=function() {
      console.log("dc.onopen", dc.readyState);
      var cnt=0;
      setInterval( function() {
        console.log("sending",dc);
        cnt++;
        dc.send("hello world "+cnt);              
      },1000);
    }
    dc.onerror = function(e) { console.log("dc error:",e); }
    dc.onclose = function() {
      console.log("onclose");
    }
    dc.onmessage=function(event) {
      console.log("dc message:",event.data);
      document.querySelector('div#message').innerHTML=event.data;
    }
    //    connection.addStream(localStream)

    //    connection.onaddstream = event => {
    //      document.querySelector('video#remote').srcObject = event.stream
    //    }

    connection.ondatachannel = function(ev) {
      console.log("connection.ondatachannel:",ev);
      var cnt=0;
      setInterval( function() {
        console.log("sending",ev.channel);
        cnt++;
        ev.channel.send("hello world "+cnt);              
      },1000);
    }
    connection.onicecandidate = event => {
      console.log("connection.onicecandidate:",event.candidate);
      if (event.candidate) {
        sendMessage({
          type: 'candidate',
          candidate: event.candidate
        })
      }
    }

    connection = new RTCPeerConnection(configuration)

    if(g_camera_stream) {
      console.log("handleLogin: camera_stream active.");
      g_camera_stream.getTracks().forEach(track => connection.addTrack(track, g_camera_stream));
    }


    var dc=connection.createDataChannel("mylabel", {ordered:false, maxRetransmitTime: 3000 });
    console.log("dc:",dc);
    dc.onopen=function() {
      console.log("onopen", dc.readyState);
      var cnt=0;
      setInterval( function() {
        console.log("sending",dc);
        cnt++;
        dc.send("hello world "+cnt);              
      },1000);
    }
    dc.onerror = function(e) { console.log("dc error:",e); }
    dc.onclose = function() {
      console.log("onclose");
    }
    dc.onmessage=function(event) {
      console.log("onmessage:",event.data);
      document.querySelector('div#message').innerHTML=event.data;
    }
    //    connection.addStream(localStream)

    //    connection.onaddstream = event => {
    //      document.querySelector('video#remote').srcObject = event.stream
    //    }

    connection.ondatachannel = function(ev) {
      console.log("ondatachannel!:",ev);
      setInterval( function() {
        console.log("s2ending",ev.channel);
        ev.channel.send("hello world 2");              
      },1000);
      
      
    }
    connection.onicecandidate = event => {
      console.log("onicecandidate!",event);      
      if (event.candidate) {
        sendMessage({
          type: 'candidate',
          candidate: event.candidate
        })
      }
    };
    connection.ontrack = event => {
      console.log("ontrack!",event);
      const video_remote = document.getElementById('video_remote');
      const audio_remote = document.getElementById('audio_remote');
      
      const stream = event.streams[0];
      if (stream.getVideoTracks().length) {
        video_remote.srcObject = stream;
      } else if (stream.getAudioTracks().length) {
        audio_remote.srcObject = stream;
      }      
    };
  }  
}

document.querySelector('button#call').addEventListener('click', () => {
  const callToUsername = document.querySelector('input#username-to-call').value
  console.log("call clicked. username:",callToUsername);

  if (callToUsername.length === 0) {
    alert('Enter a username 😉')
    return
  }

  otherUsername = callToUsername
  console.log("otherUsername:",callToUsername);
  connection.createOffer()
    .then(offer => {
      console.log("connection.createOffer offer:",offer);
      sendMessage({
        type: 'offer',
        offer: offer
      })
      connection.setLocalDescription(offer)
    })
    .catch(error => {
      alert('Error when creating an offer')
      console.error(error)
    });                
});

const handleOffer = (offer, username) => {
  otherUsername = username
  connection.setRemoteDescription(new RTCSessionDescription(offer));
  connection.createAnswer()
    .then (answer => {
      connection.setLocalDescription(answer)
      sendMessage({
        type: 'answer',
        answer: answer
      })
    })
    .catch(error => {
      alert('Error when creating an answer')
      console.error(error)
    });
}

const handleAnswer = answer => {
  connection.setRemoteDescription(new RTCSessionDescription(answer))
}

const handleCandidate = candidate => {
  connection.addIceCandidate(new RTCIceCandidate(candidate))
}

document.querySelector('button#close-call').addEventListener('click', () => {
  sendMessage({
    type: 'close'
  })
  handleClose()
})

const handleClose = () => {
  otherUsername = null
  document.querySelector('video#remote').src = null
  connection.close()
  connection.onicecandidate = null
  connection.onaddstream = null
}


////////////////////////

var g_camera_stream;

// カメラを起動する関数
// この関数は、カメラを起動するためのボタンを押したときに呼び出される
function startCamera() {
  // カメラを起動する
//  const getUserMedia = navigator.getUserMedia || navigator.webkitGetUserMedia || navigator.mozGetUserMedia || navigator.mediaDevices.getUserMedia;
    navigator.mediaDevices.getUserMedia({video: true, audio: true})
        .then (function(stream) {
            var video = document.getElementById('video_local');
            console.log("startCamera: video:",video,"stream:", stream);
            video.srcObject=stream;
            video.play();
            g_camera_stream=stream;
        })
        .catch(function(error) {
            // カメラを起動できなかった場合の処理
            console.log(error);
        });
}
//ページがロードされたときにstartCamera関数を呼び出す



window.onload = function() {
//  startCamera();
}

document.querySelector('button#start_camera').addEventListener('click', () => {
    startCamera();
});
