using System;
using System.IO;
using Newtonsoft.Json;

namespace annotation
{
    public partial class BaseConfig
    {
        public string BaseUrl { get; set; } = "";
        public string AccessToken { get; set; } = "";
        public bool AsstModeless { get; set; } = false;
        public class CleanRule
        {
            public int FromNow { get; set; } = 30;
        };
        public CleanRule cleanRule = new CleanRule();
        public class DetectionSetting
        {
            public double Accuracy { get; set; } = 0.85;
            public double Delta { get; set; } = 100;
        };
        public DetectionSetting detectionSetting = new DetectionSetting();
        public bool ObjectViewMode { get; set; } = false;
        public int DelayCapture { get; set; } = 0;

        public string WorkingDir { get; set; } = "";
        public bool AutoLogin { get; set; } = false;
        public string ModelDetect { get; set; } = "";
        public BaseConfig() { }

        public static BaseConfig GetBaseConfig()
        {
            string root = Directory.GetCurrentDirectory();
            string configPath = Path.Combine(root, "appconfig.json");
            string jsonContent = System.IO.File.ReadAllText(configPath);
            BaseConfig baseConfig = jsonContent.FromJson<BaseConfig>();
            return baseConfig;
        }
        public static void WriteBaseConfig(BaseConfig baseConfig)
        {
            // convert baseConfig to json string
            string configJson = JsonConvert.SerializeObject(baseConfig, Newtonsoft.Json.Formatting.Indented);
            string rootPath = Directory.GetCurrentDirectory();
            string configFilePath = Path.Combine(rootPath, "appconfig.json");

            // write data to file 
            File.WriteAllText(configFilePath, configJson);
        }
        public static void WriteBaseConfig(string key, object value)
        {
            try
            {
                var baseConfig = GetBaseConfig();
                baseConfig.GetType().GetProperty(key).SetValue(baseConfig, value, null);
                string configJson = JsonConvert.SerializeObject(baseConfig, Newtonsoft.Json.Formatting.Indented);
                string rootPath = Directory.GetCurrentDirectory();
                string configFilePath = Path.Combine(rootPath, "appconfig.json");

                // write data to file 
                File.WriteAllText(configFilePath, configJson);
            }
            catch (Exception ex)
            {
                Logger.Error(ex.Message);
            }
        }
    }
}
