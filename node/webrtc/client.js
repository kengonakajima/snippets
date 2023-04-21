var tks=document.location.host.split(":");
var wsaddr = tks[0];
var ws_url= (tks[1]) ? `ws://${wsaddr}:${tks[1]}` : `ws://${wsaddr}`;

const ws = new WebSocket(ws_url);

ws.onopen = () => {
  console.log('Connected to the signaling server')
}

ws.onerror = err => {
  console.error(err)
}

ws.onmessage = msg => {
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

let connection = null
let name = null
let otherUsername = null

const sendMessage = message => {
  if (otherUsername) {
    message.otherUsername = otherUsername
  }

  ws.send(JSON.stringify(message))
}

document.querySelector('div#call').style.display = 'none'

document.querySelector('button#login').addEventListener('click', event => {
  username = document.querySelector('input#username').value

  if (username.length < 0) {
    alert('Please enter a username 🙂')
    return
  }

  sendMessage({
    type: 'login',
    username: username
  })
})

const handleLogin = async success => {
  if (success === false) {
    alert('😞 Username already taken')
  } else {
    document.querySelector('div#login').style.display = 'none'
    document.querySelector('div#call').style.display = 'block'

    /*    let localStream
          try {
          localStream = await navigator.mediaDevices.getUserMedia({
          video: true,
          audio: true
          })
          } catch (error) {
          alert(`${error.name}`)
          console.error(error)
          }

          document.querySelector('video#local').srcObject = localStream
    */
    
    const configuration = {
      iceServers: [{ url: 'stun:stun2.1.google.com:19302' }]
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

  if (callToUsername.length === 0) {
    alert('Enter a username 😉')
    return
  }

  otherUsername = callToUsername

  connection.createOffer(
    offer => {
      sendMessage({
        type: 'offer',
        offer: offer
      })

      connection.setLocalDescription(offer)
    },
    error => {
      alert('Error when creating an offer')
      console.error(error)
    }
  )
})

const handleOffer = (offer, username) => {
  otherUsername = username
  connection.setRemoteDescription(new RTCSessionDescription(offer))
  connection.createAnswer(
    answer => {
      connection.setLocalDescription(answer)
      sendMessage({
        type: 'answer',
        answer: answer
      })
    },
    error => {
      alert('Error when creating an answer')
      console.error(error)
    }
  )
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
  navigator.getUserMedia = navigator.getUserMedia || navigator.webkitGetUserMedia || navigator.mozGetUserMedia;
  navigator.getUserMedia({video: true, audio: true}, function(stream) {
    var video = document.getElementById('video_local');
    console.log("startCamera: video:",video,"stream:", stream);
    video.srcObject=stream;
    video.play();
    g_camera_stream=stream;
  }, function(error) {
    // カメラを起動できなかった場合の処理
    console.log(error);

  });
}
//ページがロードされたときにstartCamera関数を呼び出す



window.onload = function() {
    startCamera();
}
