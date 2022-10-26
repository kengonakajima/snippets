const sc = require("supercolliderjs");

sc.server.boot().then(async server => {
    // Compile and send to server from inline SuperCollider code
    const pulse = await server.synthDef(
        "pulse",
        `{ arg out = 0, freq=200;
           Out.ar(out, Pulse.ar(200, Line.kr(0.01,0.99,8), 0.2))
         }`,
    );

    await server.synth(pulse, { freq: 300 });


    console.log("PPP",server);
});