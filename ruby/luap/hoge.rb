# parse lua and print summaries
require "../rumino/rumino"
require "pp"

sexp=cmd( "ruby lua-parser/lua2sexp  ./lumino.lua" )
#sexp=cmd( "ruby lua-parser/lua2sexp ./p.lua" )

#p sexp

$defns=Hash.new(0)
$calls=Hash.new(0)

def s(*args)
  assert(args[0])
  t = args[0]
  if t == :function then
    funcname = args[1]
    if funcname then
      names = funcname[1]
      if names then
        n = names[1..-1].join(".")
        p "DEFN:", n
        $defns[n] += 1
      end
    end
  end
  if t == :call then
    pfexp = args[1]
    methname = args[2]
    args = args[3]
    if pfexp then
      var = pfexp[1]
      name = var[1]
      funcname = name[1..-1].join(".")
      n = funcname
      if funcname == "require" then
        explist = args[1]
        if explist then
          exp = explist[1]
          if exp then
            str = exp[1]
            if str then
              p "REQUIRE: #{str[1]}"
            end
          end
        end
      end
      if methname then
        mn = methname[1]
        n += "." + mn.to_s
      end
#      p "CALL:", n
      $calls[n] += 1
    end
  end
  return args
end

eval(sexp)
#p sexp


$calls.valsort.reverse.each do |name,cnt|
  p "CALL: #{name}: #{cnt}"
end






