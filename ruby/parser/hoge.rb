require "rubygems"
require 'ruby_parser'
require "../rumino/rumino"

@p = RubyParser.new

#s=readFile("../rumino/rumino.rb")
s=readFile("src.rb")

out=@p.parse(s)


$defns = Hash.new(0)
$methcalls = Hash.new(0)
$gcalls = Hash.new(0)
$classes = Hash.new(0)
$classdefs = {}
$requires = Hash.new(0)
$consts = Hash.new(0)
$methhist = Hash.new(0)

def s(*args)
  case args[0]
  when :call 
    if args[2] == :require and typeof(args[3])==Array then
      arglist = args[3]
      if typeof(arglist[1])==Array then
#        p "aa:",typeof(arglist[1][1])
        if typeof(arglist[1][1])==String then
          $requires[ arglist[1][1] ]+=1
#          p "REQUIRE #{arglist[1][1]}"
        end
      end
    end
    if args[1] == nil then
      $gcalls[args[2]]+=1
    else
      if typeof(args[1])==Array then
        if args[1][0] == :lvar then
          if args[2].to_s =~ /^[a-zA-Z]/ then
            m = "#{args[1][1]}.#{args[2]}"
          else
            m = "#{args[1][1]}#{args[2]}"
          end
          $methhist[m]+=1
        end
      end
      $methcalls[args[2]]+=1
    end
  when :defn
    $defns[args[1]]+=1
  when :class
    $classes[args[1]]+=1
    scope = args[3]
    blk = scope[1]
    blk.each do |b|
      if typeof(b)==Array then
        defn = b[0]
        if defn == :defn then
#          p "CLASS #{args[1]} defs #{b[1]}"
#          p typeof($classdefs[args[1]])
          if $classdefs[args[1]]==nil then 
            $classdefs[args[1]] = []
          end
          $classdefs[args[1]].push( b[1] )
        end
      end
    end
  when :const
    $consts[args[1]]+=1
  end
  return args
end

eval(out.to_s)

$requires.keys.each do |name|
  print "REQUIRE: #{name}\n"
end
$classes.keys.each do |name|
  print "CLASS: #{name}\n"
end

$classdefs.each do |name,ary|
  print "CLASS #{name} DEFS: #{ary.join(',')}\n"
end

$defns.keys.each do |name|
  print "DEFN: #{name}\n"
end
$consts.valsort.reverse.each do |name,cnt|
  print "CONST: #{name} : #{cnt}\n"
end
$gcalls.valsort.reverse.each do |name,cnt|
  print "GCALL: #{name} : #{cnt}\n"
end
$methcalls.valsort.reverse.each do |name,cnt|
  print "METHCALL: #{name} : #{cnt}\n"
end

$methhist.valsort.reverse.each do |k,v|
  print "METHHIST: #{k} : #{v}\n"
end

print out.to_s
