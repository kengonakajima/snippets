function hoge()
  local a = 2
  local f = function(b)
    return a * b
  end
  a = 3
  return f
end
f = hoge()
print(f(3))

function foo(a)
  local f = function(b)
    return a * b
  end
  a = 3
  return f
end

f = foo(2)
print(f(3))
