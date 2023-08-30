using System;

namespace ProfilingTest
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Start of the program.");

            TestFunction();

            Console.WriteLine("End of the program.");
        }

        static void TestFunction()
        {
            Console.WriteLine("Inside the TestFunction.");
        }
    }
}
