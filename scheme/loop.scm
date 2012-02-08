(define loop (lambda (n)
               (if (= n 0)
                   1
                   (let ( (res (* (loop (- n 1)) 2)) )
                     (display res )
                     (display "\n" )
                     (+ 0 res)
                     )
                   )))



