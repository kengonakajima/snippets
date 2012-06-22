#
# racc footer
#


if ARGV.size < 1 then 
  STDERR.print <<EOF
Need input file(s).
Options:
 -q : be quiet(parse only)
 -x : test by executing out-put sexp
 -a : print as array literal
EOF
  exit 1
end

fmt = "s"
exectest = false

ARGV.each do |arg|
  if arg =~ /^-/ then
    if arg == "-q" then
      fmt = nil
    elsif arg == "-x"
      exectest = true
    elsif arg == "-a"
      fmt = "a"
    end
    
  else
	lp = JS.new
    s = File.open(arg).read
    lp.parse(s,fmt,exectest)
  end
end




    

