require "sidekiq"

Sidekiq.configure_client do |config|
  config.redis = { :namespace => 'x', :size => 1 }
end

Sidekiq.configure_server do |config|
  config.redis = { :namespace => 'x' }
end


class PlainOldRuby
  include Sidekiq::Worker

  def perform(how_hard="super hard", how_long=1)
    sleep how_long
    puts "Workin' #{how_hard}"
  end
end
