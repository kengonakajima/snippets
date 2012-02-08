using System.Text.RegularExpressions;

using System.Text;

using System;

namespace TestRegex

{

public class RegexBench2

{  

  private static String _doc = 

  "CGAATCTAAAAATAGATTCGGACGTGATGTAGTCGTACAAATGAAAAAGTAAGCC";

  private  static int ITERATIONS = 1000000;

  public static void Main()

  {

    long start = System.DateTime.Now.Ticks / 10000; 

    long end;

    int length = 1;

    for( int i = ITERATIONS; i <= ITERATIONS * 2; i++ )

        {

        length = (int) (Math.Log((double)i)/Math.Log(4)); 

   

        String matchthis = generateWord(i, length + 1);

        Regex regexpr = new Regex(matchthis, RegexOptions.Compiled);

        Boolean b = regexpr.IsMatch(_doc);

        if( b )

                {

                end = System.DateTime.Now.Ticks / 10000;        

                Console.WriteLine("found {0} at {1} it took {2} miliseconds", 

                matchthis, i, end - start );

                }

        }     

    end = System.DateTime.Now.Ticks / 10000;

    Console.WriteLine(".NET regex took {0} miliseconds",end - start);

 } 

public static String generateWord(int value, int length )

 {

  StringBuilder buf = new StringBuilder();

  int current = value;

  for(int i = 0; i < length; i++ )

   {

   int v = current % 4;

   current = current / 4;

   buf.Append( convert(v) );

   }

  return  buf.ToString();

 }

        

private static String convert(int value) 

 {

 switch(value)

  {

  case 0: return "A";

  case 1: return "G";

  case 2: return "T";

  case 3: return "C"; 

  default:

   return "0";

  }

 }

}

}
