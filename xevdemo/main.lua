


-- win
W=300
H=200
MOAISim.openWindow("xev",W,H)

function scrToWorld(x,y)
   return x - W/2, H/2 - y
end
function now()
   return MOAISim.getElapsedTime()
end
function rand()
   return math.random()
end
function normalize(x1,y1,x2,y2,mul)
   local len = math.sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1))
   return (x2-x1)/len*mul, (y2-y1)/len*mul
end


score = 0

-- input
ptrx,ptry=0,0
if MOAIInputMgr.device.pointer then
   MOAIInputMgr.device.pointer:setCallback ( function(x,y)
--                                                print("ptr. coord:",x,y)
                                                ptrx,ptry = x,y
                                             end)
   MOAIInputMgr.device.mouseLeft:setCallback ( function(flag)
--                                                  print("left. flag:",flag)
                                                  sol:setGoal( scrToWorld(ptrx,ptry) )
                                               end)
end
if MOAIInputMgr.device.touch then
   MOAIInputMgr.device.touch:setCallback ( function ( eventType, idx, x, y, tapCount )
--                                              print( "touch dn:", eventType, "idx:", idx, "xy:", x,y, "cnt:",tapCount )
                                              sol:setGoal( scrToWorld(x,y))
                                           end)
end

         
-- tex, deck
t = MOAITexture.new()
t:load("xeviousbase.png")
deck = MOAITileDeck2D.new()
deck:setTexture(t)
deck:setSize(8,8,16/128,16/128,0,0,16/128,16/128)
deck:setRect(-0.5,-0.5,0.5,0.5)

-- viewport, layer, camera
vp = MOAIViewport.new()
vp:setSize(W,H)
vp:setScale(W,H)

layer = MOAILayer2D.new()
bglayer = MOAILayer2D.new()
MOAISim.pushRenderPass(bglayer)
MOAISim.pushRenderPass(layer)


camera = MOAITransform.new()
layer:setCamera(camera)
bglayer:setCamera(camera)
layer:setViewport(vp)
bglayer:setViewport(vp)


-- prop
props={}

function Char(x,y,ind)
   local p = MOAIProp2D.new()
   p:setDeck(deck)
   p:setIndex(ind)
   p.x, p.y = x,y
   p:setLoc(x,y)
   p.sz = 32
   p:setScl(p.sz,p.sz)
   layer:insertProp(p)

   p.goalx, p.goaly = nil,nil
   p.setGoal = function(self,x,y)
                  self.goalx, self.goaly = x,y
               end

   p.cb = nil
   p.vx, p.vy = 0,0
   p.update = function(self,dt)
                if self.cb then
                   self:cb(dt)
                end
                self.x, self.y = self:getLoc()
                local x,y = self.x, self.y
                local nx,ny = x + self.vx*dt, y + self.vy*dt

                if self.goalx and ((self.vx > 0 and nx >= self.goalx ) or (self.vx < 0 and nx <= self.goalx)) then self.goalx, self.vx = nil,0 end
                if self.goaly and ((self.vy > 0 and ny >= self.goaly ) or (self.vy < 0 and ny <= self.goaly)) then self.goaly, self.vy = nil,0 end
                   
                self:setLoc(nx,ny)
                if nx < -W/2 or nx > W/2 or ny < -H/2 or ny > H/2 then
                   self.toRemove = true
                end
             end
   p.intersect = function(self,v)
                    return ( (v.x - v.sz/2) <= self.x + self.sz/2  ) and ((v.x + v.sz/2) >= self.x - self.sz/2 ) and ( (v.y - v.sz/2) <= self.y+self.sz/2) and ( (v.y + v.sz/2) >= (self.y - self.sz/2) ) 
                 end
   
   p.getHits = function(self)
                  local out={}
                  for i,v in ipairs(props) do
                     if self:intersect(v) and v ~= self then
                        table.insert( out, v )
                     end
                  end
                  return out
               end
   
   table.insert(props,p)
   return p
end

zapperCurve = MOAIAnimCurve.new()
zapperCurve:reserveKeys(2)
zapperCurve:setKey( 1, 0, 10 )
zapperCurve:setKey( 2, 0.1, 11)

function Zapper(x,y)
   local p = Char(x,y,10)

   local anim = MOAIAnim.new()
   anim:reserveLinks(1)
   anim:setLink( 1, zapperCurve, p, MOAIProp2D.ATTR_INDEX )
   anim:setMode( MOAITimer.LOOP )
   anim:start()
   
   p.vx, p.vy = 0, 700
   p.cb = function(self,dt)
             local hits = self:getHits()
             for i,v in ipairs(hits) do
                if v.isEnemy and v.isDestroyable then
                   self.toRemove = true
                   score = score + 50
                   updateScore( score )
                   hitse:play()
                   v.toRemove = true
                   break
                end
             end
          end
   return p
end

function Enemy(x,y,ind)
   local p = Char(x,y,ind)
   p.isEnemy = true
   return p
end

