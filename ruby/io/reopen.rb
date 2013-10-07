open("output.txt", "a") do |f|
  $stdout = f
  puts "Hello at #{Time.now}"
  $stdout = STDOUT
  puts "Hello2 at #{Time.now}"
end
