﻿using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Json;
using System.Text;
using System.Threading.Tasks;
using System.Data;
using System.Data.OleDb;

namespace ConsoleApp1
{
    class Program
    {
        static void Main(string[] args)
        {
            //connect to db
            var path = Environment.CurrentDirectory;
            string zDb = "kinhtang.accdb";
            while (path != null) {
                path = Path.GetDirectoryName(path);
                if (File.Exists(path + "\\" + zDb)) { break; }
            }
            zDb = path + "\\" + zDb;

            var cnnStr = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=<db>;";
            cnnStr = cnnStr.Replace("<db>", zDb);
            System.Data.OleDb.OleDbConnection conn = new System.Data.OleDb.OleDbConnection(cnnStr);
            conn.Open();
            //ADODB.Connection cnn = new ADODB.Connection();
            //cnn.Open(cnnStr);

            List<MyTitle> titleLst = getTitles(conn);
            foreach(var title in titleLst) { 
                title.paragraphLst = getTitleParagraphs(conn, title.ID);
            }

            conn.Close();
            //cnn.Close();

            //generate json text
            var x = createSerializer(titleLst.GetType());
            var mem = new MemoryStream();
            x.WriteObject(mem, titleLst);
            StreamReader sr = new StreamReader(mem);
            mem.Position = 0;
            string myStr = sr.ReadToEnd();

            string txt = File.ReadAllText(path + "\\" + "templ2.html");
            myStr = myStr.Replace("\\r", "");
            myStr = myStr.Replace("\\/", "/");
            txt = txt.Replace("jsTxt = 'jsdata'", "jsTxt = '"+myStr+"'");
            string titleFile = path + "\\" + "titles.html";
            var fout = File.CreateText(titleFile);
            fout.Write(txt);
            fout.Close();
        }

        static List<MyTitle> getTitles(OleDbConnection cnn)
        {
            var titleLst = new List<MyTitle>();
            //get title
            var qry = "select * from titles INNER JOIN paths ON titles.pathId = paths.ID";
            var cmd = new OleDbCommand(qry, cnn);
            var reader = cmd.ExecuteReader();
            while (reader.Read())
            {
                var title = new MyTitle();
                title.ID = Convert.ToUInt64(reader["titles.ID"]);
                title.zTitle = Convert.ToString(reader["title"]);
                title.pathId = Convert.ToUInt64(reader["pathId"]);
                title.zPath = Convert.ToString(reader["path"]);
                title.zPath += "/" + title.zTitle;
                titleLst.Add(title);
            }
            reader.Close();
            return titleLst;
        }
        static List<MyParagraph> getTitleParagraphs(ADODB.Connection cnn ,UInt64 titleId)
        {
            var paragraphLst = new List<MyParagraph>();
            //get title
            var qry = "select * from paragraphs where titleId = ? ";
            var rst = new ADODB.Recordset();
            rst.Open(qry.Replace("?", titleId.ToString()),
                cnn, ADODB.CursorTypeEnum.adOpenKeyset,
                ADODB.LockTypeEnum.adLockOptimistic,
                (int)ADODB.CommandTypeEnum.adCmdText);
            rst.MoveFirst();
            while (!rst.EOF)
            {
                var par = new MyParagraph();
                par.order = Convert.ToInt32(rst.Fields["order"].Value);
                par.alignment = Convert.ToInt32(rst.Fields["alignment"].Value);
                par.leftIndent = Convert.ToInt32(rst.Fields["leftIndent"].Value);
                par.fontSize = Convert.ToInt32(rst.Fields["fontSize"].Value);
                par.fontBold = Convert.ToInt32(rst.Fields["fontBold"].Value);
                par.fontItalic = Convert.ToInt32(rst.Fields["fontItalic"].Value);
                par.content = Convert.ToString(rst.Fields["content"].Value);
                paragraphLst.Add(par);

                rst.MoveNext();
            }
            rst.Close();
            return paragraphLst;
        }

        static List<MyParagraph> getTitleParagraphs(OleDbConnection cnn, UInt64 titleId)
        {
            var paragraphLst = new List<MyParagraph>();
            //get title
            var qry = "select * from paragraphs where titleId = @id ";
            var cmd = new OleDbCommand(qry,cnn);
            cmd.Parameters.AddWithValue("@id", titleId);
            var reader = cmd.ExecuteReader();
            while (reader.Read())
            {
                var par = new MyParagraph();
                par.titleId = Convert.ToUInt64(reader["titleId"]);
                par.order = Convert.ToInt32(reader["order"]);
                par.alignment = Convert.ToInt32(reader["alignment"]);
                par.leftIndent = Convert.ToInt32(reader["leftIndent"]);
                par.fontSize = Convert.ToInt32(reader["fontSize"]);
                par.fontBold = Convert.ToInt32(reader["fontBold"]);
                par.fontItalic = Convert.ToInt32(reader["fontItalic"]);
                par.content = Convert.ToString(reader["content"]);
                paragraphLst.Add(par);
            }
            reader.Close();
            return paragraphLst;
        }

        [DataContract(Name = "MyTitle")]
        class MyTitle
        {
            [DataMember(Name = "ID", EmitDefaultValue = false)]
            public UInt64 ID;
            [DataMember(Name = "title", EmitDefaultValue = false)]
            public string zTitle;
            public UInt64 pathId;
            [DataMember(Name = "path", EmitDefaultValue = false)]
            public string zPath;
            public List<MyParagraph> paragraphLst;
        }
        [DataContract(Name = "MyParagraph")]
        class MyParagraph
        {
            public UInt64 ID;
            public String zTitle;
            public UInt64 titleId;
            public int order;
            public int alignment;
            public int leftIndent;
            public int fontSize;
            public int fontBold;
            public int fontItalic;
            [DataMember(Name = "content", EmitDefaultValue = false)]
            public string content;
        }
        static XmlObjectSerializer createSerializer(Type type)
        {
            Type[] knownTypes = new Type[] {
                    typeof(MyTitle),
                    typeof(MyParagraph),
                };

            DataContractJsonSerializerSettings settings = new DataContractJsonSerializerSettings
            {
                IgnoreExtensionDataObject = true,
                EmitTypeInformation = EmitTypeInformation.AsNeeded,
                KnownTypes = knownTypes
            };
            return new DataContractJsonSerializer(type, settings);
        }
    }
}   
