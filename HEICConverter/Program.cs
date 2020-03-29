using ImageMagick;
using System;
using System.IO;

namespace HEICConverter
{
    class Program
    {
        public static void ProcessFile(string path)
        {
            using (MagickImage image = new MagickImage(path))
            {
                string newFile = path.Replace(Path.GetExtension(path), ".jpg");
                image.Write(newFile);
            }
            Console.WriteLine("Processed file '{0}'.", path);
        }
        static void Main(string[] args)
        {
            foreach (string path in args)
            {
                if (File.Exists(path))
                {
                    ProcessFile(path);
                }
            }
        }
    }

}
