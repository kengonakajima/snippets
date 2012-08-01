local string = require("string")
local table = require("table")

function XPMImage(w,h)
  local img = {}
  img.colors = {}
  img.pixels = {}
  img.width, img.height = w,h
  function img:addColor(r,g,b)
    local s = string.format( "#%02x%02x%02x", r,g,b)
    table.insert( self.colors, s )
    if #self.colors > 52 then
      error("too many colors")
    end
    return #img.colors    
  end
  
  function img:set(x,y,colorid)
    local ix,iy = x+1, y+1
    if self.pixels[iy] == nil then
      self.pixels[iy] = {}
    end
    local row = self.pixels[iy]
    row[ix] = colorid
  end

  function img:out(name)
    
    local outary = {}

    local chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    local charary = {}
    for i=1,#chars do
      table.insert( charary, chars:sub(i,i) )
    end
    
    for i,v in ipairs(self.colors) do
      table.insert( outary, string.format( "\"%s c %s\",\n", charary[i], self.colors[i] ) )
    end
      
    for y=0,self.width-1 do
      local row = self.pixels[y+1]
      local rowstrary = {}
      for x=0,self.height-1 do
        if not row then
          table.insert( rowstrary, "." )
        else
          local pixel = row[x+1]
          if pixel then
            table.insert( rowstrary, charary[pixel] )
          else
            table.insert( rowstrary, "." )
          end
        end
      end
      table.insert( outary, "\"" .. table.concat( rowstrary ) .. "\",\n" )
    end
    return  "static const char* const " .. name .. "[] = {\n" ..
      string.format( "\"%d %d %d %d\",\n", self.width, self.height, (#self.colors+1), 1 ) ..
      "\". c None\",\n" .. 
      table.concat( outary ) .. "};\n"
  end
  
  return img
end

  
xpm = XPMImage(10,10)

c1 = xpm:addColor(255,0,0)
c2 = xpm:addColor(0,255,0)
xpm:set(1,1,c1)
xpm:set(2,2,c1)
xpm:set(7,7,c2)
xpm:set(8,8,c2)

out = xpm:out( "hoge") 
print( out )

