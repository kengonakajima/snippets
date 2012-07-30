function a()
  return b()
end

function b()
  return c()
end
function c()
  return 1
end

print( a() )

