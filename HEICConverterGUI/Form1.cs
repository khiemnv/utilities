using ImageMagick;
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
using System.Text.RegularExpressions;
using System.Threading;
using System.Collections.Concurrent;

namespace GUI
{
    public partial class Form1 : Form
    {
        //protected Menu m_menu;
        string m_srcDir;
        protected TreeView m_tree;
        Dictionary<string, Node> m_nodeDict;
        BackgroundWorker m_bw;
        PictureBox m_pb;
        SplitContainer m_sc;
        public Form1()
        {
            InitializeComponent();
#if true
            var m_menu = new MenuStrip();
            ToolStripMenuItem file = new ToolStripMenuItem("&File");
            m_menu.Items.Add(file);
            ToolStripMenuItem open = new ToolStripMenuItem("&Open");
            file.DropDownItems.Add (open);
            ToolStripMenuItem export = new ToolStripMenuItem("&Export");
            file.DropDownItems.Add(export);
            //ToolStripMenuItem refresh = new ToolStripMenuItem("&Refresh");
            //m_menu.Items.Add(refresh);
            //var refresh = Menu.MenuItems.Add("&Refresh");

            var edit = new ToolStripMenuItem("&Edit");
            m_menu.Items.Add(edit);
            var rLeft = new ToolStripMenuItem("&Left(90°)");
            var rRight = new ToolStripMenuItem("&Right(90°)");
            edit.DropDownItems.AddRange(new ToolStripItem[] { rLeft, rRight });
#else
            this.Menu = new MainMenu();
            var file = Menu.MenuItems.Add("&File");
            var open = file.MenuItems.Add("&Open");
            var export = file.MenuItems.Add("&Export");
            //var refresh = Menu.MenuItems.Add("&Refresh");

            var edit = Menu.MenuItems.Add("&Edit");
            var rLeft = edit.MenuItems.Add("&Left(90°)");
            var rRight = edit.MenuItems.Add("&Right(90°)");
#endif

            open.Click += Open_Click;
            export.Click += Export_Click;
            //refresh.Click += Refresh_Click;
            rLeft.Click += RLeft_Click;
            rRight.Click += RRight_Click;

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


            m_bw = new BackgroundWorker();
            m_bw.DoWork += M_bw_DoWork;
            m_bw.ProgressChanged += M_bw_ProgressChanged;
            m_bw.RunWorkerCompleted += M_bw_RunWorkerCompleted;
            m_bw.WorkerReportsProgress = true;

            m_sc.Visible = true;
#if use_progress_bar
            progressBar.Visible = false;
            progressBar.Dock = DockStyle.Bottom;
            Controls.Add(progressBar);
#endif

            m_pb = new PictureBox();
            m_pb.MouseWheel += M_pb_MouseWheel;
            m_pb.MouseDown += M_pb_MouseDown;
            m_pb.MouseMove += M_pb_MouseMove;
            m_pb.MouseUp += M_pb_MouseUp;
            //m_pb.Dock = DockStyle.Fill;
            m_sc.Panel2.Controls.Add(m_pb);

            this.Controls.Add(m_menu);

            Load += Form1_Load;
            AllowDrop = true;
            DragEnter += Form1_DragEnter;
            DragDrop += Form1_DragDrop;
        }

        private void Form1_DragEnter(object sender, DragEventArgs e)
        {
            if (e.Data.GetDataPresent(DataFormats.FileDrop)) e.Effect = DragDropEffects.Copy;
        }

        private void Form1_DragDrop(object sender, DragEventArgs e)
        {
            string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);

            UInt64 size = 0;
            m_nodeDict.Clear();
            foreach (string file in files)
            {
                size += addFileFolder(file);
            }
            if (size > 0)
            {
                renderTree(m_nodeDict.Values.ElementAt(0));
                Display1st();
            }
        }

        private Boolean canMove = false;
        private int prevY;
        private int prevX;
        private int threshHold = 5;

        class median
        {
            Queue<int> q;
            private int maxCount = 5;
            public median()
            {
                reset();
            }
            public void reset()
            {
                q = new Queue<int>();
            }
            public void push(int v)
            {
                q.Enqueue(v);
                if(q.Count > maxCount)
                {
                    q.Dequeue();
                }
            }
            public int calc()
            {
                var b = q.ToList();
                b.Sort();
                
                return b[b.Count/2];
            }
        }

        private median aX = new median();
        private median aY = new median();
        private void M_pb_MouseUp(object sender, MouseEventArgs e)
        {
            canMove = false;
        }

