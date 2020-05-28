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
using System.Speech.Synthesis;

namespace WindowsFormsApp1
{
    public partial class Form1 : Form
    {
        protected TreeView m_tree;
        Dictionary<string, Node> m_nodeDict;
        BackgroundWorker m_bw;
        SplitContainer m_sc;
        SplitContainer m_rsc;
        RichTextBox m_rtb;
        List<MyTitle> m_titles;

        string m_curTitle = "";     //xxx/yyy
        bool isEditing = false;

#if use_gecko
        protected Gecko.GeckoWebBrowser m_wb;
#elif use_chromium
        protected CefSharp.WinForms.ChromiumWebBrowser m_wb;
#else
        protected WebBrowser m_wb;
#endif
        string m_db;
        string m_cnnStr;
        lContentProvider m_content { get { return ConfigMng.getInstance().m_content; } }
        public Form1()
        {
            InitializeComponent();

            var menu = new MainMenu();
            var file = menu.MenuItems.Add("&File");
            file.MenuItems.Add("&Open").Click += (s, e) => { OnOpenDb(s, e); };
            file.MenuItems.Add("&Preview").Click += (s, e) => { PreviewTitle(s, e); };
            file.MenuItems.Add("&Export").Click += (s, e) => { ExportSelected(s, e); };
            file.MenuItems.Add("&Find").Click += (s, e) => { OpenFindWnd(); };
            this.Menu = menu;

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
            m_tree.NodeMouseClick += OnTreeNodeClick;
            m_tree.NodeMouseDoubleClick += OnTreeNodeDblClick;
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
            m_rsc = new SplitContainer()
            {
                Dock = DockStyle.Fill,
            };
            m_rsc.Panel1.Controls.Add(m_wb);

            var edtPanel = new EditPanel();
            m_edtPanel = edtPanel;

            m_rsc.Panel2Collapsed = true;
            m_rsc.Panel2.Hide();
            m_rsc.Panel2.Controls.Add(edtPanel.m_tlp);

            m_sc.Panel2.Controls.Add(m_rsc);

            var bs = new BindingSource
            {
                DataSource = edtPanel.m_dataTable
            };
            edtPanel.m_dataTable.RowChanged += OnEditPar;
            edtPanel.OnHideEditor += OnCloseEditor;
            bs.CurrentItemChanged += Bs_CurrentItemChanged;
            bs.CurrentChanged += Bs_CurrentChanged;

            this.Load += OnLoadForm;
            this.FormClosed += OnCloseForm;
        }

        private void OpenFindWnd()
        {
            var dlg = new Form();
            dlg.Name = "Find";
            dlg.Icon = new Icon(@"..\..\..\Search.ico");
            var srchPanel = new SearchPanel(ConfigMng.getInstance().m_cnnInfo.cnnStr);
            dlg.Controls.Add(srchPanel.m_tblLayout);
            srchPanel.OnSelectTitle += (s, e) =>
            {
                var title = m_titles.Find((x) => { return x.ID == e; });
                DisplayTitle2(title);
            };

            dlg.AcceptButton = srchPanel.m_acceptBtn;

            var cfg = ConfigMng.getInstance();
            if (cfg.m_srchWndSize.Width > 0)
            {
                dlg.Location = cfg.m_srchWndPos;
                dlg.Size = cfg.m_srchWndSize;
            }
            dlg.FormClosed += (s, e) =>
            {
                cfg.m_srchWndPos = dlg.Location;
                cfg.m_srchWndSize = (dlg.WindowState == FormWindowState.Normal) ? dlg.Size : dlg.RestoreBounds.Size;
            };
            dlg.Show();
        }

        SpeechSynthesizer m_ss;
        private void SpeechTitle(MyTitle title)
        {
            //throw new NotImplementedException();
            if (m_ss == null)
            {
                var ss = new SpeechSynthesizer();
                ss.SetOutputToDefaultAudioDevice();
                var iv = ss.GetInstalledVoices();
                var vn = iv.First(v => { return v.VoiceInfo.Culture.Name == "vi-VN"; });
                ss.SelectVoice(vn.VoiceInfo.Name);
                m_ss = ss;
            }
            foreach (MyParagraph p in title.paragraphLst)
            {
                m_ss.SpeakAsync(p.content);
            }
        }
        private void SpeechStop()
        {
            m_ss.SpeakAsyncCancelAll();
        }

