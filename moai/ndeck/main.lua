require "lumino/lumino"

shortenMOAI()

SCRW, SCRH = 1280, 768

Sim.openWindow( "ndeck", SCRW, SCRH )

layer = Layer2D.new()

viewport = Viewport.new()
viewport:setSize(SCRW,SCRH)
viewport:setScale(SCRW,SCRH)

layer:setViewport(viewport)

Sim.pushRenderPass(layer)

deck = loadTileDeck( "minecraft.png", 16,16, false, false )
assert(deck)

n = 20000
dur = 30

for i=1,n do
  local x,y = range(-SCRW,SCRW), range(-SCRH,SCRH)
  p = Prop2D.new()
  p:setDeck(deck)
  p:setIndex(irange(1,96) )
  local r = range(1,32)
  p:setScl(r,r)
  p:setLoc(x,y)
  p:seekLoc(0,0,dur,EaseType.LINEAR )
  p:seekRot(dur/2*360,range(dur,dur*2), EaseType.LINEAR )

  layer:insertProp(p)
end

every(1,function()
        local f = Sim.getElapsedFrames()        
        if not prevFrame then prevFrame = f end
        local df = f - prevFrame
        prevFrame = f
        
        print( "fps:", df )
        end)


startTimerPoller()