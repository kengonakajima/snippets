Char={}
function Char.new(x,y)
   local o = {}
   o.x = x
   o.y = y
   return setmetatable( o, { __index = Char } )
end

Player={}
setmetatable( Player, { __index = Char } )
function Player.new(x,y,nm)
   local o = Char.new(x,y)
   o.name = nm 
   return setmetatable( o, { __index = Player } )
end


local p = Player.new(10,20,"ringo" )

print( "P:", p.x, p.y, p.name )

function Char_new(x,y)
   local o = {}
   o.x = x
   o.y = y
   return o
end
function Player_new(x,y,nm)
   local o = Char_new(x,y)
   o.name = nm
   return o
end

p = Player_new(30,40,"kengo")

print( "P:", p.x, p.y, p.name )