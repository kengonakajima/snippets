require "net/http"


Net::HTTP.version_1_1


req = Net::HTTP::Get.new( "/~ringo/aho.txt")



if( fork() == nil ) then
    # Get file's [0] and [1]
    req.range =  0..1
    f = File.open( "aho.txt.1","w")
else
    # Get file's [2] and [3]
    req.range =  2..3
    f = File.open( "aho.txt.2","w")
end

Net::HTTP.start( "nsx" ) { |http|
    response = http.request( req )
    print response.body, "\n"
    f.write response.body
}

f.close
