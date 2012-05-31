
def a(h)
  h.keys.each do |k|
    print k,"\n"
  end
end

a :hoge => 100, :piyo => "fuga"