        private void M_pb_MouseMove(object sender, MouseEventArgs e)
        {
            if (canMove)
            {
                //Debug.WriteLine(String.Format("X: {0} Y:{1}", e.X, e.Y));
                aX.push(e.X);
                aY.push(e.Y);
                var mY = aY.calc();
                var mX = aX.calc();
                if (Math.Abs(mY - prevY) > threshHold)
                {
                    m_pb.Top += mY - prevY;
                    prevY = mY;
                }
                if (Math.Abs(mX - prevX) > threshHold)
                {
                    m_pb.Left += mX - prevX;
                    prevX = mX;
                }
            }
        }

        private void M_pb_MouseDown(object sender, MouseEventArgs e)
        {
            canMove = true;
            prevY = e.Y;
            prevX = e.X;
            aX.reset();
            aY.reset();
        }

        private void M_pb_MouseWheel(object sender, MouseEventArgs e)
        {
            if (e.Delta > 0)
            {
                ZoomInOut(true);
            }
            else
            {
                ZoomInOut(false);
            }
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            this.Text = "HEIC";
            Bitmap bitmap = (Bitmap)Image.FromFile(@"..\..\..\icon.ico");
            Icon = Icon.FromHandle(bitmap.GetHicon());
            if (Program.g_args.Length > 0) {
                var size = addFileFolder(Program.g_args[0]);
                if (size > 0)
                {
                    renderTree(m_nodeDict.Values.ElementAt(0));
                    Display1st();
                }
            }
        }
        void Display1st()
        {
            var node = m_tree.Nodes[0];
            while (node.Nodes.Count != 0)
            {
                node = node.Nodes[0];
            }
            DisplayImagine((string)node.Tag);
        }

        private void RRight_Click(object sender, EventArgs e)
        {
            if (m_curImg == null) return;
            m_curImg.Rotate(90);
            DisplayCurImagine();
        }

        private void RLeft_Click(object sender, EventArgs e)
        {
            if (m_curImg == null) return;
            m_curImg.Rotate(270);
            DisplayCurImagine();
        }
        private void ZoomInOut(bool zoom)
        {
            //Zoom ratio by which the images will be zoomed by default
            int zoomRatio = 10;
            //Set the zoomed width and height
            int widthZoom = m_pb.Width * zoomRatio / 100;
            int heightZoom = m_pb.Height * zoomRatio / 100;
            //zoom = true --> zoom in
            //zoom = false --> zoom out
            if (!zoom)
            {
                widthZoom *= -1;
                heightZoom *= -1;
            }
            //Add the width and height to the picture box dimensions
            m_pb.Width += widthZoom;
            m_pb.Height += heightZoom;
            m_pb.Top -= heightZoom / 2;
            m_pb.Left -= widthZoom / 2;

        }

        private void M_bw_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
#if use_progress_bar
            progressBar.Visible = false;
#endif
        }
#if use_progress_bar
        ProgressBar progressBar = new ProgressBar();
#endif
        private void M_bw_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
#if use_progress_bar
            progressBar.Value = e.ProgressPercentage;
#else
            m_progressDlg.m_cursor.setPos(e.ProgressPercentage);
