----------------------------------------------------------------
-- Copyright (c) 2010-2011 Zipline Games, Inc. 
-- All Rights Reserved. 
-- http://getmoai.com
----------------------------------------------------------------

MOAISim.openWindow ( "test", 320, 480 )

viewport = MOAIViewport.new ()
viewport:setSize ( 320, 480 )
viewport:setScale ( 320, -480 )

layer = MOAILayer2D.new ()
layer:setViewport ( viewport )
MOAISim.pushRenderPass ( layer )

gfxQuad = MOAIGfxQuad2D.new ()
--gfxQuad:setTexture ( "images/cathead.png" )
gfxQuad:setTexture ( "sol.png" )
gfxQuad:setRect ( -64, -64, 64, 64 )
gfxQuad:setUVRect ( 0, 0, 1, 1 )

prop = MOAIProp2D.new ()
prop:setDeck ( gfxQuad )
layer:insertProp ( prop )

prop:moveRot ( 360, 8.5 )

cnt = 0
function upd()
  cnt = cnt + 1
  io.stdout:write(".")
  io.stdout:flush()
  if cnt==50 then
    nodef()
  end
end


function fn()
  while true do
    xpcall( upd, crashHandler )
    coroutine.yield()
  end  
end

t = MOAIThread.new()
t:run( fn )


function onkey()
  nodef()
end

function onKeyboardEvent()
  xpcall( nodef, crashHandler )
end

MOAIInputMgr.device.keyboard:setCallback ( onKeyboardEvent )
MOAIInputMgr.device.pointer:setCallback ( onPointerEvent )
MOAIInputMgr.device.mouseLeft:setCallback ( onMouseLeftEvent )
MOAIInputMgr.device.mouseRight:setCallback ( onMouseRightEvent )

function crashHandler(e)
  print(e)
  local s = debug.traceback(e)
  
  task = MOAIHttpTask.new ()
  task:setVerb ( MOAIHttpTask.HTTP_POST )
  task:setUrl ( "106.187.55.104:31111/crashlog" )
  task:setCallback ( onFinish )
  task:setBody( s )
  task:setUserAgent ( "Moai" )
-- task:setHeader ( "Baz", "baz" )
  task:setVerbose ( true )
  task:performAsync ()
  
end


function onFinish ( task, responseCode )
  print ( "onFinish", responseCode )
  if ( task:getSize ()) then
    print ( task:getString ())
  else
    print ( "nothing" )
  end
end

