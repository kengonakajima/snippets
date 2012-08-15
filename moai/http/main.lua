----------------------------------------------------------------
-- Copyright (c) 2010-2011 Zipline Games, Inc. 
-- All Rights Reserved. 
-- http://getmoai.com
----------------------------------------------------------------

require("lumino")




---------

MOAISim.openWindow ( "test", 320, 480 )

viewport = MOAIViewport.new ()
viewport:setSize ( 320, 480 )
viewport:setScale ( 320, -480 )

layer = MOAILayer2D.new ()
layer:setViewport ( viewport )
MOAISim.pushRenderPass ( layer )

gfxQuad = MOAIGfxQuad2D.new ()
gfxQuad:setTexture ( "sol.png" )
gfxQuad:setRect ( -64, -64, 64, 64 )
gfxQuad:setUVRect ( 0, 0, 1, 1 )

prop = MOAIProp2D.new ()
prop:setDeck ( gfxQuad )
layer:insertProp ( prop )

prop:moveRot ( 360, 8.5 )



-----

function onFinish ( task, responseCode )
  local s = task:getString()
  print ( "finished. code:", responseCode, "size:", task:getSize(), "slen:", #s , "ind:", task.jpgIndex )
  assert(writeFile("hoge"..task.jpgIndex..".jpg", s )  )
end


local nget = 50

function getSync(ind)
  task = MOAIHttpTask.new ()
  task:httpGet( "106.187.55.104:8981/8", nil, false, true )
  writeFile( "hoge"..ind..".jpg", task:getString())
end

for i=1,nget do
  print("start getSync:",i)
  getSync(i)
end

function getAsync(ind)
  task = MOAIHttpTask.new()
  task:httpGet( "106.187.55.104:8981/8", nil, false, false )
  task.jpgIndex = ind
  task:setCallback( onFinish)  
end

for i=1,nget do
  print("start getAsync:",i)
  getAsync(i)
end