#endif
        }

        class BWParam
        {
            public string srcDir;
            public string desDir;
            public List<string> selected;
        }


        private void M_bw_DoWork(object sender, DoWorkEventArgs e)
        {
            BWParam par = (BWParam)e.Argument;
            SpinLock spinLock = new SpinLock();
            int i = 0;
            var dict = new HashSet<string>();
            var arr = par.selected.ToArray();
            var arr2 = Array.ConvertAll(arr, file =>
            {

                string relPath = Path.GetDirectoryName(file.Substring(m_srcDir.Length + 1));
                string newDir = Path.Combine(par.desDir, relPath);
                if (!dict.Contains(newDir))
                {
                    if (!Directory.Exists(newDir))
                    {
                        Directory.CreateDirectory(newDir);
                    }
                    dict.Add(newDir);
                }
                string fileName = Path.GetFileNameWithoutExtension(file);
                string newFile = Path.Combine(par.desDir, relPath, fileName + ".jpg");
                return newFile;
            });
            var rangePartitioner = Partitioner.Create(0, arr.Length);
            Parallel.ForEach(rangePartitioner, (range, loopState) =>
            {
                // Loop over each range element without a delegate invocation.
                for (int index = range.Item1; index < range.Item2; index++)
                {
                    var file = arr[index];
                    var newFile = arr2[index];
                    exportToJpg(file, newFile);
                    bool lockTaken = false;
                    try
                    {
                        spinLock.Enter(ref lockTaken);
                        i++;
                        Debug.WriteLine("{0} {1}", index, i);
#if use_progress_bar
                m_bw.ReportProgress(i * 100 / par.selected.Count);
#else
                        m_bw.ReportProgress(i);
#endif
                    }
                    finally
                    {
                        if (lockTaken) spinLock.Exit();
                    }


                }
            });
#if use_progress_bar
            //delay to show progressbar 100%
            m_bw.ReportProgress(100);
            Thread.Sleep(100);
#endif
        }

        private void Export_Click(object sender, EventArgs e)
        {
            //get selected item
            List<string> selected = new List<string>();
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
                            Debug.Assert(node.type == 'F');
                            selected.Add(node.id);
                        }
                    }
                }
            }

            if (selected.Count != 0)
            {
                FolderBrowserDialog fbd = new FolderBrowserDialog();
                if (fbd.ShowDialog() == DialogResult.OK)
                {
                    exportToJpg(fbd.SelectedPath, selected, m_srcDir);
                }
            }
            else
            {
                MessageBox.Show("No file seleted!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
        void exportToJpg(string srcFile, string newFile)
        {
            //Thread.Sleep(1000);
            //return;
            using (MagickImage image = new MagickImage(srcFile))
            {
                image.Write(newFile);
            }
        }

        ProgressDlg m_progressDlg;
        class cursor : ICursor
        {
            long m_pos;
            string m_status = "";
            public long getPos()
            {
                return m_pos;
            }

            public string getStatus()
            {
                return m_status;
            }

            public void setPos(long pos)
            {
                m_pos = pos;
            }

            public void setStatus(string msg)
            {
                m_status = msg;
            }
        }
        void exportToJpg(string des, List<string> selected, string src)
        {
#if use_progress_bar
            progressBar.Value = 0;
            progressBar.Visible = true;
#endif
            m_bw.RunWorkerAsync(new BWParam() { desDir = des, selected = selected, srcDir = src });

            #if !use_progress_bar
            m_progressDlg = new ProgressDlg();
            m_progressDlg.m_cursor = new cursor();
            m_progressDlg.m_endPos = selected.Count;
            m_progressDlg.ShowDialog();
            #endif
        }
        private void Refresh_Click(object sender, EventArgs e)
        {
            renderTree(m_nodeDict.Values.ElementAt(0));
        }

        private void Open_Click(object sender, EventArgs e)
        {
#if false
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Filter = "heic files (*.heic)|*.heic";
            ofd.Multiselect = true;
            var ret = ofd.ShowDialog();
            if (ret == DialogResult.OK)
            {
                foreach (string file in ofd.FileNames)
                {
                    //
                    addRow('F', 0, file);
                }
            }
#endif
#if false
            var dialog = new CommonOpenFileDialog();
            dialog.IsFolderPicker = true;
            dialog.Multiselect = true;
            CommonFileDialogResult result = dialog.ShowDialog();
            if (result == CommonFileDialogResult.Ok)
            {
                foreach(string path in dialog.FileNames)
                {

                }
            }
#endif
#if true
            var fbd = new FolderBrowserDialog();
            var ret = fbd.ShowDialog();
            if (ret == DialogResult.OK)
            {
                m_nodeDict.Clear();
                m_srcDir = fbd.SelectedPath;
                UInt64 size = addFolder(m_srcDir);
                renderTree(m_nodeDict.Values.ElementAt(0));
            }
#endif
        }

        UInt64 addFileFolder(string path)
        {
            if (Directory.Exists(path))
            {
                return addFolder(path);
            } 
            else if (File.Exists(path) && Path.GetExtension(path).ToLower() == ".heic")
            {
                var fi = new FileInfo(path);
                var size = (UInt64)fi.Length;
                addRow('F', (UInt64)fi.Length, path, Path.GetDirectoryName(path));
                return size;
            }
            else
            {
                return 0;
            }
        }

        UInt64 addFolder(string path)
        {
            UInt64 size = 0;
            foreach (string file in Directory.GetFiles(path, "*.heic"))
            {
                var fi = new FileInfo(file);
                size += (UInt64)fi.Length;
                addRow('F', (UInt64)fi.Length, file, m_srcDir);
            }
            foreach (string sub in Directory.GetDirectories(path))
            {
                size += addFolder(sub);
            }
            if (size != 0)
            {
                m_nodeDict[path].size = size;
            }
            return size;
        }

        DataTable GetData()
        {
            DataTable dt = new DataTable();
            dt.Columns.Add("type", typeof(char));
            dt.Columns.Add("size", typeof(UInt64));
            dt.Columns.Add("name", typeof(string));

            System.IO.StreamReader rd = System.IO.File.OpenText(@"C:\tmp\log2.txt");

            string[] txt = {
                @"F 2415 D:\.Trash-1000\files\backupsettings.conf",
                @"F 28018176 D:\.Trash - 1000\files\BHome3388.exe",
                @"F 53082 D:\.Trash - 1000\files\brse.odt",
                @"F 16645 D:\.Trash - 1000\files\KhiemNV - CV - 20140429.docx",
                @"F 108886 D:\.Trash - 1000\files\KhiemNV.CV.pdf",
                @"D 0 D:\.Trash - 1000\files\linux",
            };

            txt = File.ReadAllLines(@"C:\tmp\log2.txt");

            //foreach (string line in txt)
            //for(; ; )
            //{
            //    string line = rd.ReadLine();
            //    if (rd.EndOfStream) break;
            for (int j = 0; j < txt.Length - 1; j++)
            {
                string line = txt[j];

                UInt64 size = 0;
                int i;
                for (i = 2; i < line.Length; i++)
                {
                    if (line[i] == ' ') { i++; break; }
                    size = (size * 10) + line[i] - '0';
                }
                dt.Rows.Add(new object[] { line[0], size, line.Substring(i) });
            }

            //rd.Close();
            return dt;
        }

        private void InitCtrls()
        {
            BuildTree();
        }
        private void Tree_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            //throw new NotImplementedException();
            OnNodeMouseClick(sender, e);
        }

        MagickImage m_curImg;
        void DisplayImagine(string path)
        {
            MagickImage img = new MagickImage(path);
            m_curImg = img;
            img.Format = MagickFormat.Bmp;
            //img.Rotate(90);
            DisplayCurImagine();
        }
        void DisplayCurImagine()
        {
            MagickImage img = m_curImg;
            int w = m_sc.Panel2.Width;
            int h = m_sc.Panel2.Height;
            MemoryStream ms = new MemoryStream();
            double scaleH = (double)img.Height / h;
            double scaleW = (double)img.Width / w;
            double scale = Math.Max(scaleW, scaleH);
            int newW = (int)((double)img.Width / scale);
            int newH = (int)((double)img.Height / scale);
            //m_pb.Dock = DockStyle.None;
            if (scaleH > scaleW)
            {
                m_pb.Left = (w - newW) / 2;
                m_pb.Top = 0;
            }
            else
            {
                m_pb.Left = 0;
                m_pb.Top = (h - newH) / 2;
            }
            m_pb.Width = newW;
            m_pb.Height = newH;

            //img.Scale(newW, newH);
            //img.Write(ms);
            //var newImg = img.Clone();
            var newImg = img;
            //newImg.Scale(newW, newH);
            newImg.Write(ms);
            Image bmpImg = Image.FromStream(ms);
            m_pb.SizeMode = PictureBoxSizeMode.Zoom;
            m_pb.Image = bmpImg;
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
                if (File.Exists(key))
                {
                    DisplayImagine(key);
                }
            }
            else if( info.Location == System.Windows.Forms.TreeViewHitTestLocations.StateImage)
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
        public class Node
        {
            public string id;
            public char type;
            public UInt64 size;
            public string name;
            public List<Node> childs = new List<Node>();
        }

        void addRow(char type, UInt64 size, string name, string dir = "")
        {
            var tDict = m_nodeDict;
            var arr = name.Substring(dir.Length).Split(new char[] { '\\' });
            arr[0] = dir;
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

                parent = child;
            }
            parent.size = size;
            parent.type = type;
        }
        private Node BuildTree(DataTable dt)
        {
            var tDict = m_nodeDict;
            foreach (DataRow row in dt.Rows)
            {
                char type = (char)row[0];
                UInt64 size = (UInt64)row[1];
                string name = (string)row[2];
                addRow(type, size, name);
            }
            return tDict.Values.ElementAt(0);
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
        private void BuildTree()
        {
            initTree();


            AddTreeNode();

        }

        TreeNode CreateTreeNode(Node node)
        {
            string txt;
            if (node.size > (1 << 30))
            {
                txt = string.Format("{0:0.00} GB", (double)node.size / (1 << 30));
            }
            else if (node.size > (1 << 20))
            {
                txt = string.Format("{0:0.00} MB", (double)node.size / (1 << 20));
            }
            else if (node.size > (1 << 10))
            {
                txt = string.Format("{0:0.00} KB", (double)node.size / (1 << 10));
            }
            else
            {
                txt = string.Format("{0} B", node.size);
            }
            string name = string.Format("{0} ({1})", node.name, txt);
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
        protected virtual void AddTreeNode()
        {
            DataTable dt = GetData();
            Node root = BuildTree(dt);
            renderTree(root);
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
            proc.Start();
        }

    }
}
