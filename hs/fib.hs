fib :: Integer -> Integer
fib n = fibGen 0 1 n
fibGen :: Integer -> Integer -> Integer -> Integer
fibGen a b n = case n of
	0 -> a
	n -> fibGen b ( a+b ) (n-1)
	
fibs :: [Integer]
fibs = 0 : 1 : [ a + b | ( a, b ) <- zip fibs ( tail fibs)]

nums :: [Int]
nums = [ 4,2,6,8,5,11 ]




fooList :: [Int]
fooList = [ 3,1,5,4]

bar :: Int -> Int
bar n = n - 2

subEachFromTen :: [Int] -> [Int]
subEachFromTen = map ( 10 - )




