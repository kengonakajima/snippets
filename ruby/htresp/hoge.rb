# http responder class

require "../rumino/rumino"

class HTTPResponders
  def default(req,res)
    hoge = req.paths.join(":")
    p "default:",req, hoge
    res.sendHTML( "<body>#{hoge}</body>\n")
  end
  def hoge(req,res)
    p "hoge:",req,res  
  end
end



web = MiniWeb.new()
web.configure( { "webPort"=>9999 } )
web.onGET() do |req,res|
  p "onGET", req.path
  httpRespond(req,res,HTTPResponders)
end


web.start()
