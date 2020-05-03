//#define use_gecko
#define use_chromium

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Data.OleDb;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Json;
using System.Text.RegularExpressions;

namespace WindowsFormsApp1
{
    public partial class Form1 : Form
    {
        protected TreeView m_tree;
        Dictionary<string, Node> m_nodeDict;
        BackgroundWorker m_bw;
        SplitContainer m_sc;
        RichTextBox m_rtb;
#if use_gecko
        protected Gecko.GeckoWebBrowser m_wb;
#elif use_chromium
        protected CefSharp.WinForms.ChromiumWebBrowser m_wb;
#else
        protected WebBrowser m_wb;
#endif
        string m_db;
        string m_cnnStr;
        public Form1()
        {
            InitializeComponent();

            this.Menu = new MainMenu();
            var file = Menu.MenuItems.Add("&File");
            var open = file.MenuItems.Add("&Open");
            open.Click += Open_Click;
            var preview = file.MenuItems.Add("&Preview");
            preview.Click += Preview_Click;
            var export = file.MenuItems.Add("&Export");
            export.Click += Export_Click;

            m_sc = new SplitContainer();
            m_sc.Dock = DockStyle.Fill;
            Controls.Add(m_sc);

            //tree
            m_nodeDict = new Dictionary<string, Node>();

            m_tree = new TreeView();
            m_sc.Panel1.Controls.Add(m_tree);

            m_treeStyle = TreeStyle.check;
            initTree(); //set state imagine

            m_tree.Dock = DockStyle.Fill;
            m_tree.NodeMouseClick += Tree_NodeMouseClick;
            m_tree.NodeMouseDoubleClick += Tree_NodeMouseDoubleClick;
            m_tree.Visible = true;
            m_tree.Nodes.Add(new TreeNode() { Text = "None", SelectedImageIndex = 0 });

            //m_rtb = new RichTextBox();
            //m_rtb.Dock = DockStyle.Fill;
            //m_sc.Panel2.Controls.Add(m_rtb);
#if use_gecko
            var wb = new Gecko.GeckoWebBrowser();
            wb.LoadHtml("<html><body></body></html>", "http://blank");
#elif use_chromium
            var wb = new CefSharp.WinForms.ChromiumWebBrowser("");
#else
            var wb = new WebBrowser();
#endif
            wb.Dock = DockStyle.Fill;
            m_wb = wb;
            m_sc.Panel2.Controls.Add(m_wb);
        }

        #region db
        class pathItem
        {
            public UInt64 id;
            public string path;
        }
        static List<MyTitle> getTitles(OleDbConnection cnn)
        {
            //get paths
            var pathLst = new List<pathItem>();
            var qry = "select * from paths order by ord ASC";
            var cmd = new OleDbCommand(qry, cnn);
            var reader = cmd.ExecuteReader();
            while (reader.Read())
            {
                pathLst.Add(new pathItem() { id = Convert.ToUInt64(reader["ID"]),
                    path = Convert.ToString(reader["path"]) });
            }
            reader.Close();

            //get title
            var titleLst = new List<MyTitle>();
            var qry2 = "select * from titles WHERE pathId = @pathId order by ord ASC";
            var cmd2 = new OleDbCommand(qry2, cnn);
            cmd2.Parameters.Add("@pathId", OleDbType.BigInt);
            foreach(var pi in pathLst)
            {
                cmd2.Parameters[0].Value = pi.id;
                var reader2 = cmd2.ExecuteReader();
                int ord = 0;
                while (reader2.Read())
                {
                    var title = new MyTitle();
                    title.ID = Convert.ToUInt64(reader2["ID"]);
                    title.zTitle = Convert.ToString(reader2["title"]);
                    title.pathId = Convert.ToUInt64(reader2["pathId"]);
                    title.zPath = pi.path + "/" + title.zTitle;
                    title.ord = Convert.ToInt32(reader2["ord"]);
                    titleLst.Add(title);
                    Debug.Assert(title.ord > ord);
                    ord = title.ord;
                }
                reader2.Close();
            }
            
            return titleLst;
        }

