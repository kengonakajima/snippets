require "em-websocket"
require "pp"

EM::WebSocket.start({:host=>"0.0.0.0", :port => 9999}) do |conn|
  conn.onopen do 
    pp conn, "\n"
  end
  conn.onmessage do |message|
    pp message, "\n"
  end    
end
