fs = require("fs")

fs.rmdir( "a" )
fs.mkdir( "a", 755)

xpcall( function()
    fs.mkdir( "a", 755)
  end,
  function(e)
    print(e)
  end
)
p("xx")

