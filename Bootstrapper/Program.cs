using System.Diagnostics;
using System.IO;
using System.IO.Compression;
using System.Net;

namespace Bootstrapper
{
    class Program
    {
        static string Host = "";
        static string TempZipURL = Host + "/files/temp.zip";
        static string BinFolder = Directory.GetCurrentDirectory() + "\\bin";
        static string ZipFileDest = BinFolder + "\\Temp.zip";
        static string Executable = BinFolder + "\\Revival.exe";

        static void Main(string[] args)
        {
            if (!Directory.Exists(BinFolder))
            {
                Directory.CreateDirectory(BinFolder);

                using (WebClient Client = new WebClient())
                {
                    Client.DownloadFile(TempZipURL, ZipFileDest);
                    ZipFile.ExtractToDirectory(ZipFileDest, BinFolder);
                }

                Process.Start(Executable);
            }
        }
    }
}
