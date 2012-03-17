MOAISim.openWindow ( "seektest", 640,480)
local viewport = MOAIViewport.new()
viewport:setSize(640,480)
viewport:setScale(640,480)

local tex = MOAITexture.new ()
tex:load ("sol.png")
local deck = MOAITileDeck2D.new()
deck:setTexture(tex)
deck:setSize(1,1)
local w,h = tex:getSize()
deck:setRect(0,0,w,h)
local layer = MOAILayer2D.new()
layer:setViewport (viewport)
local camera = MOAITransform.new()
layer:setCamera(camera)
MOAISim.pushRenderPass (layer)

local centermark = MOAIProp2D.new()
centermark:setDeck(deck)
centermark:setIndex(1)
layer:insertProp(centermark)
centermark:setLoc(0,0)

local p = MOAIProp2D.new()    
p:setDeck(deck)
p:setIndex(1)
layer:insertProp(p)

-- start from x = -100 
p:setLoc(-100,0)

-- goes 100 px in 2 second, stop at x == 0 (center)
--p:moveLoc(100,0,2,MOAIEaseType.LINEAR)

-- goes 100+100=200 px in 2 seconds, stops at x == 100
--p:moveLoc(100,0,2,MOAIEaseType.LINEAR)
--p:moveLoc(100,0,2,MOAIEaseType.LINEAR) 

-- goes 100 px in 2 seconds, stop at x == 0,   because setLink overwrites the move. but it's too verbose..
local dr = p:moveLoc(100,0,2,MOAIEaseType.LINEAR)
dr:setLink(1, p, MOAITransform.ATTR_X_LOC,0, MOAIEaseType.LINEAR)
p:moveLoc(100,0,1,MOAIEaseType.LINEAR)

-- I wish I could write like this:
--p:moveLoc(100,0,2,MOAIEaseType.LINEAR)
--p:resetEaseDrivers()  -- this stops all ease drivers with the prop
--p:moveLoc(100,0,2,MOAIEaseType.LINEAR) -- then p goes to (0,0) correctly

-- or, more shorter:

--p:moveLoc(0,0,2,MOAIEaseType.LINEAR)
--p:moveLoc(0,0,2,MOAIEaseType.LINEAR, true) -- the last argument is "reset and overwrite" mode
