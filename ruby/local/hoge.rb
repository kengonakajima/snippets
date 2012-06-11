def hoge(a,b,c)
  print local_variables.join(","),"\n"
  local_variables.each do |name|
    puts eval name
  end
end

hoge(1,2,3)
