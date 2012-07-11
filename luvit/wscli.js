if( "WebSocket" in window ) {
    var cnt=0;
    var ws = new WebSocket("ws://localhost:8080/ws","echo-protocol");
    console.log(ws);
    ws.onopen = function() {
        console.log("onopen");
        ws.send( "hello\0\0" );
    };
    ws.onmessage = function(evt) {
        console.log("onmessage:", evt.type, " data:", evt.data, " len:", evt.data.length  );
        cnt++;
        if(cnt<10){
            ws.send("hello"+cnt);
        } else {
            ws.close();
        }
    };
    ws.onclose = function() {
        console.log("onclose");
    };
    
    

}

