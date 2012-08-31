function ringbuf(sz)
  local b = {}
  b.buf = {}
  b.head = 1
  b.size = sz
  b.pos = 1
  function b:insert(v)
    self.buf[ self.head ] = v
    self.head = self.head + 1
    self.pos = self.pos + 1
    if self.head == self.size+1 then
      self.head = 1
    end
    print("new head:", self.head, "pos:", self.pos )
  end
  function b:getHead()
    local at = self.head - 1
    if at == 0 then
      at =
    return self.buf[ self.head ]
  end
  
  return b
end


r = ringbuf(4)

r:insert(100)
print( r:getHead() )

