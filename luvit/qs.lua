require("./lumino/lumino")
u=require("utils")
JSON=require("json")

local qs = require("querystring")

a = "payload={\"aho\":1,\"hoge\":1000}&opt=AAAAAAAAAAAAAAAAA"

t = qs.parse(a)

tt = JSON.parse( t["payload"])


u.prettyPrint(tt)

