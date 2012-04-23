def aho()
  hoge()
end

begin
  begin
    aho()
  rescue

    print "hoge 1 #{$!.backtrace}\n"
    raise $!.backtrace
  end
rescue
  print "hoge 2 #{$!}\n"
  print $!.backtrace,"\n"
end

# ->
# hoge 1
# hoge 2
