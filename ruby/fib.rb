# フィボナッチ数
def fib(n)
    if (n < 2)
        return n
    else
        return fib(n - 2) + fib(n - 1)
    end
end
puts fib(ARGV[0].to_i)

=begin
example
    ruby fib_rb.rb 39
=end
