(load-option 'regular-expression)


(display
 (substring string
            (re-match-start-index 0 (re-string-match ".*" "asdf" ))))



