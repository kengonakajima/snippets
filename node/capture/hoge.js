var captureWindow = require("capture-window");



function doit() {
    var start_time = new Date();
    
    captureWindow('Finder', 'ダウンロード', function (err, filePath) {
        if (err) { throw err; }

        var end_time = new Date();
        console.log("done:", filePath, end_time - start_time );
        // filePath is the path to a png file
    });
}


setInterval( function() {
    console.log("interval:", new Date());
    doit();
}, 100 );
