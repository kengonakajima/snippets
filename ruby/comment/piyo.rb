a = <<EOF
hoge = "aksjdfasdf "
hoge = "aksdfasdf \
asdfasdfasdf \
asdfasdfasdf"

piyo = 'asdfasdf'

popo = 'asdfasdf \
asdfasdf \
asdf'

pugo = " \\" asdfasdfas "
puho = ' \\' asdfasd fas dfa sdfasdf '

EOF

def removeStringLit(s)
  s = s.dup

  s.gsub!("\\\n","")
  s.gsub!("\\\"", " ")
  s.gsub!("\\\'", " ")

  out=[]
  until s.empty?
    case s
    when /\A\".*?\"/
    when /\A\'.*?\'/
    when /\A.?/m
      m = $&
      out.push(m)
    else
      raise "invalid:#{s}"
    end
    s = $'
  end
  return out.join("")
end

print removeStringLit(a)
