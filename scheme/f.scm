(load-option 'format)

(define (type-clash-error procedure arg spec actual)
  (format
   #t
   "~%Procedure ~S~%requires its ~A~% argument ~
to be of type ~S,~%but it was called with ~
an arugment of type ~S.~%"
   procedure arg spec actual))

(type-clash-error 'vector-ref
                  "first"
                  'integer
                  'vector)

