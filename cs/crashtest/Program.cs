using System;

namespace osslcrashtest1
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Hello World!");
            Mrs3.mrs_initialize();
            
            var ctx = Mrs3.mrs_context_create(maxcliconn: 100);
            var con = Mrs3.mrs_connect(ctx, Mrs3.MrsConnectionType.TCP, addr: "127.0.0.1", port: 22223, timeout_msec: 5000);
            Mrs3.mrs_connection_enable_encryption(con);


        }
    }
}
