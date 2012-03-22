require 'webrick'
srv = WEBrick::HTTPServer.new({ :DocumentRoot => './',
                                :BindAddress => '0.0.0.0',
                                :Port => 20080})


class HogeServlet < WEBrick::HTTPServlet::AbstractServlet
  def do_GET(req,res)
    STDERR.print "GET req:", req, "\n"
    res.content_type="text/plain"
    res.body = "hogehogeget"
    return
  end
  def do_POST(req,res)
    STDERR.print "POST req:", req, "\n"
    res.content_type="text/plain"
    res.body = "hogehogepost"
  end
end

srv.mount( "/hoge", HogeServlet )

#srv.mount('/foo.html', WEBrick::HTTPServlet::FileHandler, 'hoge.html')

trap("INT"){ srv.shutdown }
trap("TERM"){ srv.shutdown }

srv.start
