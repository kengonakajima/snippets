(call-with-current-continuation
 (lambda (exit)
   (for-each (lambda (x)
               (if (negative? x)
                   (exit x)))
             '(54 0 37 -3 246 19))
   #t))

