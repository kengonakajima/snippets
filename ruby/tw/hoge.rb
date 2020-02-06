require "twitter"

client=Twitter::REST::Client.new do |config|
  config.consumer_key = ENV["TWITTER_API_KEY"]
  config.consumer_secret = ENV["TWITTER_API_SECRET_KEY"]
  config.access_token = ENV["TWITTER_ACCESS_TOKEN"]
  config.access_token_secret = ENV["TWITTER_ACCESS_TOKEN_SECRET"]  
end

while line=gets do
    line.strip!
    next if line.empty?
    begin
      result = client.search(line, count:10, result_type: "recent", exclude: "retweets" )
      result.take(50).each do |tw|
        print tw.attrs[:text],"\n"
        print "\n"
      end
    rescue
      print "ERROR!\n"
    end
end
  


