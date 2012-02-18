 

-- this code is released under the MIT License

-- Copyright (c) 2012 innovationtech.co.uk

-- <!-- m --><a class="postlink" href="http://www.opensource.org/licenses/mit-license.php">http://www.opensource.org/licenses/mit-license.php</a><!-- m -->

 

function addListItem(text,image)
  textbox = MOAITextBox.new ()
  textbox:setString ( text )
  textbox:setFont ( font )
  textbox:setTextSize ( font:getScale ())
  textbox:setRect ( -100,-100,100,100)
  textbox:setLoc(140,-140)
  textbox:setYFlip ( true )
  layer:insertProp ( textbox )
  items=items+1
end


-- INITIALISATION --

screen_width,screen_height=MOAIEnvironment.getScreenSize()
adirectory=MOAIEnvironment.getDocumentDirectory()
osbrand=MOAIEnvironment.getOSBrand()
if screen_width==0 then screen_width=800 end
if screen_height==0 then screen_height=480 end

if adirectory=='UNKNOWN' then
  assetdirectory=''
end
if adirectory~='UNKNOWN' then
  assetdirectory=adirectory..'/'
end


MOAISim.openWindow ( "test", screen_width, screen_height )

layer = MOAILayer2D.new ()
MOAISim.pushRenderPass ( layer )
 

viewport = MOAIViewport.new ()
viewport:setSize ( screen_width,screen_height )
viewport:setScale ( screen_width,screen_height)
layer:setViewport ( viewport )


alert_partition = MOAIPartition.new ()
layer:setPartition ( alert_partition )


charcodes = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 .,:;!?()&/-'
font = MOAIFont.new ()
font:loadFromTTF ( 'arial.TTF', charcodes, 20)
font2 = MOAIFont.new ()
font2:loadFromTTF ( 'arial.TTF', charcodes, 30)
 

items=0
padding=2
itemsize=24
oldx=0
oldy=0
oldlayerx=0
oldlayery=0
starty=1*screen_height/2-100
buttons=10
startx=-1*((buttons)/2)*(itemsize+padding)*2
letters={'q','w','e','r','t','y','u','i','o','p','a','s','d','f','g','h','j','k','l','z','x','c','v','b','n','m','@','$'}
inputtext=""
addListItem("","")

------------------------------

for variable = 1, 10, 1 do
  prop = MOAIProp2D.new ()
  gfxQuad = MOAIGfxQuad2D.new ()
  gfxQuad:setTexture ( "cathead.png" )
  gfxQuad:setRect ( -1*itemsize,-1*itemsize,itemsize,itemsize)
  prop:setDeck ( gfxQuad )
  prop:setPriority ( 5 )
  pox=variable*(itemsize+padding)*2
  prop.name = ""..letters[variable]
  layer:insertProp(prop)
  textboxkey = MOAITextBox.new ()
  textboxkey.name=""..letters[variable]
  textboxkey:setString ( ""..prop.name )
  textboxkey:setFont ( font2 )
  textboxkey:setTextSize ( font2:getScale ())
  textboxkey:setRect ( -1*itemsize,-1*itemsize,itemsize,itemsize)
  textboxkey:setLoc(startx+pox,starty)
  textboxkey:setAlignment(MOAITextBox.CENTER_JUSTIFY)
  textboxkey:setYFlip ( true )
  textboxkey:setPriority(10)
  layer:insertProp ( textboxkey )
  alert_partition:insertProp ( textboxkey )
  prop:setLoc(startx+pox,starty)
end

       
startx=startx+itemsize
starty=starty+(itemsize+padding)*2*-1

