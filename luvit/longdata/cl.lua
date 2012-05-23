local net = require('net')

local client
client = net.createConnection(61111, '127.0.0.1', function (err)
  if err then error(err) end
  print("Connected...")
  client:on("data",function(data)
      print("data:",#data)
      client:emit("data", "hoge\n")
      print("hog")      
    end)

end)

