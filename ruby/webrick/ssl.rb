require 'webrick'
require 'webrick/https'



# ここで自由に MIME を設定できるのが便利
mime = WEBrick::HTTPUtils::DefaultMimeTypes.merge({
  "js"       => "text/javascript",
  "json"     => "text/json",
  "log"      => "text/plain",
  "md"       => "text/plain",
  "markdown" => "text/plain",
})

config = {
  :Port         => 8443,
  :DocumentRoot => '.',
  :MimeTypes    => mime,
  :SSLEnable    => true,
  :SSLCertName  => [["CN", "devserver"]], # 証明書を自動で生成してくれる
}

server = WEBrick::HTTPServer.new(config)
server.mount_proc("/") do |req,res|
  if req.request_method == "POST" then
    print req.methods,"\n"
    res.body = req.body
  end
end

Signal.trap('INT') { server.shutdown }
server.start
