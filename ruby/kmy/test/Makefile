OUT=../lua2sexp
CMD=time ruby
OPT=-q -x

all: test testpl 

test: $(OUT)
	$(CMD) $(OUT) mini.lua
	$(CMD) $(OUT) hello.lua
	$(CMD) $(OUT) table.lua
	$(CMD) $(OUT) $(OPT) ret2.lua
	$(CMD) $(OUT) $(OPT) num.lua
	$(CMD) $(OUT) $(OPT) var.lua	
	$(CMD) $(OUT) $(OPT) mlstr.lua
	$(CMD) $(OUT) $(OPT) long.lua
	$(CMD) $(OUT) $(OPT) forin.lua
	$(CMD) $(OUT) $(OPT) for.lua
	$(CMD) $(OUT) $(OPT) repeat.lua
	$(CMD) $(OUT) $(OPT) while.lua
	$(CMD) $(OUT) $(OPT) do.lua
	$(CMD) $(OUT) $(OPT) local.lua
	$(CMD) $(OUT) $(OPT) bool.lua
	$(CMD) $(OUT) $(OPT) unops.lua
	$(CMD) $(OUT) $(OPT) ops.lua
	$(CMD) $(OUT) $(OPT) lcomment.lua
	$(CMD) $(OUT) $(OPT) comment.lua
	$(CMD) $(OUT) $(OPT) table3.lua
	$(CMD) $(OUT) $(OPT) lstr.lua
	$(CMD) $(OUT) $(OPT) nstr.lua
	$(CMD) $(OUT) $(OPT) elseif.lua
	$(CMD) $(OUT) $(OPT) ifthen.lua
	$(CMD) $(OUT) $(OPT) dot3f.lua
	$(CMD) $(OUT) $(OPT) table2.lua
	$(CMD) $(OUT) $(OPT) table1.lua
	$(CMD) $(OUT) $(OPT) dot3.lua
	$(CMD) $(OUT) $(OPT) func2.lua
	$(CMD) $(OUT) $(OPT) func.lua
	$(CMD) $(OUT) $(OPT) empty.lua
	$(CMD) $(OUT) $(OPT) mini.lua
	$(CMD) $(OUT) $(OPT) ret.lua
	$(CMD) $(OUT) $(OPT) semi.lua
	$(CMD) $(OUT) $(OPT) lumino.lua

testpl:
	$(CMD) $(OUT) $(OPT) pl/xml.lua
	$(CMD) $(OUT) $(OPT) pl/app.lua
	$(CMD) $(OUT) $(OPT) pl/array2d.lua
	$(CMD) $(OUT) $(OPT) pl/class.lua
	$(CMD) $(OUT) $(OPT) pl/comprehension.lua
	$(CMD) $(OUT) $(OPT) pl/config.lua
	$(CMD) $(OUT) $(OPT) pl/data.lua
	$(CMD) $(OUT) $(OPT) pl/Date.lua
	$(CMD) $(OUT) $(OPT) pl/file.lua
	$(CMD) $(OUT) $(OPT) pl/func.lua
	$(CMD) $(OUT) $(OPT) pl/init.lua
	$(CMD) $(OUT) $(OPT) pl/input.lua
	$(CMD) $(OUT) $(OPT) pl/lapp.lua
	$(CMD) $(OUT) $(OPT) pl/lexer.lua
	$(CMD) $(OUT) $(OPT) pl/List.lua
	$(CMD) $(OUT) $(OPT) pl/luabalanced.lua
	$(CMD) $(OUT) $(OPT) pl/Map.lua
	$(CMD) $(OUT) $(OPT) pl/MultiMap.lua
	$(CMD) $(OUT) $(OPT) pl/operator.lua
	$(CMD) $(OUT) $(OPT) pl/OrderedMap.lua
	$(CMD) $(OUT) $(OPT) pl/path.lua
	$(CMD) $(OUT) $(OPT) pl/permute.lua
	$(CMD) $(OUT) $(OPT) pl/pretty.lua
	$(CMD) $(OUT) $(OPT) pl/seq.lua
	$(CMD) $(OUT) $(OPT) pl/Set.lua
	$(CMD) $(OUT) $(OPT) pl/sip.lua
	$(CMD) $(OUT) $(OPT) pl/strict.lua
	$(CMD) $(OUT) $(OPT) pl/stringio.lua
	$(CMD) $(OUT) $(OPT) pl/stringx.lua
	$(CMD) $(OUT) $(OPT) pl/tablex.lua
	$(CMD) $(OUT) $(OPT) pl/template.lua
	$(CMD) $(OUT) $(OPT) pl/test.lua
	$(CMD) $(OUT) $(OPT) pl/text.lua
	$(CMD) $(OUT) $(OPT) pl/utils.lua


