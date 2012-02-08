require 'net/http'
Net::HTTP.version_1_2   # おまじない
Net::HTTP.start('0.0.0.0', 80) {|http|
  response = http.get('/index.html')
  puts response.body
}
