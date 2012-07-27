require( "./lumino/lumino")

t=measure(function()
    for i=1,10000000 do
      now()
    end
  end)
print(t)
