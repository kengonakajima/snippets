
a = "りんご\"asdうんこfas\"りうんぎお。"

b = "\"asdうんこfas\""
a.gsub!( /#{b}/ , "BMC" )

print a  , "\n"
