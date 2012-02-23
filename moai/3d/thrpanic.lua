
MOAISim.openWindow ( "test", 320, 480 )

viewport = MOAIViewport.new ()
viewport:setSize ( 320, 480 )
viewport:setScale ( 320, 480 )

layer = MOAILayer.new ()
layer:setViewport ( viewport )
MOAISim.pushRenderPass ( layer )

gfxQuad = MOAIGfxQuad2D.new ()
gfxQuad:setTexture ( "../sol.png" ) --"cathead.png" )
gfxQuad:setRect ( -64, -64, 64, 64 )
gfxQuad:setUVRect ( 0, 1, 1, 0 )


prop = MOAIProp.new ()
prop:setDeck ( gfxQuad )
layer:insertProp ( prop )

prop:moveRot ( 360, 180, 0, 3 )
prop:moveLoc ( 0, 0, 128, 3 )


prop2 = MOAIProp.new ()
prop2:setDeck ( gfxQuad )
layer:insertProp ( prop2 )

prop2:moveRot ( 360, 180, 0, 8 )
prop2:moveLoc ( 40, 0, 128, 3 )



camera = MOAICamera3D.new ()
camera:setLoc ( 0, 0, camera:getFocalLength ( 320 ))
layer:setCamera ( camera )



function errh(erro)
  print("qqq")
  print( debug.traceback() )
  return false
end

xpcall( function()
    function onKeyboardEvent(k,dn)
      prrrrrrrrrrrrrrrrrrrint()
    end
    MOAIInputMgr.device.keyboard:setCallback( onKeyboardEvent )
  end, errh )
function a()
  prrrrrrrrrrrrrint()
end


th = MOAIThread.new()
th:run( function()
    local cnt=0
    while true do
      cnt = cnt + 1
      print("cnt:",cnt)
      if cnt > 1000000 then
        a()
      end
      coroutine.yield()
    end    
  end)