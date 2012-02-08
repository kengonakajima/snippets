import java.text.*;

import java.util.regex.*;

public class RegexBench2

{ 

static String _doc = "CGAATCTAAAAATAGATTCGGACGTGATGTAGTCGTACAAATGAAAAAGTAAGCC";

static int ITERATIONS = 1000000;

public static void main(String args[]) {

    long start = System.currentTimeMillis();

    int length = 1;

    for( int i = ITERATIONS; i <= ITERATIONS * 2; i++ ) {

      length = (int) (Math.log((double)i)/Math.log(4)); 

      String matchthis = generateWord(i, length + 1);

      Pattern regexpr = Pattern.compile(matchthis); Matcher matcher = regexpr.matcher(_doc);

      boolean b = matcher.find(); 

      if(b){

          long end = System.currentTimeMillis();

          System.out.println( MessageFormat.format("found {0} at {1} it took {2} miliseconds", 

              new Object[] {matchthis, "" + i, "" + (end-start) } )); }

         } 

      long end = System.currentTimeMillis();

      System.out.println("Java regex took " + (end - start) + " miliseconds");

      } 

     static String generateWord(int value, int length ) {

         StringBuffer buf = new StringBuffer(); int current = value;

         for(int i = 0; i < length; i++ ) {

            int v = current % 4; current = current / 4; buf.append( convert(v) );

            }

         return buf.toString();

        }

       static String convert(int value) {

           switch(value) {

               case 0: return "A"; case 1: return "G"; case 2: return "T"; case 3: return "C";  default: return "0"; } 

      }

}
