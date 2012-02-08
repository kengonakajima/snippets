a = Hash.new

b = Hash.new

b["hoge"] = 12345

a["poo"] = b


print a["poo"]["hoge"] ,"\n"
a["poo"] = 23456