        static List<MyParagraph> getTitleParagraphs(OleDbConnection cnn, UInt64 titleId)
        {
            var paragraphLst = new List<MyParagraph>();
            //get title
            var qry = "select * from paragraphs where titleId = @id ";
            var cmd = new OleDbCommand(qry, cnn);
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
            paragraphLst.Sort((x, y) => { return (x.order - y.order); });

            int order = 0;
            foreach (var par in paragraphLst)
            {
                Debug.Assert(par.order > order);
                order = par.order;
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
            [DataMember(Name = "paragraphs", EmitDefaultValue = false)]
            public List<MyParagraph> paragraphLst;
            public int ord;
        }
        [DataContract(Name = "MyParagraph")]
        class MyParagraph
        {
            public UInt64 ID;
            public String zTitle;
            public UInt64 titleId;
            [DataMember(Name = "order", EmitDefaultValue = false)]
            public int order;
            [DataMember(Name = "alignment", EmitDefaultValue = false)]
            public int alignment;
            [DataMember(Name = "leftIndent", EmitDefaultValue = false)]
            public int leftIndent;
            [DataMember(Name = "fontSize", EmitDefaultValue = false)]
            public int fontSize;
            [DataMember(Name = "fontBold", EmitDefaultValue = false)]
            public int fontBold;
            [DataMember(Name = "fontItalic", EmitDefaultValue = false)]
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
        List<MyTitle> getTitles()
        {
            OleDbConnection conn = new OleDbConnection(m_cnnStr);
            conn.Open();

            List<MyTitle> titleLst = getTitles(conn);
            foreach (var title in titleLst)
            {
                title.paragraphLst = getTitleParagraphs(conn, title.ID);
                //break;
            }

            conn.Close();
            return titleLst;
        }
        #endregion
        #region event_process
        private void Tree_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            //throw new NotImplementedException();
            OnNodeMouseClick(sender, e);
        }
        private void Tree_NodeMouseDoubleClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            TreeNode tn = e.Node;
            string path = (string)tn.Tag;
            bool ret = System.IO.File.Exists(path);
            Process proc = new Process();
            proc.StartInfo.FileName = path;
            proc.StartInfo.UseShellExecute = true;
            //proc.StartInfo.Arguments = path;
            proc.StartInfo.Verb = "open";
            //proc.Start();
        }
        private void Open_Click(object sender, EventArgs e)
        {
#if true
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "access files (*.accdb)|*.accdb";
            //ofd.Multiselect = true;
            var ret = ofd.ShowDialog();
            if (ret == DialogResult.OK)
            {
                m_db = ofd.FileName;
                var cnnStr = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=<db>;";
                m_cnnStr = cnnStr.Replace("<db>", m_db);

                //clear tree
                m_nodeDict.Clear();
                m_tree.Nodes.Clear();

                var titles = getTitles();
                addTitles(titles);
                renderTree(m_nodeDict.Values.ElementAt(0));
            }
#endif
        }

