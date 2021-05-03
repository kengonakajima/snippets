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

var server = mc.createServer({
  'online-mode': true,   // optional
  encryption: true,      // optional
  host: '0.0.0.0',       // optional
  port: 25565,           // optional
  version: '1.16.3'
});
const mcData = require('minecraft-data')(server.version)

server.on('login', function(endclient) {
  
  let loginPacket = mcData.loginPacket

  endclient.write('login', {
    entityId: endclient.id,
    isHardcore: false,
    gameMode: 0,
    previousGameMode: 255,
    worldNames: loginPacket.worldNames,
    dimensionCodec: loginPacket.dimensionCodec,
    dimension: loginPacket.dimension,
    worldName: 'minecraft:overworld',
    hashedSeed: [0, 0],
    maxPlayers: server.maxPlayers,
    viewDistance: 10,
    reducedDebugInfo: false,
    enableRespawnScreen: true,
    isDebug: false,
    isFlat: false
  });
    /*
  endclient.write('position', {
    x: 0,
    y: 1.62,
    z: 0,
    yaw: 0,
    pitch: 0,
    flags: 0x00
  });
  var msg = {
    translate: 'chat.type.announcement',
    "with": [
      'Server',
      'Hello, world!'
    ]
  };
  endclient.write("chat", { message: JSON.stringify(msg), position: 0, sender: '0' });
  */
});

client.on("data", function(packet) {
    console.log("PKT:",packet);
});
