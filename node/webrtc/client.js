const ws = new WebSocket('ws://localhost:8080')

ws.onopen = () => {
  console.log('Connected to the signaling server')
}

ws.onerror = err => {
  console.error(err)
}

ws.onmessage = msg => {
  console.log('Got message', msg.data)

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

    let localStream
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

    const configuration = {
      iceServers: [{ url: 'stun:stun2.1.google.com:19302' }]
    }

    connection = new RTCPeerConnection(configuration)

    connection.addStream(localStream)

    connection.onaddstream = event => {
      document.querySelector('video#remote').srcObject = event.stream
    }

    connection.onicecandidate = event => {
      if (event.candidate) {
        sendMessage({
          type: 'candidate',
          candidate: event.candidate
        })
      }
    }
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