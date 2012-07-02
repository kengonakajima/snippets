def a()
  q=1
  def c()
    print("C",q)
  end
  def b()
    print("B")
    c()
  end

  b()
end
a()
