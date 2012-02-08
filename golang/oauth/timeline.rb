#!/usr/bin/env ruby
# coding: utf-8

# http://d.hatena.ne.jp/shibason/20090802/1249204953  に基づくコード


require 'rubygems'
require 'oauth'
require 'json'

CONSUMER_KEY = 'Vj1kPZIdQWvwjsanAzyxCA' # ←ここを書き換える
CONSUMER_SECRET = 'G3pi811H2VaQbEipRZF9XWKA8Miz4Ubx9lqMesLyzsQ' # ←ここを書き換える
ACCESS_TOKEN = '52653-WItLBZVz57JevBFyewEkgisW4KNiMpzgebILJEkcy9k' # ←ここを書き換える
ACCESS_TOKEN_SECRET = 'DbUjJcNvKw9CWSB3pImI7v8Jj8LZOY0aWzyfkjdPJZE' # ←ここを書き換える

# 下準備
consumer = OAuth::Consumer.new(
  CONSUMER_KEY,
  CONSUMER_SECRET,
  :site => 'http://twitter.com'
)
access_token = OAuth::AccessToken.new(
  consumer,
  ACCESS_TOKEN,
  ACCESS_TOKEN_SECRET
)

# タイムラインを取得して時系列順に表示
#response = access_token.get('http://twitter.com/statuses/friends_timeline.json')
response = access_token.get('http://api.twitter.com/1/statuses/friends_timeline/12345.json')
JSON.parse(response.body).reverse_each do |status|
  user = status['user']
  puts "#{user['name']}(#{user['screen_name']}): #{status['text']}"
end

# Tweetの投稿
#response = access_token.post(
#  'http://twitter.com/statuses/update.json',
#  'status'=> 'このメッセージはOAuth認証を通して投稿しています。'
#)
