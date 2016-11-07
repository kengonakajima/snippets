require 'sinatra/base'
require 'webrick'
require "json"


class MyServer  < Sinatra::Base
  post '/', provides: :json do
	data = request.body.read
    open( "saved." + DateTime.now.strftime( "%Y%m%d%H%M" ), 'a'){ |f| f.write(data) }
    end
end

webrick_options = {
  :Port               => 8888,
  :Logger             => WEBrick::Log::new($stderr, WEBrick::Log::DEBUG),
  :DocumentRoot       => "/",
  :app                => MyServer
}
Rack::Server.start webrick_options
