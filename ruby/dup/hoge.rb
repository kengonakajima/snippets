a = { "AHO"=>"HOGE", "HOGE"=>"MOGE"};


print a["AHO"]
a["AHO"]="PAPAPA"
print a["AHO"]
b = a
b["AHO"]="NASU"
print a["AHO"]
c=a.dup
c["AHO"]="AAAAAAAAAAAAAAAA"
print a["AHO"]
print c["AHO"]
