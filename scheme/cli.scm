(define hg (lambda ()
             (let ( (p (open-tcp-stream-socket "localhost" 80) ) )
               (port/set-line-ending p 'NEWLINE)
               (display p)
               (write "GET" p)
               (display (read p))

               )
             ))
  
