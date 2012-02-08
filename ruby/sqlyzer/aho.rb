require 'rubygems'
require 'sqlyzer'

class Test
  include Sqlyzer::Serializer

  sql_use_table  'TestingTable'
  sql_has_keys   :lastname, :firstname/SQL_TEXT
  sql_has_values :age/SQL_INT2, :birth/Time.now, :male/SQL_BOOL

  def initialize(firstname, lastname, age, male = false)
    @firstname = firstname
    @lastname = lastname
    @age = age
    @male = male
  end

end

Sqlyzer::Db::connect 'Pg', 'localhost', 'sqlyzer', 'postgres', 'postgres'
Test.new('sexy', 'jimmy', 42).sql_new