        private void Preview_Click(object sender, EventArgs e)
        {
            var selected = getSelectedTitles();
            if (selected.Count > 0)
            {
                string jsTxt = titlesLstToJson(selected);
                string htmlTxt = genHtmlTxt(jsTxt);
                UpdateWB(htmlTxt);
            }
        }
        private void Export_Click(object sender, EventArgs e)
        {
            //get selected item
            var selected = getSelectedTitles();
            if (selected.Count != 0)
            {
                SaveFileDialog saveFileDialog1 = new SaveFileDialog();
                saveFileDialog1.Filter = "html files (*.html)|*.html|All files (*.*)|*.*";
                saveFileDialog1.FilterIndex = 1;
                saveFileDialog1.RestoreDirectory = true;
                if (saveFileDialog1.ShowDialog() == DialogResult.OK)
                {
                    exportTitles(saveFileDialog1.FileName, selected);
                }
            }
            else
            {
                MessageBox.Show("No title seleted!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
        List<MyTitle> getSelectedTitles()
        {
            List<MyTitle> selected = new List<MyTitle>();
            Queue<TreeNodeCollection> q = new Queue<TreeNodeCollection>();
            q.Enqueue(m_tree.Nodes);
            while (q.Count > 0)
            {
                foreach (TreeNode tn in q.Dequeue())
                {
                    if (tn.StateImageIndex != 0)
                    {
                        if (tn.Nodes.Count > 0)
                        {
                            q.Enqueue(tn.Nodes);
                        }
                        else if (tn.StateImageIndex == 1)
                        {
                            Node node = m_nodeDict[(string)tn.Tag];
                            Debug.Assert(node.type == 'T');
                            selected.Add(node.title);
                        }
                    }
                }
            }
            return selected;
        }
        void exportTitles(string path, List<MyTitle> selected)
        {
            string jsTxt = titlesLstToJson(selected);
            string htmlTxt = genHtmlTxt(jsTxt);
            File.WriteAllText(path, htmlTxt);
        }
        private void DisplayTitle(string key)
        {
            var tNode = m_nodeDict[key];
            var title = tNode.title;
            if (title == null) { return; }
            //m_rtb.Clear();
            //foreach(var par in title.paragraphLst)
            //{
            //    FontStyle fs = new FontStyle();
            //    if (par.fontBold == -1) { fs |= FontStyle.Bold; }
            //    if (par.fontItalic == -1) { fs |= FontStyle.Italic; }
            //    m_rtb.Font = new Font(m_rtb.Font, fs);
            //    m_rtb.SelectionIndent = par.leftIndent;
            //    if (par.alignment == 1) {m_rtb.SelectionAlignment = HorizontalAlignment.Center;}
            //    m_rtb.SelectedText = par.content + "\n";
            //}
            string jsTxt = titlesLstToJson(title);
            string htmlTxt = genHtmlTxt(jsTxt);
            UpdateWB(htmlTxt);
        }

        string genHtmlTxt(string jsTxt)
        {
            var path = Environment.CurrentDirectory;
            string zTmpl = "templ.html";
            while (path != null)
            {
                path = Path.GetDirectoryName(path);
                if (File.Exists(path + "\\" + zTmpl)) { break; }
            }
            var txt = File.ReadAllText(path + "\\" + zTmpl);
            var htmlTxt = txt.Replace("var jsTxt = null", "var jsTxt = " + jsTxt);
            return htmlTxt;
        }

        string titlesLstToJson(MyTitle title)
        {
            List<MyTitle> titleLst = new List<MyTitle>() { title };
            return titlesLstToJson(titleLst);
        }
        string titlesLstToJson(List<MyTitle> titleLst)
        {
            var x = createSerializer(titleLst.GetType());
            var mem = new MemoryStream();
            x.WriteObject(mem, titleLst);
            StreamReader sr = new StreamReader(mem);
            mem.Position = 0;
            string myStr = sr.ReadToEnd();
            return myStr;
        }

        protected void UpdateWB(string htmlTxt)
        {
            string filename = string.Format(@"{0}{1}", Path.GetTempPath(), "page.htm");
            File.WriteAllText(filename, htmlTxt);
#if use_gecko
            m_wb.Navigate(filename);
#elif use_chromium
            m_wb.Load(filename);
#else
            m_wb.DocumentText = htmlTxt;
#endif
            //OpenInBrowser(htmlTxt);
        }
        #endregion

        #region tree
        void addTitles(List<MyTitle> titles)
        {
            foreach(var title in titles)
            {
                var tNode = addRow('T', (UInt64)title.paragraphLst.Count, 
                    title.zPath, title.zTitle);
                tNode.title = title;
            }
        }

        int getNo(string txt)
        {
            Regex reg = new Regex("([IVXLC]+)|một|hai|ba|bốn|năm|sáu|bảy|tám|chín|mười|mươi");
            var mc = reg.Matches(txt);
            //tap I
            string zNo = txt.Substring(4);
            switch (zNo)
            {
                case "I":return 1;
                case "II": return 2;
                case "III": return 3;
                case "IV": return 4;
                case "V": return 5;
                case "VI": return 6;
                case "VII": return 7;
                case "VIII": return 8;
                case "IX": return 9;
            }
            return 0;
        }
        Node addRow(char type, UInt64 size, string name, string dir = "")
        {
            var tDict = m_nodeDict;
            var arr = name.Split(new char[] { '\\', '/' });
            //arr[0] = dir;
            string path = arr[0];
            Node parent;
            if (tDict.ContainsKey(path))
            {
                parent = tDict[path];
            }
            else
            {
                parent = new Node() { id = path, name = path };
                tDict.Add(path, parent);
            }
            Node child;
            for (int j = 1; j < arr.Length; j++)
            {
                path = path + "\\" + arr[j];
                if (tDict.ContainsKey(path))
                {
                    child = tDict[path];
                }
                else
                {
                    child = new Node() { id = path, name = arr[j] };
                    tDict.Add(path, child);
                    parent.childs.Add(child);
                }

                parent.size++;
                parent = child;
            }
            parent.size = size;
            parent.type = type;
            return parent;
        }
        TreeNode CreateTreeNode(Node node)
        {
            //if (node.type != 'T') { node.size = (UInt64)node.childs.Count; }
            string name = string.Format("{0} ({1})", node.name, node.size);
            TreeNode newNode = new TreeNode(name)
            {
                Tag = node.id,
                StateImageIndex = 0
            };
            return newNode;
        }
        void renderTree(Node root)
        {
            var tree = m_tree;
            m_tree.Nodes.Clear();

            var tnRoot = CreateTreeNode(root);
            Queue<KeyValuePair<Node, TreeNode>> q = new Queue<KeyValuePair<Node, TreeNode>>();
            q.Enqueue(new KeyValuePair<Node, TreeNode>(root, tnRoot));
            while (q.Count > 0)
            {
                var rec = q.Dequeue();
                foreach (Node child in rec.Key.childs)
                {
                    var tnChild = CreateTreeNode(child);
                    rec.Value.Nodes.Add(tnChild);
                    q.Enqueue(new KeyValuePair<Node, TreeNode>(child, tnChild));
                }
            }

            tree.Nodes.Add(tnRoot);
        }
        private ImageList CrtChkBoxImg()
        {
            var lst = new ImageList();
            for (int i = 0; i < 3; i++)
            {
                // Create a bitmap which holds the relevent check box style
                // see http://msdn.microsoft.com/en-us/library/ms404307.aspx and http://msdn.microsoft.com/en-us/library/system.windows.forms.checkboxrenderer.aspx

                System.Drawing.Bitmap bmp = new System.Drawing.Bitmap(16, 16);
                System.Drawing.Graphics chkGraphics = System.Drawing.Graphics.FromImage(bmp);
                switch (i)
                {
                    // 0,1 - offset the checkbox slightly so it positions in the correct place
                    case 0:
                        System.Windows.Forms.CheckBoxRenderer.DrawCheckBox(chkGraphics, new System.Drawing.Point(0, 1), System.Windows.Forms.VisualStyles.CheckBoxState.UncheckedNormal);
                        break;
                    case 1:
                        System.Windows.Forms.CheckBoxRenderer.DrawCheckBox(chkGraphics, new System.Drawing.Point(0, 1), System.Windows.Forms.VisualStyles.CheckBoxState.CheckedNormal);
                        break;
                    case 2:
                        System.Windows.Forms.CheckBoxRenderer.DrawCheckBox(chkGraphics, new System.Drawing.Point(0, 1), System.Windows.Forms.VisualStyles.CheckBoxState.MixedNormal);
                        break;
                }

                lst.Images.Add(bmp);
            }
            return lst;
        }
        private ImageList CrtRadBtnImg()
        {
            var lst = new ImageList();
            for (int i = 0; i < 2; i++)
            {
                Bitmap bmp = new Bitmap(16, 16);
                Graphics chkGraphics = Graphics.FromImage(bmp);
                switch (i)
                {
                    // 0,1 - offset the checkbox slightly so it positions in the correct place
                    case 0:
                        RadioButtonRenderer.DrawRadioButton(chkGraphics, new Point(0, 1), System.Windows.Forms.VisualStyles.RadioButtonState.UncheckedNormal);
                        break;
                    case 1:
                        RadioButtonRenderer.DrawRadioButton(chkGraphics, new Point(0, 1), System.Windows.Forms.VisualStyles.RadioButtonState.CheckedNormal);
                        break;
                }

                lst.Images.Add(bmp);
            }
            return lst;
        }
        void initTree()
        {
            m_tree.CheckBoxes = false;
            m_tree.StateImageList = new ImageList();
            switch (m_treeStyle)
            {
                case TreeStyle.check:
                    m_tree.StateImageList = CrtChkBoxImg();
                    break;
                case TreeStyle.radio:
                    m_tree.StateImageList = CrtRadBtnImg();
                    break;
            }
        }
        protected void OnNodeMouseClick(object sender, System.Windows.Forms.TreeNodeMouseClickEventArgs e)
        {
            //base.OnNodeMouseClick(e);

            // is the click on the checkbox?  If not, discard it
            TreeViewHitTestInfo info = m_tree.HitTest(e.X, e.Y);
            if (info == null)
            {
                return;
            }
            else if (info.Location == TreeViewHitTestLocations.Label)
            {
                string key = (string)e.Node.Tag;
                DisplayTitle(key);
            }
            else if (info.Location == System.Windows.Forms.TreeViewHitTestLocations.StateImage)
            {
                TreeNode tn = e.Node;
                var bChk = Check(tn);
                Check(tn, !bChk);
            }
            else
            {
                return;
            }
        }
        protected enum TreeStyle
        {
            check,
            radio,
        }
        protected TreeStyle m_treeStyle;
        private void Tree_AfterCheck(object sender, TreeViewEventArgs e)
        {
            //if (e.Action != TreeViewAction.Unknown)
            switch (m_treeStyle)
            {
                case TreeStyle.check:
                    updateChkBoxState(e);
                    break;
                case TreeStyle.radio:
                    UpdateRadBtnState(e);
                    break;
            }

            OnSelectedChg();
        }
        private void updateChkBoxState(TreeViewEventArgs e)
        {
            // check/uncheck tree nodes
            var val = Check(e.Node);
            var lst = new List<TreeNode>();
            lst.Add(e.Node);
            while (lst.Count > 0)
            {
                var node = lst[0];
                lst.RemoveAt(0);
                var parent = node.Parent;
                if (parent != null)
                {
                    CheckParentNode(parent, val);
                    lst.Add(parent);
                }
            }
            lst.Add(e.Node);
            while (lst.Count > 0)
            {
                var node = lst[0];
                lst.RemoveAt(0);
                if (Check(node) != val) { Check(node, val); }
                lst.AddRange(node.Nodes.Cast<TreeNode>());
            }
        }
        private void UpdateRadBtnState(TreeViewEventArgs e)
        {
            var val = Check(e.Node);
            if (!val)
            {
                Check(e.Node, true);
                Check(e.Node, 1);
            }
            else
            {
                //uncheck other node
                foreach (TreeNode node in m_tree.Nodes)
                {
                    if (Check(node)) { Check(node, 0); Check(node, false); }
                }
                Check(e.Node, true);
                Check(e.Node, 1);
            }
        }
        public virtual void OnSelectedChg() { }
        class Node
        {
            public MyTitle title;
            public string id;
            public char type;
            public UInt64 size;
            public string name;
            public List<Node> childs = new List<Node>();
        }

        private List<TreeNode> GetAllLeafs(TreeNode parent)
        {
            var lst = new List<TreeNode>();
            var q = new List<TreeNode>();
            q.Add(parent);
            while (q.Count > 0)
            {
                var n = q[0];
                q.RemoveAt(0);
                if (n.Nodes.Count == 0) { lst.Add(n); }
                else
                {
                    q.AddRange(n.Nodes.Cast<TreeNode>().ToList());
                }
            }
            return lst;
        }

        private void CheckParentNode(TreeNode parent, bool val)
        {
            int i = 0;
            var childLst = parent.Nodes;
            for (; i < childLst.Count; i++)
            {
                var child = childLst[i];
                if (!(Check(child) == val)) { break; }    //child not checked
            }
            if (i == childLst.Count)
            {
                if (Check(parent) != val) { Check(parent, val); }
                Check(parent, val ? 1 : 0);
            }
            else
            {
                if (Check(parent) != false) { Check(parent, false); }
                Check(parent, 2);
            }
        }

        private void UpdateParent(TreeNode node, int idx)
        {
            int state = idx;
            foreach (TreeNode child in node.Nodes)
            {
                if (child.StateImageIndex != idx)
                {
                    state = 2; //gray
                    break;
                }
            }
            node.StateImageIndex = state;

            if (node.Parent != null)
            {
                UpdateParent(node.Parent, idx);
            }
        }

        private bool Check(TreeNode node, bool val)
        {
            int idx = val ? 1 : 0;
            node.StateImageIndex = idx;

            if (node.Nodes.Count > 0)
            {
                UpdateChilds(node, idx);
            }

            if (node.Parent != null)
            {
                UpdateParent(node.Parent, idx);
            }

            return idx == 1;
        }
        protected bool Check(TreeNode node, int idx = -1)
        {
            if (idx == -1)
            {
                return node.StateImageIndex == 1;
            }
            else
            {
                node.StateImageIndex = idx;
                return idx == 1;
            }
        }
        private void UpdateChilds(TreeNode node, int idx)
        {
            foreach (TreeNode i in node.Nodes)
            {
                i.StateImageIndex = idx;
                if (i.Nodes.Count > 0) { UpdateChilds(i, idx); }
            }
        }
        #endregion
    }
}