function Tarken(x,y)
   local p = Enemy(x,y,2)
   p.vx = 0
   p.vy = -200 - 200 * rand()
   if x > 0 then
      p.vx = - 200 * rand()
   elseif x < 0 then
      p.vx = 200 * rand()
   end
   
   p.turnY = - 200  + 400 * rand()
   p.isDestroyable = true
   p.cb = function(self,dt)
             if self.vy < 0 then
                if self.y < 0 then
                   Spario(self.x,self.y-16,sol.x,sol.y)
                   self.vy, self.vx = 0,0
                   self.departAt = now() + 0.5
                end                
             end
             if self.departAt and now() > self.departAt then
                self.vy = 200 + 200 * rand()
             end
          end   
   return p
end

sparioCurve = MOAIAnimCurve.new()
sparioCurve:reserveKeys(5)
sparioCurve:setKey( 1, 0, 12)
sparioCurve:setKey( 2, 0.05, 13)
sparioCurve:setKey( 3, 0.10, 14)
sparioCurve:setKey( 4, 0.15, 15)
sparioCurve:setKey( 5, 0.20, 16)

function Spario(fromx,fromy,tox,toy)
   local p = Enemy(fromx,fromy,9)
   local anim = MOAIAnim.new()
   anim:reserveLinks(1)
   anim:setLink( 1, sparioCurve, p, MOAIProp2D.ATTR_INDEX )
   anim:setMode( MOAITimer.LOOP )
   anim:start()

   p.vx, p.vy = normalize(fromx, fromy, tox, toy, 200 )
   p.cb = function(self,dt)
             if sol and self:intersect(sol) then
                sol:setLoc(0,0)
                self.toRemove = true
             end
          end   
   return p
end


function Zosie(x,y)
   local p = Enemy(x,y,33)
   p.cnt = 0
   p.cb = function(self,dt)
             p.cnt = p.cnt + 1
             if (p.cnt % 30 ) == 0 then
                p.vx, p.vy = -200 + 400 * rand(), -200 + 400 * rand()
             end
             if p.y < 10 and p.vy < 0 then
                p.vy = p.vy * -1
             end
          end
   return p
end

-- myship
function Solvalou(x,y)
   local p = Char(x,y,1)
   p.isMyShip = true
   p.cb = function(self,dt)
             self.dx, self.dy = 0,0

             local x,y = self.x, self.y
             local xsign, ysign

             local speed = 200
             if self.goalx then
                if x>self.goalx then self.vx = -speed end
                if x<self.goalx then self.vx = speed end
             end
             if self.goaly then
                if y>self.goaly then self.vy = -speed end
                if y<self.goaly then self.vy = speed end
             end

             local t = now()
             if not self.lastShoot then
                self.lastShoot = t
             end
             if t > self.lastShoot + 0.5 then
                Zapper(self.x,self.y)
                self.lastShoot = t
             end
          end
   return p
end

sol = Solvalou(0,0)


-- bg
bggrid = MOAIGrid.new()
bggrid:setSize( 16,16, 32,32 )
bg = MOAIProp2D.new()
bg:setDeck(deck)
bg:setGrid(bggrid)
if bg.setRepeat then
   bg:setRepeat(true)
else
   bggrid:setRepeat(true)   
end

bglayer:insertProp(bg)

for i=1,16 do
   for j=1,16 do
      local ind = 25
      if rand()<0.1 then ind = 26 end
      bggrid:setTile(i,j,ind)
   end
end
bg:setLoc(0,0)
bg:setScl(1,1)
print( "prio:", bg:getPriority() )


-- fonts, textbox
charcodes = "0123456789"
font = MOAIFont.new()
font:loadFromTTF("cinecaption227.ttf", charcodes, 12, 163 )

textbox = MOAITextBox.new()
textbox:setString( "0" )
textbox:setFont(font)
textbox:setTextSize( font:getScale() )
textbox:setRect( -W/2+50,H/2,100,H/2-40)
textbox:setYFlip( true )
layer:insertProp(textbox)
function updateScore(n)
   textbox:setString( string.format("%d",n))
end

-- sounds
MOAIUntzSystem.initialize(44100,1024)
hitse = MOAIUntzSound.new()
hitse:load( "./hit02.wav",true )  
hitse:setVolume(0.5)
hitse:setLooping(false)
hitse:setPosition(0)




-- loop
function looper()
   local nowt,lastt
   while true do
      nowt = now()
      if not lastt then lastt = nowt - 0.01 end
      local dt = nowt - lastt 
      for i,v in ipairs(props) do
         v:update(dt)
         if v.toRemove then
            layer:removeProp(v)
            table.remove( props, i )
         end
      end
      lastt = nowt

      if math.random() < 0.01 then
         Tarken( -W/2 + W*math.random(), H/2 )
      end
      if math.random() < 0.02 then
         Zosie( -W/2 + W*math.random(), H/2 - 20 )
      end

      -- move bg
      local x,y=bg:getLoc()
      y = y - 20 * dt
      bg:setLoc(x,y)
      
      coroutine.yield()
   end
end
coro = MOAIThread.new()
coro:run( looper )
