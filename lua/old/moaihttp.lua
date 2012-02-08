----------------------------------------------------------------
-- Copyright (c) 2010-2011 Zipline Games, Inc. 
-- All Rights Reserved. 
-- http://getmoai.com
----------------------------------------------------------------

MOAISim.openWindow ( "test", 320, 320)

function onFinish ( task, responseCode )
   print ( "onFinish:", responseCode )

   if responseCode > 0 then
      print("task.getsize:", task:getSize() )
   else
      error("responseCode zero")
   end   
   startTask(1)   
end

function startTask(n)
   print("startTask n:",n)
   for i=1,n do
      local task = MOAIHttpTask.new ()
      task:setCallback ( onFinish )
      local url = "http://127.0.0.1/ringo/tablebase1024.png" 
      url = "http://127.0.0.1:8091/images/tablebase1024.png"
      url = "http://127.0.0.1:8091/images/dobutushogi_chook128.png"
      task:httpGet ( url )
   end
end

startTask(15)