console.log("arg:",process.argv);

var mc = require('minecraft-protocol');
var client = mc.createClient({
    host: process.argv[2],
    port: 25565,         
    username: process.argv[3],
  password: process.argv[4],
  auth: 'mojang' // optional; by default uses mojang, if using a microsoft account, set to 'microsoft'
});
client.on('chat', function(packet) {
  // Listen for chat messages and echo them back.
    var jsonMsg = JSON.parse(packet.message);
    var msg=jsonMsg.extra[0].text;
    console.log("received chat:",msg);    
    if(msg.match("<neko")) {
        client.write('chat', {message: "world"});
    }
    /*
  if(jsonMsg.translate == 'chat.type.announcement' || jsonMsg.translate == 'chat.type.text') {
    var username = jsonMsg.with[0].text;
    var msg = jsonMsg.with[1];
    if(username === client.username) return;
    client.write('chat', {message: msg.text});
  }
  */
});
