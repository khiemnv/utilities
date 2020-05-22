#define use_sqlite

using System;
using System.Collections.Generic;
using System.Data.OleDb;
using System.Data.SQLite;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace WindowsFormsApp1
{
    public class Searcher
    {
        string m_cnnStr;
        string m_searchDb;

        public Searcher(string cnnStr, string srchDb = "")
        {
            this.m_cnnStr = cnnStr;
            if (srchDb == "")
            {
                var reg = new Regex("Data Source=(.*);");
                var m = reg.Match(cnnStr);
                m_searchDb = m.Groups[1].Value.Replace(".accdb", "_search.db");
            }
        }

        public void BuildSearchDb()
        {
            var cnn = new OleDbConnection(m_cnnStr);
            cnn.Open();
            var cmd = new OleDbCommand();
            cmd.Connection = cnn;
            cmd.CommandText = "SELECT * FROM paragraphs";
            //cmd.CommandText = "SELECT * FROM paragraphs WHERE ID > (SELECT MAX(paragraphId) FROM words)";
            cmd.CommandText = "SELECT * FROM paragraphs WHERE ID > 42207";  //last build search data
            var rd = cmd.ExecuteReader();
            var reg = new Regex(@"[\w]+(-\w+)*");
            reg = new Regex(@"[\w]+");
            var dict = new HashSet<string>();
            var lst = new List<word>();
            var maxKeyLen = 0;
            myKey keygen = new myKey();
            while (rd.Read())
            {
                var titleId = Convert.ToUInt64(rd["titleId"]);
                var parId = Convert.ToUInt64(rd["ID"]);
                var parOrd = Convert.ToInt16(rd["ord"]);
                var mc = reg.Matches(Convert.ToString(rd["content"]));
                foreach (Match m in mc)
                {
                    bool chk;
                    var key = keygen.genKey2(m.Value, out chk);
                    if (!chk)
                    {
                        Debug.WriteLine("{0} {1} {2} {3} {4}", titleId, parId, parOrd, m.Index, m.Value);
                    }
                    if (key.Length > maxKeyLen) { maxKeyLen = key.Length; }
                    lst.Add(new word()
                    {
                        content = m.Value,
                        titleId = titleId,
                        parId = parId,
                        key = key,
                        pos = m.Index
                    });
                    dict.Add(key);
                }
            }
            rd.Close();
            cmd.Dispose();

            //insert to word tbl
            AddToSearchDb(dict, lst);

            cnn.Close();
            cnn.Dispose();
        }

        void insertToDb(OleDbConnection cnn, Dictionary<string, UInt64> keysD, List<word> wordsL)
        {
            //find keys
            var cmd1 = new OleDbCommand("select ID from keys where key = ?", cnn);
            cmd1.Parameters.Add(new OleDbParameter("@key", OleDbType.Char, 32));
            var keysL = new List<string>();
            foreach (var key in keysD.Keys)
            {
                cmd1.Parameters[0].Value = key;
                var res = cmd1.ExecuteScalar();
                if (res == null)
                {
                    keysL.Add(key);
                }
                else
                {
                    keysD[key] = Convert.ToUInt64(res);
                }
            }
            cmd1.Dispose();

            //add keys
            var trans = cnn.BeginTransaction();
            var cmd = new OleDbCommand("insert into keys ([key]) VALUES (?)", cnn, trans);
            cmd.Parameters.Add(new OleDbParameter("", OleDbType.Char, 32));
            var cmd2 = new OleDbCommand("Select @@Identity", cnn, trans);
            foreach (var key in keysL)
            {
                cmd.Parameters[0].Value = key;
                var eff = cmd.ExecuteNonQuery();
                keysD[key] = Convert.ToUInt64(cmd2.ExecuteScalar());
            }
            cmd.Dispose();
            cmd2.Dispose();

            var cmd3 = new OleDbCommand("insert into words ([keyId],[titleId],[paragraphId],[pos],[word]) " +
                "values (?,?,?,?,?)", cnn, trans);
            cmd3.Parameters.Add(new OleDbParameter("", OleDbType.BigInt));
            cmd3.Parameters.Add(new OleDbParameter("", OleDbType.BigInt));
            cmd3.Parameters.Add(new OleDbParameter("", OleDbType.BigInt));
            cmd3.Parameters.Add(new OleDbParameter("", OleDbType.Integer));
            cmd3.Parameters.Add(new OleDbParameter("", OleDbType.Char, 32));
            foreach (var w in wordsL)
            {
                cmd3.Parameters[0].Value = keysD[w.key];
                cmd3.Parameters[1].Value = w.titleId;
                cmd3.Parameters[2].Value = w.parId;
                cmd3.Parameters[3].Value = w.pos;
                cmd3.Parameters[4].Value = w.content;
                cmd3.ExecuteNonQuery();
            }
            cmd3.Dispose();
            trans.Commit();
            trans.Dispose();
        }

        string sqliteCnnStr { get
            {
                return string.Format(@"Data Source={0};version=3;",m_searchDb);
            } }

        void AddToSearchDb(HashSet<string> keysH, List<word> wordsL)
        {
            //find keys
            var cnn = new SQLiteConnection(sqliteCnnStr);
            cnn.Open();
            var cmd1 = new SQLiteCommand("select ID from keys where key = ?", cnn);
            cmd1.Parameters.Add(new SQLiteParameter(System.Data.DbType.String));
            var keysL = new List<string>();
            var keysD = new Dictionary<string, UInt64>();
            foreach (var key in keysH)
            {
                cmd1.Parameters[0].Value = key;
                var res = cmd1.ExecuteScalar();
                if (res == null)
                {
                    keysL.Add(key);
                }
                else
                {
                    keysD[key] = Convert.ToUInt64(res);
                }
            }
            cmd1.Dispose();

            //add keys
            var trans = cnn.BeginTransaction();
            var cmd = new SQLiteCommand("insert into keys (key) VALUES (?)", cnn, trans);
            cmd.Parameters.Add(new SQLiteParameter("", System.Data.DbType.String));
            var cmd2 = new SQLiteCommand("select last_insert_rowid()", cnn, trans);
            foreach (var key in keysL)
            {
                cmd.Parameters[0].Value = key;
                var eff = cmd.ExecuteNonQuery();
                keysD[key] = Convert.ToUInt64(cmd2.ExecuteScalar());
            }
            cmd.Dispose();
            cmd2.Dispose();

            var cmd3 = new SQLiteCommand("insert into words (keyId,titleId,paragraphId,pos,word) " +
                "values (?,?,?,?,?)", cnn, trans);
            cmd3.Parameters.Add(new SQLiteParameter("", System.Data.DbType.UInt64));
            cmd3.Parameters.Add(new SQLiteParameter("", System.Data.DbType.UInt64));
            cmd3.Parameters.Add(new SQLiteParameter("", System.Data.DbType.UInt64));
            cmd3.Parameters.Add(new SQLiteParameter("", System.Data.DbType.Int32));
            cmd3.Parameters.Add(new SQLiteParameter("", System.Data.DbType.String));
            foreach (var w in wordsL)
            {
                cmd3.Parameters[0].Value = keysD[w.key];
                cmd3.Parameters[1].Value = w.titleId;
                cmd3.Parameters[2].Value = w.parId;
                cmd3.Parameters[3].Value = w.pos;
                cmd3.Parameters[4].Value = w.content;
                cmd3.ExecuteNonQuery();
            }
            cmd3.Dispose();
            trans.Commit();
            trans.Dispose();
            cnn.Close();
            cnn.Dispose();
        }

        static Dictionary<string, string> dict = new Dictionary<string, string>() {
                {"ĐÐđ", "d"     },
                {"áàảãạÁÀẢÃẠ", "a" },
                {"éèẻẽẹÉÈẺẼẸ", "e" },
                {"íìĩỉịÍÌĨỈỊ", "i" },
                {"òóỏõọÒÓỎÕỌ", "o" },
                {"úùủũụÚÙỦŨỤ", "u" },
                {"ỳýỹỷỵỲÝỸỶỴ", "y" },
                {"ăằắẵẳặĂẰẮẴẲẶ", "a"},
                {"âầấẫẩậÂẦẤẪẨẬ", "a"},
                {"êềếễểệÊỀẾỄỂỆ", "e"},
                {"ồôỗốộổỒÔỖỐỘỔ", "o"},
                {"ơờớỡởợƠỜỚỠỞỢ", "o"},
                {"ừưữứửựỪƯỮỨỬỰ", "u"},
            };
        class myKey
        {
            char chMin, chMax;
            char[] tbl;
            Regex reg = new Regex("^[a-z0-9]+$", RegexOptions.IgnoreCase);

            public myKey()
            {
                tbl = makeTbl(out chMin, out chMax);
            }
            char[] makeTbl(out char outMin, out char outMax)
            {
                char min = 'á';
                char max = 'á';
                foreach (var p in dict)
                {
                    foreach (var ch in p.Key)
                    {
                        if (ch > max) { max = ch; }
                        if (ch < min) { min = ch; }
                    }
                }
                int d = max - min + 1;
                char[] tbl = new char[d];
                for (var ch = min; ch < max; ch++)
                {
                    tbl[ch - min] = ch;
                }
                foreach (var p in dict)
                {
                    foreach (var ch in p.Key)
                    {
                        tbl[ch - min] = p.Value[0];
                    }
                }
                outMin = min;
                outMax = max;
                return tbl;
            }
            public string genKey2(string value, out bool chk)
            {
                string ret = "";
                foreach (char ch in value)
                {
                    if (ch >= chMin && ch <= chMax)
                    {
                        ret += tbl[ch - chMin];
                    }
                    else
                    {
                        ret += ch;
                    }
                }
                chk = reg.IsMatch(ret);
                return ret.ToLower();
            }
        }

        public SrchRes[] Find(string txt)
        {
            int nRow;
            var arr = getWords(txt, out nRow);

            var ret = calcDiff(arr, nRow);
            return ret;
        }

        class MyTimer
        {
            int begin;
            string msg;
            public MyTimer(string msg)
            {
                begin = Environment.TickCount;
                this.msg = msg;
            }
            ~MyTimer()
            {
                Debug.WriteLine("{0} {1}", msg, Environment.TickCount - begin);
            }
        }

#if use_sqlite
        private word[][] getWords(string txt, out int nRow)
        {
            var t = new MyTimer("getWords");
            var cnn = new SQLiteConnection(sqliteCnnStr);
            cnn.Open();
            var cmd = new SQLiteCommand("select ID from keys where key = ?", cnn);
            cmd.Parameters.Add(new SQLiteParameter("", System.Data.DbType.String));

            var reg = new Regex(@"[\w]+");
            var mc = reg.Matches(txt);
            var tDict = new Dictionary<string, UInt64>();
            foreach (Match m in mc)
            {
                cmd.Parameters[0].Value = m.Value;
                var res = cmd.ExecuteScalar();
                if (res != null)
                {
                    tDict.Add(m.Value, Convert.ToUInt64(res));
                }
            }
            cmd.Dispose();

            var cmd2 = new SQLiteCommand("select titleId, paragraphId, pos, word from words where keyId = ?", cnn);
            cmd2.Parameters.Add(new SQLiteParameter("", System.Data.DbType.UInt64));
            var arr = new word[tDict.Count][];
            nRow = 0;
            foreach (var keyId in tDict.Values)
            {
                var lst = new List<word>();
                cmd2.Parameters[0].Value = keyId;
                var rd = cmd2.ExecuteReader();
                while (rd.Read())
                {
                    lst.Add(new word()
                    {
                        titleId = Convert.ToUInt64(rd[0]),
                        parId = Convert.ToUInt64(rd[1]),
                        pos = Convert.ToInt32(rd[2]),
                        content = Convert.ToString(rd[3])
                    });
                }
                arr[nRow++] = lst.ToArray();
                rd.Close();
            }

            cnn.Close();
            cnn.Dispose();
            return arr;
        }
#else
        private object[] getWords(string txt, out int nRow)
        {
            var cnn = new OleDbConnection(m_cnnStr);
            cnn.Open();
            var cmd = new OleDbCommand("select ID from keys where key = ?", cnn);
            cmd.Parameters.Add(new OleDbParameter("", OleDbType.Char, 32));

            var reg = new Regex(@"[\w]+");
            var mc = reg.Matches(txt);
            var tDict = new Dictionary<string, UInt64>();
            foreach (Match m in mc)
            {
                cmd.Parameters[0].Value = m.Value;
                var res = cmd.ExecuteScalar();
                if (res != null)
                {
                    tDict.Add(m.Value, Convert.ToUInt64(res));
                }
            }
            cmd.Dispose();

            var cmd2 = new OleDbCommand("select titleId, paragraphId, pos, word from words where keyId = ?", cnn);
            cmd2.Parameters.Add(new OleDbParameter("", OleDbType.BigInt));
            var arr = new object[tDict.Count];
            nRow = 0;
            foreach (var keyId in tDict.Values)
            {
                var lst = new List<word>();
                cmd2.Parameters[0].Value = keyId;
                var rd = cmd2.ExecuteReader();
                while (rd.Read())
                {
                    lst.Add(new word()
                    {
                        titleId = Convert.ToUInt64(rd[0]),
                        parId = Convert.ToUInt64(rd[1]),
                        pos = Convert.ToInt32(rd[2]),
                        content = Convert.ToString(rd[3])
                    });
                }
                arr[nRow++] = lst;
                rd.Close();
            }

            cnn.Close();
            cnn.Dispose();
            return arr;
        }
#endif

        SrchRes[] calcDiff(word[][] arr, int nRow)
        {
            var begin = Environment.TickCount;
            var res = new SrchRes[arr[0].Length];
            for (int i = 0; i < arr[0].Length; i++)
            {
                res[i] = new SrchRes() { path = new int[] { i },d = 0};
            }
            for (var row = 1; row < nRow; row++)
            {
                var lst = arr[row];
                var prevLst = arr[row - 1];
                var tmplRes = new List<int[]>();
                for (var j = 0; j < res.Length; j++)
                {
                    var prevD = res[j].d;
                    var prevI = res[j].path[row - 1];
                    var prevW = prevLst[prevI];
                    for (var k = 0; k < lst.Length; k++)
                    {
                        var curW = lst[k];
                        if (prevW.titleId == curW.titleId)
                        {
                            //var d = wordDiff(res[j].w, lst[k]);
                            var d = Convert.ToInt32(prevW.parId) - Convert.ToInt32(curW.parId);
                            d *= d < 0 ? -100 : 100;
                            d += Math.Abs(prevW.pos - curW.pos)*10;
                            d += prevD;
                            tmplRes.Add(new int[] { j, k, d });
                        }
                    }
                }
                
                var h = new myHeap<int[]>(tmplRes.ToArray(), (x, y) => x[2] - y[2]);
                var n = Math.Min(100, tmplRes.Count);
                var top100 = new SrchRes[100];
                for (int i = 0; i < n; i++)
                {
                    var t = h.PopMin();
                    top100[i] = new SrchRes() { d = t[2], path = new int[row + 1]};
                    res[t[0]].path.CopyTo(top100[i].path, 0);
                    top100[i].path[row] = t[1];
                }
                res = top100;
            }
            var elapsed = Environment.TickCount - begin;
            Debug.WriteLine("calc diff {0}", elapsed);
            foreach(var item in res)
            {
                var row = 0;
                item.detail = item.path.Select(v => arr[row++][v]).ToList();
            }
            return res;
        }

        int wordDiff(word w1, word w2)
        {
            var a1 = new int[] {
                Convert.ToInt32(w1.titleId),
                Convert.ToInt32(w1.parId),
                w1.pos };
            var a2 = new int[] {
                Convert.ToInt32(w2.titleId),
                Convert.ToInt32(w2.parId),
                w2.pos };
            var c = new int[] { 1000, 100, 10 };
            int diff = 0;
            for (int i = 0; i < 3; i++)
            {
                diff += Math.Abs(a1[i] - a2[i]) * c[i];
            }
            return diff;
        }
    }

    public class SrchRes
    {
        public int[] path;
        public int d;
        public List<word> detail;
    }
    public class word
    {
        public string content;
        public UInt64 titleId;
        public UInt64 parId;
        public int pos;
        public string key;
    }
    public class myHeap<T>
    {
        T[] h;
        Func<T, T, int> comp;
        int len;
        public myHeap(T[] arr, Func<T, T, int> comp)
        {
            h = arr;
            this.comp = comp;
            len = arr.Length;
        }

        void BuildMinHeap()
        {
            for (int i = len / 2; i >= 0; i--)
            {
                MinHeap(i);
            }
        }
        void MinHeap(int i)
        {
            int m = (i + 1) * 2;
            if (m < len)
            {
                if (comp(h[m - 1], h[m]) < 0)
                {
                    m--;
                }
            }
            else if (m == len)
            {
                m--;
            }
            else
            {
                //do nothing
                return;
            }

            if (comp(h[m], h[i]) < 0)
            {
                swap(i, m);
                MinHeap(m);
            }
        }
        void swap(int i, int m)
        {
            T tmp = h[m];
            h[m] = h[i];
            h[i] = tmp;
        }
        public T PopMin()
        {
            BuildMinHeap();
            T tmp = h[0];
            len--;
            swap(0, len);
            return tmp;
        }
    }


    public class SearchPanel
    {
        string cnnStr;
        public TableLayoutPanel m_tblLayout;
        ListView m_lstV;
        StatusBar m_sts;
        public SearchPanel(string cnnStr)
        {
            this.cnnStr = cnnStr;

            m_tblLayout = new TableLayoutPanel();
            m_tblLayout.Dock = DockStyle.Fill;

            var edt = new TextBox();
            edt.Anchor = AnchorStyles.Left | AnchorStyles.Right;

            var btn = new Button();
            btn.Text = "Search";
            btn.AutoSize = true;
            btn.Anchor = AnchorStyles.Right;
            btn.Click += (s, e) =>
            {
                OnSearch(edt.Text);
            };

            var lst = new ListView();
            lst.View = View.Details;
            lst.FullRowSelect = true;
            lst.GridLines = true;
            lst.Dock = DockStyle.Fill;
            lst.ItemSelectionChanged += (s, e) =>
              {
                  UInt64 titleId = 0;
                  foreach (ListViewItem li in lst.SelectedItems) {
                      UInt64.TryParse(li.SubItems[1].Text, out titleId);
                      //OnSelectTitle(titleId);
                      OnSelectTitle?.Invoke(s, titleId);
                      break;
                  }
              };
            m_lstV = lst;

            var sts = new StatusBar();
            sts.Dock = DockStyle.Bottom;
            sts.ShowPanels = false;
            m_sts = sts;

            int iRow = 0;
            m_tblLayout.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            m_tblLayout.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            m_tblLayout.RowStyles.Add(new RowStyle(SizeType.Percent,100));
            m_tblLayout.RowStyles.Add(new RowStyle(SizeType.AutoSize));
            m_tblLayout.CellBorderStyle = TableLayoutPanelCellBorderStyle.Single;
            m_tblLayout.Controls.Add(edt, 0, iRow++);
            m_tblLayout.Controls.Add(btn, 0, iRow++);
            m_tblLayout.Controls.Add(lst, 0, iRow++);
            m_tblLayout.Controls.Add(sts, 0, iRow++);
        }

        public event EventHandler<UInt64> OnSelectTitle;
        private void SelectTitle(int titleId)
        {
            
        }

        private void OnSearch(string txt)
        {
            if (txt == "") return;

            var begin = Environment.TickCount;
            var srch = new Searcher(cnnStr);
            //srch.BuildSearchDb();
            //return;
            var res = srch.Find(txt);
            showSearchRes(res);
            m_sts.Text = string.Format("elapsed time: {0}(ms)", Environment.TickCount-begin);
        }

        void showSearchRes(SrchRes[] res)
        {
            var listView1 = m_lstV;
            listView1.Clear();
            listView1.Columns.Add("content");
            listView1.Columns.Add("title");
            listView1.Columns.Add("paragraph");
            listView1.Columns.Add("pos");
            listView1.Columns.Add("diff");
            listView1.GridLines = true;
            foreach (var rec in res)
            {
                //var tempTxt = string.Format("{0} {1} {2} {3}", 
                //    string.Join(" ", rec.path.Select((v)=>v.content).ToArray()),
                //    rec.d, rec.w.titleId, rec.w.parId);
                var li = listView1.Items.Add(string.Join(" ", rec.detail.Select((v) => v.content)));
                li.SubItems.Add(rec.detail[0].titleId.ToString());
                li.SubItems.Add(string.Join(" ", rec.detail.Select(v=>v.parId)));
                li.SubItems.Add(string.Join(" ", rec.detail.Select(v => v.pos)));
                var sub = li.SubItems.Add(rec.d.ToString());
                
            }
        }
    }
}
