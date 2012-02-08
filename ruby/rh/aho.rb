
h = { "aho" => 1, "hoge" => 2 }


a = Hash.new

h.keys.each { |k|

    print "KEY: #{k}\n"
    a[k] = h[k]
}


print h.keys.join(":"), "\n"
print a.keys.join(":"), "\n"

