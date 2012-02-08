
def aho
  yield 2 
  yield 3
end


aho { |i|
  print i, "\n"
}
