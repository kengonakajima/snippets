
MOAISim.openWindow ( "test", 320, 480 )

viewport = MOAIViewport.new ()
viewport:setSize ( 320, 480 )
viewport:setScale ( 320, -480 )

layer = MOAILayer2D.new ()
layer:setViewport ( viewport )
MOAISim.pushRenderPass ( layer )

gfxQuad = MOAIGfxQuad2D.new ()
gfxQuad:setTexture ( "cathead.png" )
gfxQuad:setRect ( -64, -64, 64, 64 )
gfxQuad:setUVRect ( 0, 0, 1, 1 )

prop = MOAIProp2D.new ()
prop:setDeck ( gfxQuad )
layer:insertProp ( prop )



MOAIUntzSystem.initialize ()

--local sb = MOAIUntzSampleBuffer.new()
--sb:load('mono16.wav' )

--local sb2 = MOAIUntzSampleBuffer.new()
--sb2:load( "recorded/snd_254.wav" )

sound = MOAIUntzSound.new ()
sound:load ("recorded/snd_0.wav" )
sound:setVolume ( 1 )
sound:setLooping ( false )
sound:play ()

t = MOAIThread.new()
t:run( function()
          cnt=0
          local lastpos = sound:getPosition()
          while true do
             local curpos = sound:getPosition()
             print("aa:", sound:isPlaying(), curpos, sound:isPaused())
             
             if lastpos == curpos then
                cnt = cnt + 1
                print(cnt)
                sound = MOAIUntzSound.new()
                sound:load( "recorded/snd_" .. cnt .. ".wav" )
                sound:setVolume(1)
                sound:setLooping(false)
                sound:play()
             end

             lastpos = curpos
             
             prop:moveRot ( 360, 1.5 )
             
             coroutine.yield()
          end          
       end)