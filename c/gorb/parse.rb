
require "rubygems"
require "parse_tree"

pt = ParseTree.new
ast = pt.process( File.read( ARGV[0] ))



def prints(ind,ast)
  ind.times do 
    print " " 
  end
  print "("

  ast.each do |a|
    if a.class == Symbol then
      print a," "
    elsif a == nil then
      print "nil "
    elsif a == true then
      print "true "
    elsif a == false then
      print "false "
    elsif a.class == Fixnum or a.class == Bignum or a.class == Float or a.class == Regexp then
      print a, " "
    elsif a.class == String then
      print "'#{a}' "
    else
      print "\n"                                                      
      prints(ind+1,a)
    end
  end
  print ") "
end

prints(0,ast)
print "\n---------------\n"
print ast
print"\n"
