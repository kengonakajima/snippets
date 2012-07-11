#!/usr/bin/env luvit

require("./lumino/lumino")

local Timer = require('timer')

--local handle_websocket = require('websocket').handler
local ws = require('./luvit-websocket')
assert(ws)
pp(ws)
local handle_websocket = ws.handler
assert(handle_websocket)


require('http').createServer(function (req, res)
  if req.url:sub(1, 3) == '/ws' then
    handle_websocket(req, res, function ()
      Timer.setInterval(1000, function ()
        res:send('.')
      end)
      -- simple repeater
      req:on('message', function (message)
        p('<', message)
        res:send(message, function ()
          p('>', message)
        end)
      end)
    end)
  else
    res:finish()
  end
end):listen(8080, '0.0.0.0')
print('Open a browser, and try to create a WebSocket for ws://localhost:8080/ws')
