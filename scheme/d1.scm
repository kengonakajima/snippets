(define a-stream
  (letrec ((next
            (lambda (n)
              (cons n (delay (next (+ n 1 )))))))
    (next 0)))

(head (tail (tail a-stream)))



