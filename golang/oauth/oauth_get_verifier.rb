#!/usr/bin/env ruby
# coding: utf-8

# http://d.hatena.ne.jp/shibason/20090802/1249204953  にもとづくコード
require 'rubygems'
require 'oauth'
#require 'oauth-patch'

CONSUMER_KEY = 'Vj1kPZIdQWvwjsanAzyxCA' # ←ここを書き換える
CONSUMER_SECRET = 'G3pi811H2VaQbEipRZF9XWKA8Miz4Ubx9lqMesLyzsQ' # ←ここを書き換える

consumer = OAuth::Consumer.new(
  CONSUMER_KEY,
  CONSUMER_SECRET,
  :site => 'http://twitter.com'
)

request_token = consumer.get_request_token

puts "Access this URL and approve => #{request_token.authorize_url}"

print "Input OAuth Verifier: "
oauth_verifier = gets.chomp.strip

access_token = request_token.get_access_token(
  :oauth_verifier => oauth_verifier
)

puts "Access token: #{access_token.token}"
puts "Access token secret: #{access_token.secret}"
