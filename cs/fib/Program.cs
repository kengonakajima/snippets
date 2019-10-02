using System;

namespace fib
{
    class Program
    {

    static long Fib(int n)
            {
                if (n < 2)
                    return n;
                else
                    return Fib(n - 2) + Fib(n - 1);
            }

    static void Main(string[] args)
            {
                int n = int.Parse(args[0]);
                Console.Write(Fib(n));
            }
    }
}