        private void OnCloseEditor(object sender, EventArgs e)
        {
            m_rsc.Panel2Collapsed = true;
            m_rsc.Panel2.Hide();
            isEditing = false;
        }

        private void Bs_CurrentChanged(object sender, EventArgs e)
        {
            throw new NotImplementedException();
        }

        private void Bs_CurrentItemChanged(object sender, EventArgs e)
        {
            throw new NotImplementedException();
        }

        private void OnLoadForm(object sender, EventArgs e)
        {
            var cfg = ConfigMng.getInstance();
            var cnnStr = cfg.m_cnnInfo.cnnStr;
            if (cnnStr == null && OpenDbDlg())
            {
                cfg.m_cnnInfo.cnnStr = m_cnnStr;
                cfg.m_content.initCnn(m_cnnStr);
                renderTree();

                //set form title
                UpdateFormName();
            }
            else
            {
                m_cnnStr = cfg.m_cnnInfo.cnnStr;
                cfg.m_content.initCnn(m_cnnStr);
                renderTree();

                //restore state
                restoreSts();

                if (cfg.m_wndSize.Width > 0)
                {
                    this.Location = cfg.m_wndPos;
                    this.Size = cfg.m_wndSize;
                }

                //set form title
                UpdateFormName();
            }

        }

        void UpdateFormName()
        {
            foreach (var title in m_titles)
            {
                var path = title.zPath;
                var n = path.IndexOf('/');
                if (n == -1) break;
                this.Text = path.Substring(0, n);
                break;
            }
        }

        TreeNode GetTreeNode(string path)
        {
            return m_nodeDict[path].tnode;
        }
        void restoreSts()
        {
            var sts = ConfigMng.getInstance().m_curSts;
            foreach (string path in sts.selectedTitles)
            {
                var node = GetTreeNode(path);
                Check(node, true);
            }
            foreach (string path in sts.expandedNodes)
            {
                var node = GetTreeNode(path);
                node.Expand();
            }

            foreach (var title in m_titles)
            {
                if (title.zPath == sts.readingTitle)
                {
                    DisplayTitle2(title);
                    break;
                }
            }
        }

        private void OnCloseForm(object sender, FormClosedEventArgs e)
        {
            Queue<TreeNodeCollection> q = new Queue<TreeNodeCollection>();
            q.Enqueue(m_tree.Nodes);
            List<string> selected = new List<string>();
            List<string> expanded = new List<string>();
            while (q.Count > 0)
            {
                var nodes = q.Dequeue();
                foreach (TreeNode node in nodes)
                {
                    if (node.Nodes.Count > 0)
                    {
                        q.Enqueue(node.Nodes);
                    }
                    else
                    {
                        //leaf
                        if (node.StateImageIndex == 1)
                        {
                            selected.Add((string)node.Tag);
                        }
                    }

                    if (node.IsExpanded)
                    {
                        expanded.Add((string)node.Tag);
                    }
                }
            }
            var cfg = ConfigMng.getInstance();
            cfg.m_curSts.selectedTitles = selected;
            cfg.m_curSts.expandedNodes = expanded;
            cfg.m_curSts.readingTitle = m_curTitle;
            cfg.m_wndPos = this.Location;
            cfg.m_wndSize = (this.WindowState == FormWindowState.Normal) ? this.Size : this.RestoreBounds.Size;
            cfg.SaveConfig();
        }

        EditPanel m_edtPanel;
        bool m_loadTitleCompleted;
        private void OnEditPar(object sender, EventArgs e)
        {
            if (!m_loadTitleCompleted) return;

            var key = m_edtPanel.m_title.zPath;
            m_curTitle = key;

            var title = m_nodeDict[key].title;
            title.paragraphLst = m_content.getTitleParagraphs(m_edtPanel.m_dataTable);
            string jsTxt = titlesLstToJson(title);
            string htmlTxt = genHtmlTxt(jsTxt);
            UpdateWB(htmlTxt);
        }

        #region db

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
            List<MyTitle> titleLst = m_content.getTitles2();
            foreach (var title in titleLst)
            {
                //title.paragraphLst = getTitleParagraphs(conn, title.ID);
                //break;
            }

