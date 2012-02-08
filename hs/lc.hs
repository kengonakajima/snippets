main = lc

lc ('\n':cs) = 1 + lc cs
lc (c:cs) = lc cs
lc [] = 0