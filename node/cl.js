if( "WebSocket" in window ) {
    var ws = new WebSocket("ws://localhost:8080/","echo-protocol");
    console.log(ws);
    ws.onopen = function() {
        console.log("onopen");
        ws.send( "hello\0\0" );
    };
    ws.onmessage = function(evt) {
        console.log("onmessage:", evt.type, " data:", evt.data, " len:", evt.data.length  );
    };
    ws.onclose = function() {
        console.log("onclose");
    };
    
    

}

