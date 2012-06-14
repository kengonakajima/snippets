class A
  def hoge()
    print("hoge\n")
  end
end
def hashy(t)
  src = <<EOF
class #{t}
  def poo()
    print("poo\n")
  end
end
EOF

  eval(src)
end

hashy(A)

a=A.new
a.hoge()
a.poo()
