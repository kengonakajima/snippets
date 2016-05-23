
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


print("MOAIUntzSystem.initialize:")
MOAIUntzSystem.initialize ()

local sb = MOAIUntzSampleBuffer.new()
--sb:load('mono16.wav' )

local totalLenSec = 5
sb:prepareBuffer( 1, 44100 * totalLenSec , 44100 )


function dumpSampleBuffer(sb)
   local data = sb:getData()
   local meter={}
   for i,v in ipairs(data) do
      if (i%100)==0 then
         table.insert(meter,v*1000)
      end
   end
   for i,v in ipairs(meter) do
      local n = math.abs(v)
      local s = "" .. n
      for j=1,n do
         s = s .. "."
      end
      print(s)
   end
end

function genTone( sb, note, n )
   local bitPerSample, numChannel, numFrames, numSamplePerSec, lenSec = sb:getInfo()
   print("soundbuffer info: bp:",bitPerSample, "ch:", numChannel, "fr:", numFrames, "numSample:",numSamplePerSec, "len:",lenSec )
   local maxDataNum = numFrames * numChannel
   local dataNum = maxDataNum
   if n and n < dataNum then dataNum = n end
   local data = {}
   for i=1,dataNum do
      data[i] = math.sin( i / note )
   end
   return data
end

local cnt = 5
sb:setData( genTone( sb, cnt, nil ), 1 )

--dumpSampleBuffer(sb)


sound = MOAIUntzSound.new ()
sound:load (sb)
sound:setVolume ( 1 )
sound:setLooping ( true )
sound:play ()

local timestep = 0.2
local threstime = timestep
local curFillIndex = 1

t = MOAIThread.new()
t:run( function()
          cnt=5

          while true do
             local curpos = sound:getPosition()
             local curDataIndex = math.floor( 44100 * curpos )
             print("curpos:",curpos, curDataIndex, threstime )
             if curpos > threstime then
                threstime = threstime + timestep
                
                local unitFrameNum = timestep * 44100 * 1 * 2
                local dat = genTone(sb, cnt, unitFrameNum )
                cnt = cnt + 1
                print("datatoset:", #dat )
                sb:setData( dat,curDataIndex )


--                cnt = cnt + 1                
--                fillTone(sb,cnt, timestep * 44100 * 1)
--                sound:setPosition(0)
             end
             prop:moveRot ( 30, 1.5 )
             
             coroutine.yield()
          end          
       end)