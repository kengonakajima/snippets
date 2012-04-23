
begin
  begin
    aho()
  rescue
    print "hoge 1\n"
    raise $!
  end
rescue
  print "hoge 2\n"
  print $!,"\n"
end

# ->
# hoge 1
# hoge 2
