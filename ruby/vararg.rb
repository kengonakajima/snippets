def a(*args)
  args.each do |v| print("v:#{v} #{v.class}\n") end
  b(*args)
end
def b(*args)
  args.each do |v| print("vb:#{v} #{v.class}\n") end
end

a(1,2,3)

