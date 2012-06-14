require "net/http"

print(URI)

u = URI.parse("http://hoge.com/aho?a=1&b=2")

print("path:",u.path,"\n" )
print("query:",u.query,"\n")
print("queryA:",u.query.class,"\n")