            return titleLst;
        }
        #endregion
        #region event_process
        private void OnTreeNodeClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            //throw new NotImplementedException();
            OnNodeMouseClick(sender, e);
        }
        private void OnTreeNodeDblClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            var title = m_nodeDict[(string)e.Node.Tag].title;
            if (title == null) return;
            isEditing = true;
            BeginEditTitle(title);
        }

        private void BeginEditTitle(MyTitle title)
        {
            //edit
            m_edtPanel.m_title = new MyTitle()
            {
                ID = title.ID,
                zPath = title.zPath,
                zTitle = title.zTitle
            };
            m_loadTitleCompleted = false;
            m_edtPanel.loadTitle();
            m_loadTitleCompleted = true;
            m_rsc.Panel2Collapsed = false;
            m_rsc.Panel2.Show();
        }
        private void OnOpenDb(object sender, EventArgs e)
        {
            if (OpenDbDlg())
            {
                //Close editor
                m_edtPanel.clear();
                m_rsc.Panel2Collapsed = true;
                m_rsc.Panel2.Hide();

                //update config
                var cnf = ConfigMng.getInstance();
                cnf.m_content.closeCnn();
                cnf.m_cnnInfo.cnnStr = m_cnnStr;
                cnf.m_content.initCnn(m_cnnStr);
                renderTree();

                //reset current title
                m_curTitle = "";

                //update form title
                UpdateFormName();
            }
        }

        bool OpenDbDlg()
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "access files (*.accdb)|*.accdb";
            //ofd.Multiselect = true;
            var ret = ofd.ShowDialog();
            if (ret == DialogResult.OK)
            {
                m_db = ofd.FileName;
                var cnnStr = "Provider=Microsoft.ACE.OLEDB.12.0;Data Source=<db>;";
                m_cnnStr = cnnStr.Replace("<db>", m_db);
                return true;
            }
            return false;
        }

        void renderTree()
        {
            //clear tree
            m_nodeDict.Clear();
            m_tree.Nodes.Clear();

            var titles = getTitles();
            m_titles = titles;
            addTitles(titles);
            renderTree(m_nodeDict.Values.ElementAt(0));
        }

        private void PreviewTitle(object sender, EventArgs e)
        {
            var selected = getSelectedTitles();
            if (selected.Count > 0)
            {
                //reset current title
                m_curTitle = "";

                string jsTxt = titlesLstToJson(selected);
                string htmlTxt = genHtmlTxt(jsTxt);
                UpdateWB(htmlTxt);
            }
        }
        private void ExportSelected(object sender, EventArgs e)
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
            foreach (var title in selected)
            {
                if (title.paragraphLst == null)
                {
                    title.paragraphLst = m_content.getTitleParagraphs(title.ID);
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

        private void DisplayTitle(MyTitle title)
        {
            Debug.Assert(title.zPath != m_curTitle);
            title.paragraphLst = m_content.getTitleParagraphs(title.ID);
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
            m_wb.Navigate(filename);
#endif
            //OpenInBrowser(htmlTxt);
        }
        #endregion

        #region tree
        void addTitles(List<MyTitle> titles)
        {
            foreach (var title in titles)
            {
                var tNode = addRow('T', title.paragraphLst == null ? 0 : (UInt64)title.paragraphLst.Count,
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
                case "I": return 1;
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
                path = path + "/" + arr[j];
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
        TreeNode CreateTreeNode(Node node, TreeNode newNode = null)
        {
            //if (node.type != 'T') { node.size = (UInt64)node.childs.Count; }
            string name = node.size == 0 ? node.name :
                string.Format("{0} ({1})", node.name, node.size);
            if (newNode == null) newNode = new TreeNode(name);
            else newNode.Name = name;

            newNode.Tag = node.id;
            newNode.StateImageIndex = 0;
#if RightClickCMS
            if (node.title != null)
            {
                ContextMenuStrip contextMenuStrip;
                contextMenuStrip = new ContextMenuStrip();
                contextMenuStrip.Items.Add("Speech").Click += (s, e)=>{ SpeechTitle(node.title); };
                contextMenuStrip.Items.Add("Stop").Click += (s, e) => { SpeechStop(); };
                newNode.ContextMenuStrip = contextMenuStrip;
            }
#endif
            node.tnode = newNode;
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

                var tNode = m_nodeDict[key];
                var title = tNode.title;
                if (title == null) { return; }

                //avoid re display title
                DisplayTitle2(title);
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

        void DisplayTitle2(MyTitle title)
        {
            if (m_curTitle == title.zPath) return;

            DisplayTitle(title);
            if (isEditing)
            {
                BeginEditTitle(title);
            }

            m_curTitle = title.zPath;
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

            public TreeNode tnode;
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

    [DataContract(Name = "MyTitle")]
    public class MyTitle
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
    public class MyParagraph
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
}
