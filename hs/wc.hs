outw [] = 0
outw (' ':cs) = outw cs
outw (c:cs) = 1 + inw cs

inw [] = 0
inw (' ':cs) = outw cs
inw (c:cs) = inw cs


foo :: Int -> Int

foo bar = bar * 2

