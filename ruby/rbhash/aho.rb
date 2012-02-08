a = Hash.new

a["aho"] = "aho"
a["hoge"] = "hoge"
a["foo"] = "boo"
a["bar"] = "bar"
a["baz"] = "baz"

a.keys.each { |k|
    
    if( a[k] == "hoge" ) then
        a.delete(k)
    end
}

print a.keys.join(":") , "\n"
