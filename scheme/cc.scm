(define r)

(call-with-current-continuation
 (lambda (cc)
   (begin
     (display "AA" )
     (set! r cc)
     (cc "CCC" )
     (display "BB" )
     )))

(display "EEE")

(display (r "DDD" ))






