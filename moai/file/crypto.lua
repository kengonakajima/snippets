--

print(crypto)
print(crypto.evp.digest)

md5 = crypto.evp.new("md5")

dg = md5:digest("asdf\n")
assert(dg == "2b00042f7481c7b056c4b410d28f33cf") -- got from osx md5 command

sha1 = crypto.evp.new("sha1")

h = sha1:digest("asdf\n")
assert(h=="7d97e98f8af710c7e7fe703abc8f639e0ee507c4")

print("test ok!")



