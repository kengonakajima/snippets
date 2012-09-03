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

-------------------------------

ndeck = 2
n = 1500
dur = 10

-- deck=300, n=1500 : 50fps @mbp
-- deck=100, n=1500 : 52fps @mbp
-- deck=1, n=1500 : 60fps @mbp
-------------------------------

decks = {}
for i=1,ndeck do
  local dk = loadTileDeck( "minecraft.png", 16,16, false, false )
  assert(dk)
  insert(decks,dk)
end




for i=1,n do
  local x,y = range(-SCRW,SCRW)/2, range(-SCRH,SCRH)/2
  p = Prop2D.new()
  p:setDeck( choose(decks) )
  if range(0,1) < 0.5 then
    local g = Grid.new()
    local gs = 8
    g:setSize(gs,gs,16,16,0,0,16,16)
    p:setGrid(g)
    ii=1
    for x=1,gs do
      for y=1,gs do
        g:setTile(x,y,ii%96)
        ii = ii + 1
      end
    end
    p:setScl(0.5,0.5)
  else
    p:setIndex(irange(1,96) )
    local r = range(1,32)
    p:setScl(r,r)
  end  
  p:setLoc(x,y)
  p:seekLoc(range(-SCRW,SCRW)/2,range(-SCRH,SCRH)/2,dur,EaseType.LINEAR )
  p:seekRot(dur/2*360,range(dur,dur*2), EaseType.LINEAR )

  layer:insertProp(p)
end

every(1,function()
        print( "fps:", Sim.getPerformance())
        end)


startTimerPoller()