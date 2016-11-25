require "base64"
s = "このメソッドは [RFC2045] に対応しています。 エンコード後の文字で 60 文字ごとに改行を追加します。"

ss = Base64.encode64(s)

print ss,"\n"

sss = Base64.decode64(ss)

print s,"\n"
print sss,"\n"
print s.inspect,"\n"
print sss.inspect,"\n"
print s.length,"\n"
print sss.length,"\n"