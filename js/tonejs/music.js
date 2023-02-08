



document.getElementById("play-button").addEventListener("click", function() {
  const synth = new Tone.Synth().toMaster();
  synth.triggerAttackRelease('C4', '8n');

  console.log("state:",Tone.Transport.state);
  if (Tone.Transport.state !== 'started') {
    Tone.Transport.start();
  } else {
    Tone.Transport.stop();
  }
  setInterval(function() {

  },100);


});

