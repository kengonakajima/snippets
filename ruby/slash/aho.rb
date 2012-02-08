

a= "\\\\"

print a.gsub( /\\/ , "\\" ) , "\n"
print a.gsub( /\\/ , "\\\\" ) , "\n"

b = "aa"

print b.gsub( /a/ , "a" ) , "\n"
print b.gsub( /a/ , "aa" ) , "\n"

