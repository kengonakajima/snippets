a,b = pcall( function()
          print("aho")
          error("aho")
          return 1,2
       end)

print("AAAAAA:",a,b)