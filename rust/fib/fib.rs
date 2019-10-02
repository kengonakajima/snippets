// フィボナッチ数 by rust
use std::env;

fn fib(n: i64) -> i64 {
    if n < 2 {
            n
                } else {
                        fib(n - 1)  + fib(n - 2)
                            }
                            }

                            fn main() {
                                if let Some(s) = env::args().nth(1) {
                                        if let Ok(n) = s.parse::<i64>() {
                                                    println!("{}", fib(n))
                                                            }
                                                                }
                                                                }

                                                                /*
                                                                 example
                                                                     compile:
                                                                             rustc -C opt-level=3 fib_rust.rs
                                                                                 run:
                                                                                         ./fib_rust 39
                                                                                         */