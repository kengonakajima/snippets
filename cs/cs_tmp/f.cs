class F
{
    static void Main() {
        string[] lines = { "First line", "Second line", "Third line" };
        using (System.IO.StreamWriter file = new System.IO.StreamWriter(@"hoge.txt")) {
            for(int i=0;i<1000000;i++) {
                file.WriteLine("hoge:"+i);
                file.Flush();
            }
        }
    }
}
