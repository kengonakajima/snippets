require "html/template"


t = HTML::Template.new("./test.html")


parent = [
    {
        "hogelist" => [
            { 
                "vv" => "nasu",
                "va" => [
                    { "vvv" => "kamokamo" }
                ]
            }
        ]
    },
    {
        "hogelist" => [
            { 
                "vv" => "otanko",
                "va" => [
                    { "vvv" => "tamotamo" }
                ]
            }
        ]
    }
]

t.param( "aholist" => parent )


print "Content-type: text/html\n\n"
print t.output

