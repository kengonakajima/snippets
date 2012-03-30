# get words from code

src = File.open( ARGV[0], "r" ).read


cnth={}

tks = src.split( /\W/ )
tks.each do |tk|
  tk.strip!
  next if tk.empty?
  if cnth[tk] == nil then
    cnth[tk]=1
  else
    cnth[tk] += 1
  end
end

sorted = cnth.sort_by { |key,val| -val }
sorted.each do |pair|
  print "#{pair[0]} : #{pair[1]}\n"
end



