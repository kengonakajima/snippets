local net = require('net')

local server
server = net.createServer(function (client)

    client:on("data", function(data)
        print("data:",#data)
        client:emit("data", "hello\n")
        print("hel")
      end)
    
    
  end):listen(61111)

print("start")
