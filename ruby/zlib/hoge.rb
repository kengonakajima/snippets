require "zlib"

s = "asjdhfkjashdlfjkhasjdkhfgaskdjhfgaksdjfhgaksjdhfgkajsdhfgkajshdgfkjahsdgfkjahsdgfkjahsdgkfjhasgdkfjhagskdjhfgakjshdfgkajhsdfgkajhsdgfkajhsdfgkajhsdfgkajhsdfgkajhsdgfkajhsdgfkajhdfgkajhdgfkjahsdgkfjahgsdkfjhasgdkfjhagsdkfhg"

ss = Zlib::Deflate.deflate(s)
sss = Zlib::Inflate.inflate(ss)

print s.length,"\n"
print ss.length,"\n"
print sss.length,"\n"
print sss==s,"\n"