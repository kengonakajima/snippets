require "rubygems"
require "sinatra"
require "erb"
require "json"
require "slim"

helpers do
  def mul(x)
    x * 2
  end
end

get "/" do
  "<html><head><title>xx</title><body>hello world</body></html>"
end

get "/snippet/:name" do
  n = params[:name]
  "snippet name:#{n} #{n} #{n}"
end


get "/poo" do
  @poo = 1234
  erb :t, :locals => { :localpoo => 23456, :agent => request.user_agent, :lang => request.env["HTTP_ACCEPT_LANGUAGE"] }
end

not_found do
  "not found!!!!!!!!!!"
end

get "/slim" do
  slim :s, :locals => { :localpoo => 23456, :agent => request.user_agent, :lang => request.env["HTTP_ACCEPT_LANGUAGE"] }
end