for variable = 1, 9, 1 do
  prop = MOAIProp2D.new ()
  gfxQuad = MOAIGfxQuad2D.new ()
  gfxQuad:setTexture ( "cathead.png" )
  gfxQuad:setRect ( -1*itemsize,-1*itemsize,itemsize,itemsize)
  prop:setDeck ( gfxQuad )
  prop:setPriority ( 5 )
  pox=variable*(itemsize+padding)*2
  prop.name = ""..letters[variable+10]
  layer:insertProp(prop)
  textboxkey = MOAITextBox.new ()
  textboxkey.name=""..letters[variable+10]
  textboxkey:setString ( ""..prop.name )
  textboxkey:setFont ( font2 )
  textboxkey:setTextSize ( font2:getScale ())
  textboxkey:setRect ( -1*itemsize,-1*itemsize,itemsize,itemsize)
  textboxkey:setLoc(startx+pox,starty)
  textboxkey:setAlignment(MOAITextBox.CENTER_JUSTIFY)
  textboxkey:setYFlip ( true )
  textboxkey:setPriority(10)
  layer:insertProp ( textboxkey )
  alert_partition:insertProp ( textboxkey )
  prop:setLoc(startx+pox,starty)
end

startx=startx+itemsize
starty=starty+(itemsize+padding)*2*-1

 
for variable = 1, 9, 1 do

  prop = MOAIProp2D.new ()

  gfxQuad = MOAIGfxQuad2D.new ()

  gfxQuad:setTexture ( "cathead.png" )

  gfxQuad:setRect ( -1*itemsize,-1*itemsize,itemsize,itemsize)

  prop:setDeck ( gfxQuad )

  prop:setPriority ( 5 )

  pox=variable*(itemsize+padding)*2

  prop.name = ""..letters[variable+19]

  layer:insertProp(prop)

  textboxkey = MOAITextBox.new ()

  textboxkey.name=""..letters[variable+19]

  textboxkey:setString ( ""..prop.name )

  textboxkey:setFont ( font2 )

  textboxkey:setTextSize ( font2:getScale ())

  textboxkey:setRect ( -1*itemsize,-1*itemsize,itemsize,itemsize)

  textboxkey:setLoc(startx+pox,starty)

  textboxkey:setAlignment(MOAITextBox.CENTER_JUSTIFY)

  textboxkey:setYFlip ( true )

  textboxkey:setPriority(10)

  layer:insertProp ( textboxkey )

  alert_partition:insertProp ( textboxkey )

  prop:setLoc(startx+pox,starty)

end



loopcomplete=0

function pointerCallback( x, y )
  local oldX = mouseX
  local oldY = mouseY    
  mouseX, mouseY = layer:wndToWorld ( x, y )

  pick = alert_partition:propForPoint ( mouseX, mouseY )
  if pick and pick~=oldpick and loopcomplete==1 then
    --print ( pick.name )
  end
  oldpick = pick
  loopcomplete=1
end

function clickCallback( down )
  if down then
    pick = alert_partition:propForPoint ( mouseX, mouseY )
    if pick then
      --print ( pick.name )
      if (pick.name=='$') then
        --print (string.len("test"))
        inputtext=string.sub(inputtext, 0,string.len(""..inputtext)-1)
      end
      if (pick.name~='$') then
        inputtext=inputtext..pick.name
      end
      textbox:setString(inputtext)
    end
  else
    if pick then
      pick = nil
    end
  end
end


if MOAIInputMgr.device.pointer then
  
  -- mouse input
  MOAIInputMgr.device.pointer:setCallback (
      function (x,y)
        pointerCallback(x,y)
      end
    )
  
  MOAIInputMgr.device.mouseLeft:setCallback (
      function (eventType)
        clickCallback(eventType)
      end
    )
else
  
  -- touch input
  MOAIInputMgr.device.touch:setCallback (
      
      function ( eventType, idx, x, y, tapCount )
        
        pointerCallback ( x, y )
        
        if eventType == MOAITouchSensor.TOUCH_DOWN then
          clickCallback ( true )
        elseif eventType == MOAITouchSensor.TOUCH_UP then
          clickCallback ( false )
        end
      end
    )
end



