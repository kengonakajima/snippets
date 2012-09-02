a = {
  "aho" => { 
    "hoge" => 2,
    "fuga" => 3,
    5 => 10 
  },
  "hoge" => {
    "poo", "moo"
  }
}

print a,"\n"

sh = a.sort do |x,y|
  x[0] <=> y[0]
end

print sh, "\n"
