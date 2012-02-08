require "rubygems"
require "dm-core"

# If you want the logs displayed you have to do this before the call to setup
  DataMapper::Logger.new($stdout, :debug)

  # An in-memory Sqlite3 connection:
#  DataMapper.setup(:default, 'sqlite::memory:')

  # A MySQL connection:
  DataMapper.setup(:default, 'mysql://localhost/datamapper_test')

  # A Postgres connection:
#  DataMapper.setup(:default, 'postgres://localhost/the_database_name')


class Post
  include DataMapper::Resource

  property :id,         Serial   # An auto-increment integer key
  property :title,      String   # A varchar type string, for short strings
  property :body,       Text     # A text block, for longer string data.
  property :created_at, DateTime # A DateTime, for any date you might like.
end

class Comment
  include DataMapper::Resource

  property :id,         Serial
  property :posted_by,  String
  property :email,      String
  property :url,        String
  property :body,       Text
end

class Category
  include DataMapper::Resource

  property :id,         Serial
  property :name,       String
end

class Post
  has n, :comments
end

class Comment
  belongs_to :post
end

class Categorization
  include DataMapper::Resource

  property :id,         Serial
  property :created_at, DateTime

  belongs_to :category
  belongs_to :post
end

# Now we re-open our Post and Categories classes to define associations
class Post
  has n, :categorizations
  has n, :categories, :through => :categorizations
end

class Category
  has n, :categorizations
  has n, :posts,      :through => :categorizations
end

DataMapper.finalize

require  'dm-migrations'

DataMapper.auto_migrate!
DataMapper.auto_upgrade!

# create makes the resource immediately
@post = Post.create(
  :title      => "My first DataMapper post",
  :body       => "A lot of text ...",
  :created_at => Time.now
)

# Or new gives you it back unsaved, for more operations
@post = Post.new(:title => "asdf", :body => "body" )
@post.save                           # persist the resource


@hoge = Post.all()
@hoge.each do |x|
  print x.title
end

