local net = require('net')
local server

server = net.createServer( function(client)
                            client:pipe(client)
                            client:pipe(process.stdout)
                           end):listen(8989)

print("TCP echo server listening on port 8989")

