require 'sinatra/base'
require 'webrick'
require 'webrick/https'
require 'hashie'
require 'pathname'
require "json"


class MyServer  < Sinatra::Base
  post '/', provides: :json do
	params = JSON.parse(request.body.read)
    open( "saved." + DateTime.now.strftime( "%Y%m%d%H%M" ), 'a'){ |f| f.puts( params ) }
    end
end

webrick_options = {
  :Port               => 8888,
  :Logger             => WEBrick::Log::new($stderr, WEBrick::Log::DEBUG),
  :DocumentRoot       => "/",
  :app                => MyServer
}
Rack::Server.start webrick_options
