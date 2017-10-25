kw=ARGV[0]


while true
  if kw then 
    system("nstat | grep #{kw}")
  else
    system("nstat")
  end
  sleep 1
end
