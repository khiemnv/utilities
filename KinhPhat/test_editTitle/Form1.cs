using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Json;
using System.Data.OleDb;
using WindowsFormsApp1;

namespace test_editTitle
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            TitleEdt edt = new TitleEdt(true);
            edt.m_edtPanel.m_title = new MyTitle()
            {
                ID = 5,
            };

            var cnnStr = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=<db>;";
            cnnStr = cnnStr.Replace("<db>", @"D:\tmp\github\utilities\KinhPhat\kinhtang.accdb");
            ConfigMng.getInstance().m_content.initCnn(cnnStr);
            //edt.m_edtPanel.m_cnnStr = cnnStr;
            //edt.m_edtPanel.initCnn();
            edt.m_edtPanel.loadTitle();
            edt.m_edtPanel.updateCmb();
            edt.ShowDialog();
        }
    }


    //[DataContract(Name = "MyTitle")]
    //public class MyTitle
    //{
    //    [DataMember(Name = "ID", EmitDefaultValue = false)]
    //    public UInt64 ID;
    //    [DataMember(Name = "title", EmitDefaultValue = false)]
    //    public string zTitle;
    //    public UInt64 pathId;
    //    [DataMember(Name = "path", EmitDefaultValue = false)]
    //    public string zPath;
    //    [DataMember(Name = "paragraphs", EmitDefaultValue = false)]
    //    public List<MyParagraph> paragraphLst;
    //    public int ord;
    //}
    //[DataContract(Name = "MyParagraph")]
    //public class MyParagraph
    //{
    //    public UInt64 ID;
    //    public String zTitle;
    //    public UInt64 titleId;
    //    [DataMember(Name = "order", EmitDefaultValue = false)]
    //    public int order;
    //    [DataMember(Name = "alignment", EmitDefaultValue = false)]
    //    public int alignment;
    //    [DataMember(Name = "leftIndent", EmitDefaultValue = false)]
    //    public int leftIndent;
    //    [DataMember(Name = "fontSize", EmitDefaultValue = false)]
    //    public int fontSize;
    //    [DataMember(Name = "fontBold", EmitDefaultValue = false)]
    //    public int fontBold;
    //    [DataMember(Name = "fontItalic", EmitDefaultValue = false)]
    //    public int fontItalic;
    //    [DataMember(Name = "content", EmitDefaultValue = false)]
    //    public string content;
    //}


}
