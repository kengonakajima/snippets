const recorder = require('node-record-lpcm16');
recorder
    .record({
        sampleRate: 48000,
        threshold: 0.5, //silence threshold
        recordProgram: 'rec', // Try also "arecord" or "sox"
        silence: '0.2', //seconds of silence before ending
    })
    .stream()
    .on('error', console.error)
    .on('data', function(data) {
        var ntimes=Math.abs(data.readInt16LE()) / 512;
        var bar="*".repeat(ntimes);
        console.log("mic data length:",data.length, " vol:", bar);
    });
console.log('Listening, press Ctrl+C to stop.');

