local ffi = require("ffi")
local bit = require("bit")
local libgd = ffi.load("gd")

require "lumino"

print("libgd:", libgd )


ffi.cdef[[
void *gdImageCreate( int w, int h );
int gdImageColorAllocate( void *imgptr, int r, int g, int b );
void gdImageLine( void *imgptr, int x0, int y0, int x1, int y1, int color );
void gdImageCopy( void *destimg, void *srcimg, int destx, int desty, int fromx, int fromy, int w, int h );
void *gdImagePngPtr( void *srcimg, unsigned int *outsz );
void gdFree( void *ptr );
]]

local gd = {
  imageCreate = libgd.gdImageCreate,
  imageColorAllocate = libgd.gdImageColorAllocate,
  imageLine = libgd.gdImageLine,
  imageCopy = libgd.gdImageCopy,
  imagePngPtr = libgd.gdImagePngPtr,
  free = libgd.gdFree
}

gd.imagePng = function(scr)
  local outsize_store = ffi.new( "unsigned int[1]" )
  local out = gd.imagePngPtr(scr, outsize_store)
  
--  print("outsz:", outsize_store[0] )
  local str = ffi.string(out,outsize_store[0])
  gd.free(out)
  return str
end


local SCRW = (640/2)
local SCRH = (480/2)


local scr = gd.imageCreate( SCRW, SCRH )
local im = gd.imageCreate( 32, 32 )

local white = gd.imageColorAllocate ( im, 255, 255, 255 ); -- 最初の割当だから, 白色が背景色になる 
local black = gd.imageColorAllocate ( im,   0,   0,   0 );
local red   = gd.imageColorAllocate ( im, 255,   0,   0 );

print("colors:",white, black, red )

-- スプライトの絵を作る
gd.imageLine ( im, 0, 0, 32, 32, black )  
gd.imageLine ( im, 0, 32, 32, 0, red )


local n = 500
local st = now()
print("st:",st)

for i=1,n do
  for j=1,200 do
    local x = irange(0,600)
    local y = irange(0,400)
    gd.imageCopy( scr, im, x,y, 0,0, 32,32 )
  end
  local pngstr = gd.imagePng( scr )
  print("pngstrlen:", #pngstr)
  if( n == 5 ) then
    writeFile( "poo.png", pngstr )
  end
end

local et = now()

print( string.format("%.2f sec for %d loop, (%.1f/sec)", (et-st), n, n / (et-st) ))

