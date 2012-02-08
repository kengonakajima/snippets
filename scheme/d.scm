(define *paths* ())
(define failsym '@)

(define (choose choices)
  (if (null? choices)
      (fail)
      (call-with-current-continuation
       (lambda (cc)
         (set! *paths*
               (cons (lambda ()
                       (cc (choose (cdr choices))))
                     *paths*))
         (car choices)))))
(define fail)

(call-with-current-continuation
 (lambda (cc)
   (set! fail
         (lambda ()
           (if (null? *paths*)
               (cc failsym)
               (let ((p1 (car *paths*)))
                 (set! *paths* (cdr *paths*))
                 (p1)))))))

(define (tn)
  (list (choose '(0 1 2 3 4 5))
        (choose '(0 1 2 3 4 5))))

(define (plt sum)
  (let ((nums (tn)))
    (if (= (apply + nums) sum)
        `(the sum of ,@nums)
        (fail))))
