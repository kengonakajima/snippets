(define checkbook (lambda ()
                    (letrec
                        ((IB "enter initial balance: " )
                         (AT "enter transaction (- for wthdrawl): ")
                         (FB "Your final balance: ")
                         (prompt-read (lambda (Prompt)
                                        (display Prompt)
                                        (read)))
                         (newbal (lambda (Init t)
                                   (if (= t 0)
                                       (list FB Init)
                                       (transaction (+ Init t)))))
                         (transaction (lambda (Init)
                                        (newbal Init (prompt-read AT)))))
                      (transaction (prompt-read IB))
                      )))


