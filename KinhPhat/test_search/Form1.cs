using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Data.OleDb;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Text.RegularExpressions;
using System.Diagnostics;
using WindowsFormsApp1;

namespace test_search
{
    public partial class Form1 : Form
    {
        SearchPanel m_srchPanel;
        public Form1()
        {
            InitializeComponent();
            m_srchPanel = new SearchPanel(cnnStr, false);
            m_srchPanel.OnSelectTitle += (s, e) =>
            {
                Debug.WriteLine("titleId {0}", e);
            };
            this.Controls.Add(m_srchPanel.m_tblLayout);
            this.AcceptButton = m_srchPanel.m_acceptBtn;
        }

        string cnnStr
        {
            get
            {
                string srcDb = @"..\..\..\truongbo.accdb";
                var cnnStr = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=<db>;";
                return cnnStr.Replace("<db>", srcDb);
            }
        }

        void testHeap()
        {
            int[] arr = new int[10];
            var rnd = new Random(101);
            for (int i = 0; i < 10; i++)
            {
                arr[i] = rnd.Next(1, 100);
            }
            Func<int, int, int> cmp = (x, y) => x - y;
            var h = new MyHeap<int>(arr, cmp);
            for (int i = 0; i < 10; i++)
            {
                var min = h.PopMin();
            }
        }
    }

}
