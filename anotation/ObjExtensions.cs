using System;
using System.Drawing.Imaging;
using System.Drawing;
using System.IO;
using Newtonsoft.Json;
using Newtonsoft.Json.Serialization;

namespace annotation
{
    public static class ObjExtensions
    {
        static readonly JsonSerializerSettings jsonSerializerSettings = new JsonSerializerSettings
        {
            ContractResolver = new CamelCasePropertyNamesContractResolver(),
            //Converters = { new StringEnumConverter() },
            NullValueHandling = NullValueHandling.Ignore,
        };
        public static string ToJson(this Object obj)
        {
            return JsonConvert.SerializeObject(obj, jsonSerializerSettings);
        }
        public static T FromJson<T>(this string json)
        {
            return JsonConvert.DeserializeObject<T>(json, jsonSerializerSettings);
        }

        public static bool SaveAsJpeg(this Bitmap img, string path)
        {
            ImageCodecInfo jpgEncoder = GetEncoder(ImageFormat.Jpeg);
            EncoderParameters myEncoderParameters = new EncoderParameters(1);
            System.Drawing.Imaging.Encoder myEncoder = System.Drawing.Imaging.Encoder.Quality;
            EncoderParameter myEncoderParameter = new EncoderParameter(myEncoder, 90L);
            myEncoderParameters.Param[0] = myEncoderParameter;
            img.Save(path, jpgEncoder, myEncoderParameters);
            return true;
        }
        private static ImageCodecInfo GetEncoder(ImageFormat format)
        {
            ImageCodecInfo[] codecs = ImageCodecInfo.GetImageEncoders();
            foreach (ImageCodecInfo codec in codecs)
            {
                if (codec.FormatID == format.Guid)
                {
                    return codec;
                }
            }
            return null;
        }

        public static Bitmap Base64StringToBitmap(this string base64String)
        {
            byte[] byteBuffer = Convert.FromBase64String(base64String);
            MemoryStream memoryStream = new MemoryStream(byteBuffer)
            {
                Position = 0
            };

            //Bitmap bmpReturn = (Bitmap)Image.FromStream(memoryStream);
            Bitmap bmpReturn = new Bitmap(memoryStream);
            //memoryStream.Close();
            return bmpReturn;
        }
    }
    
}
