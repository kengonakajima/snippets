function hoge()
{
    if (!navigator.mediaDevices || !navigator.mediaDevices.enumerateDevices) {
        console.log("enumerateDevices() not supported.");
        return;
    }

    // List cameras and microphones.

    navigator.mediaDevices.enumerateDevices()
        .then(function(devices) {
            devices.forEach(function(device) {
                console.log(device);
            });
        });

    navigator.mediaDevices.getUserMedia ( { audio: true } )
        .then(function(stream) {
            const audioCtx = new AudioContext();
            const analyser = audioCtx.createAnalyser();
            audioCtx.createMediaStreamSource(stream).connect(analyser);
            setInterval(function() {
                var dataArray = new Float32Array(analyser.fftSize);
                analyser.getFloatTimeDomainData(dataArray);
                const vol = Math.abs(dataArray[0]) * 50;
                var s="";
                for(var i=0;i<vol;i++) s+= "*";
                console.log("Volume:",vol,s);
            },100);
        });
    
}


hoge();