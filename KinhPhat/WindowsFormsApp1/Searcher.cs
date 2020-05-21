using System;
using System.Collections.Generic;
using System.Data.OleDb;
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
        string cnnStr;

        public Searcher(string cnnStr)
        {
            this.cnnStr = cnnStr;
        }

        public void BuildWordDb()
        {
            var cnn = new OleDbConnection(cnnStr);
            cnn.Open();
            var cmd = new OleDbCommand();
            cmd.Connection = cnn;
            cmd.CommandText = "SELECT * FROM paragraphs";
            cmd.CommandText = "SELECT * FROM paragraphs WHERE ID > (SELECT MAX(paragraphId) FROM words)";
            var rd = cmd.ExecuteReader();
            var reg = new Regex(@"[\w]+(-\w+)*");
            reg = new Regex(@"[\w]+");
            var dict = new Dictionary<string, UInt64>();
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
                    if (!dict.ContainsKey(key)) { dict.Add(key, 0); }
                }
            }
            rd.Close();
            cmd.Dispose();

            //insert to word tbl
            insertToDb(cnn, dict, lst);

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

            //var tDict = new Dictionary<string, UInt64>();
            //var cmd2 = new OleDbCommand("Select * from keys",cnn);
            //var rd = cmd2.ExecuteReader(CommandBehavior.SequentialAccess);
            //while (rd.Read())
            //{
            //    tDict.Add(Convert.ToString(rd[1]), Convert.ToUInt64(rd[0]));
            //}
            //rd.Close();
            //cmd2.Dispose();

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
            var cnn = new OleDbConnection(cnnStr);
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
            var nRow = 0;
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

            var ret = calcDiff(arr, nRow);
            return ret;
        }
        SrchRes[] calcDiff(object[] arr, int nRow)
        {
            var begin = Environment.TickCount;
            var res =
            ((List<word>)(arr[0]))
            .Select((v) => new SrchRes() { path = new List<word> { v }, w = v, d = 0 })
            .ToArray();
            for (var row = 1; row < nRow; row++)
            {
                var lst = (List<word>)arr[row];
                var tmplRes = new List<int[]>();
                for (var j = 0; j < res.Length; j++)
                {
                    for (var k = 0; k < lst.Count; k++)
                    {
                        if (res[j].w.titleId == lst[k].titleId)
                        {
                            var d = wordDiff(res[j].w, lst[k]);
                            tmplRes.Add(new int[] { j, k, d });
                        }
                        //var d = wordDiff(res[j].w, lst[k]);
                        //tmplRes[(j * lst.Count) + k] = ( new int[] { j, k, d });
                    }
                }
                //tmplRes.Sort((v1, v2) => { return v1[2] - v2[2]; });
                //res = tmplRes.Take(100).Select((v) => new { path = res[v[0]].path + " " + lst[v[1]].content, w = lst[v[1]], d = res[v[0]].d + v[2] }).ToArray();
                var h = new myHeap<int[]>(tmplRes.ToArray(), (x, y) => x[2] - y[2]);
                var n = Math.Min(100, tmplRes.Count);
                var top100 = new List<int[]>();
                for (int i = 0; i < n; i++)
                {
                    top100.Add(h.PopMin());
                }
                res = top100.Select((v) =>
                {
                    var obj = new SrchRes()
                    {
                        path = new List<word>(),
                        w = lst[v[1]],
                        d = res[v[0]].d + v[2]
                    };
                    obj.path.AddRange(res[v[0]].path);
                    obj.path.Add(lst[v[1]]);
                    return obj;
                }).ToArray();
            }
            var elapsed = Environment.TickCount - begin;
            Debug.WriteLine("calc diff {0}", elapsed);
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
        public List<word> path;
        public word w;
        public int d;
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

            int iRow = 0;
            m_tblLayout.Controls.Add(edt, 0, iRow++);
            m_tblLayout.Controls.Add(btn, 0, iRow++);
            m_tblLayout.Controls.Add(lst, 0, iRow++);
        }

        public event EventHandler<UInt64> OnSelectTitle;
        private void SelectTitle(int titleId)
        {
            
        }

        private void OnSearch(string txt)
        {
            var srch = new Searcher(cnnStr);
            var res = srch.Find(txt);
            showSearchRes(res);
        }

        void showSearchRes(SrchRes[] res)
        {
            var listView1 = m_lstV;
            listView1.Clear();
            listView1.Columns.Add("content");
            listView1.Columns.Add("title");
            listView1.Columns.Add("paragraph");
            listView1.Columns.Add("diff");
            listView1.GridLines = true;
            foreach (var rec in res)
            {
                //var tempTxt = string.Format("{0} {1} {2} {3}", 
                //    string.Join(" ", rec.path.Select((v)=>v.content).ToArray()),
                //    rec.d, rec.w.titleId, rec.w.parId);
                var li = listView1.Items.Add(string.Join(" ", rec.path.Select((v) => v.content).ToArray()));
                li.SubItems.Add(rec.w.titleId.ToString());
                li.SubItems.Add(rec.w.parId.ToString());
                li.SubItems.Add(rec.d.ToString());
            }
        }
    }
}
