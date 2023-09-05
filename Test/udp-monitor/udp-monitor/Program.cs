using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using Newtonsoft.Json.Linq;

class Program
{
    private static Dictionary<string, object> datas = new Dictionary<string, object>();
    private static UdpClient server = new UdpClient(2222);
    private static string[] globalArgs;

    static void Main(string[] args)
    {
        globalArgs = args;

        try
        {
            server.BeginReceive(new AsyncCallback(ReceiveCallback), null);

            Console.WriteLine($"Server is listening at port {2222}");
            Console.WriteLine($"Server ip : {GetLocalIPAddress()}");
            Console.WriteLine($"Server is IP4/IP6 : IPv4");

            while (true)
            {
                Console.Write("Enter TraceId: ");
                string message = Console.ReadLine();

                foreach (var entry in datas)
                {
                    if (((JObject)entry.Value)["TraceId"].ToString() == message)
                    {
                        Console.WriteLine(entry.Value);
                    }
                }
                Console.WriteLine("************************************************************************");
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine("Error: " + ex.Message);
        }
    }

    private static void ReceiveCallback(IAsyncResult ar)
    {
        IPEndPoint ep = new IPEndPoint(IPAddress.Any, 2222);
        byte[] data = server.EndReceive(ar, ref ep);
        string message = Encoding.UTF8.GetString(data);

        if (globalArgs.Length > 0 && globalArgs[0] == "nd")
        {
            PrintNonDuplicateMessages(message);
        }
        else if (globalArgs.Length > 0 && globalArgs[0] == "dt")
        {
            PrintDuplicateTraceIds(message);
        }
        else
        {
            Console.WriteLine(message);
        }

        server.BeginReceive(new AsyncCallback(ReceiveCallback), null);
    }

    private static void PrintNonDuplicateMessages(string message)
    {
        if (!datas.ContainsKey(message))
        {
            Console.WriteLine(message);
            datas[message] = "";
        }
    }

    private static void PrintDuplicateTraceIds(string message)
    {
        try
        {
            JObject json = JObject.Parse(message);
            string traceId = json["TraceId"].ToString();

            if (datas.ContainsKey(traceId))
            {
                Console.WriteLine("Duplicate TraceId: " + traceId);
            }
            else
            {
                datas[traceId] = json;
            }
        }
        catch
        {
            // Handle error if needed
        }
    }

    public static string GetLocalIPAddress()
    {
        var host = Dns.GetHostEntry(Dns.GetHostName());
        foreach (var ip in host.AddressList)
        {
            if (ip.AddressFamily == AddressFamily.InterNetwork)
            {
                return ip.ToString();
            }
        }
        throw new Exception("No network adapters with an IPv4 address in the system!");
    }
}
