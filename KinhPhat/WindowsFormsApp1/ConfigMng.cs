using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Json;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace WindowsFormsApp1
{
    public class CnnInfo
    {
        public string cnnStr;
    }

    [DataContract(Name = "config")]
    public class ConfigMng
    {
        [DataMember(Name = "cnnInfo")]
        public CnnInfo m_cnnInfo;

        static ConfigMng m_instance;
        static string m_cfgPath = @"..\..\..\config.xml";

        ConfigMng()
        {
            m_cnnInfo = new CnnInfo();
        }

        static XmlObjectSerializer createSerializer()
        {
            Type[] knownTypes = new Type[] {
                typeof(CnnInfo),
                };

            DataContractJsonSerializerSettings settings = new DataContractJsonSerializerSettings();
            settings.IgnoreExtensionDataObject = true;
            settings.EmitTypeInformation = EmitTypeInformation.AsNeeded;
            settings.KnownTypes = knownTypes;
            return new DataContractJsonSerializer(typeof(ConfigMng), settings);
        }

        public static ConfigMng getInstance()
        {
            string cfgPath = m_cfgPath;
            if (m_instance == null)
            {
                XmlObjectSerializer sz = createSerializer();
                if (File.Exists(cfgPath))
                {
                    XmlReader xrd = XmlReader.Create(cfgPath);
                    xrd.Read();
                    xrd.ReadToFollowing("config");
                    var obj = sz.ReadObject(xrd, false);
                    xrd.Close();
                    m_instance = (ConfigMng)obj;
                }
                else
                {
                    m_instance = new ConfigMng();
                }
            }
            return m_instance;
        }
        public void UpdateConfig()
        {
            var sz = createSerializer();
            XmlWriterSettings settings = new XmlWriterSettings();
            settings.Indent = true;
            settings.IndentChars = "\t";
            settings.Encoding = Encoding.Unicode;

            XmlWriter xwriter;
            xwriter = XmlWriter.Create(m_cfgPath, settings);
            xwriter.WriteStartElement("config");
            sz.WriteObjectContent(xwriter, this);
            xwriter.WriteEndElement();
            xwriter.Close();
        }
    }
}
