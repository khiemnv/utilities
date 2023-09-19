using Microsoft.WindowsAPICodePack.Controls.WindowsForms;
using Microsoft.WindowsAPICodePack.Dialogs;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Runtime.ConstrainedExecution;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml.Linq;
using static Microsoft.WindowsAPICodePack.Shell.PropertySystem.SystemProperties.System;

namespace cleanerGUI
{
    public partial class Form1 : Form
    {
        private string rootDir;
        private MyNode rootNode;
        private TreeView tree;
        private BackgroundWorker bg;

        private StatusStrip progressStatusStrip = new StatusStrip();
        private ToolStripProgressBar toolStripProgressBar = new ToolStripProgressBar();
        private ToolStripStatusLabel toolStripStatusLabel = new ToolStripStatusLabel();

        public Form1()
        {
            InitializeComponent();

            // split
            var spt = new SplitContainer();
            spt.Dock = DockStyle.Fill;

            // tree
            tree = new TreeView();
            tree.Dock = DockStyle.Fill;
            spt.Panel1.Controls.Add(tree);
            tree.NodeMouseClick += Tree_NodeMouseClick;

            // menu
            var fileContextMenuStrip = new ContextMenuStrip();
            fileContextMenuStrip.Items.Add("Open").Click += OnOpen;
            fileContextMenuStrip.Items.Add("-");
            fileContextMenuStrip.Items.Add("Exit").Click += OnExit;
            var file = new ToolStripMenuItem("File");
            file.DropDown = fileContextMenuStrip;
            var ms = new MenuStrip();
            ms.Items.Add(file);

            // progress bar
            Application.EnableVisualStyles();
            toolStripStatusLabel.Overflow = ToolStripItemOverflow.Always;
            progressStatusStrip.Items.Add(toolStripProgressBar);
            progressStatusStrip.Items.Add(toolStripStatusLabel);

            Controls.Add(spt);
            Controls.Add(ms);
            Controls.Add(progressStatusStrip);

            // bg
            bg = new BackgroundWorker();
            bg.WorkerReportsProgress = true;
            bg.DoWork += OnWork;
            bg.ProgressChanged += OnProgress;
            bg.RunWorkerCompleted += OnDone;

            //refresh key
            KeyDown += OnKeyDown;

            // cms
            var cms = new ContextMenuStrip();
            cms.Opening += new System.ComponentModel.CancelEventHandler(cms_Opening);
            ContextMenuStrip = cms;
        }

        private void Tree_NodeMouseClick(object sender, TreeNodeMouseClickEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                ContextMenuStrip.Show();
                ContextMenuStrip.Items.Clear();
                var n = (MyNode)e.Node.Tag;
                ContextMenuStrip.Items.Add("Clean").Click += (_s, _e) =>
                {
                    if(n.isFile)
                    {
                        var f = Path.Combine(n.parent.di.FullName, n.name);
                        switch (Path.GetExtension( n.name).ToLower())
                        {
                            case ".sln":
                                CleanSln(f);
                                Refresh(n.parent);
                                return;
                            case ".csproj":
                                CleanPrj(f);
                                Refresh(n.parent);
                                return;
                        }
                    }
                };
                ContextMenuStrip.Items.Add("Explore").Click += (_s, _e) =>
                {
                    var f = (n.isFile)?n.parent.di.FullName: n.di.FullName;
                    Process.Start(f);
                };
                base.ContextMenuStrip.Items.Add("Refresh").Click += (_s, _e) =>
                {
                    Refresh(n);
                };
                tree.SelectedNode = e.Node;
            }
        }

        private void Refresh(MyNode n)
        {
            var f = (n.isFile) ? n.parent.di.FullName : n.di.FullName;
            n.nodes.Clear();
            n.treeNode.Nodes.Clear();
            n.Size = 0;
            bg.RunWorkerAsync(n);
        }

        private void CleanSln(string f)
        {
            var cmd = new string[]
            {
                $@"call ""C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat""",
                $@"msbuild {f} -t:""Clean"" /p:Configuration=""Debug""",
                $@"msbuild {f} -t:""Clean"" /p:Configuration=""Release"""
            };
            var bat = Path.GetTempPath() + @"\cleanGui.bat";
            File.WriteAllLines(bat, cmd);
            var process = Process.Start(bat);
            process.WaitForExit(-1);
            File.Delete(bat);
        }
        private void CleanPrj(string f)
        {
            var cmd = new string[]
            {
                $@"call ""C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat""",
                $@"msbuild {f} -t:""Clean""",
            };
            var bat = Path.GetTempPath() + @"\cleanGui.bat";
            File.WriteAllLines(bat, cmd);
            var process = Process.Start(bat);
            process.WaitForExit(-1);
            File.Delete(bat);
        }

        private void OnKeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.F5)
            {
                if (Directory.Exists(rootDir))
                {
                    tree.Nodes.Clear();
                    bg.RunWorkerAsync(rootDir);
                }
            }
        }

        private void cms_Opening(object sender, System.ComponentModel.CancelEventArgs e)
        {
            var cms = (ContextMenuStrip)sender;
            //cms.Items.Add("Clean").Click += (_s, _e) =>
            //{

            //};

            // Set Cancel to false. 
            // It is optimized to true based on empty entry.
            e.Cancel = false;
        }

        public void SetProgressBar(string text, int value)
        {
            //Logger.Debug($"{text} {value}");
            if (value != 0)
            {
                toolStripProgressBar.Style = ProgressBarStyle.Marquee;
            }
            else
            {
                toolStripProgressBar.Style = ProgressBarStyle.Blocks;
            }
            //toolStripProgressBar.Value = value;
            toolStripStatusLabel.Text = text;
        }
        private void OnDone(object sender, RunWorkerCompletedEventArgs e)
        {
            var node = (MyNode)e.Result;
            var st = new List<MyNode> { node };
            if (node.treeNode == null && node.parent == null)
            {
                // if is is root then add to tree
                node.treeNode = tree.Nodes.Add(node.Label);
                node.treeNode.Tag = node;
            }
            while (st.Count > 0)
            {
                var cur = st[0];
                st.RemoveAt(0);
                if (cur.nodes != null)
                {
                    st.InsertRange(0, cur.nodes);
                }
                if (cur.treeNode == null)
                {
                    cur.treeNode = cur.parent.treeNode.Nodes.Add(cur.Label);
                    cur.treeNode.Tag = cur;
                }
                else if (cur.Label != cur.treeNode.Text)
                {
                    cur.treeNode.Text = cur.Label;
                }
            }

            // update parent info
            var p = node.parent;
            while (p!=null)
            {
                if (p.Label != p.treeNode.Text)
                {
                    p.treeNode.Text = p.Label;
                }
                p = p.parent;
            }

            // expand parent
            node.treeNode.Expand();

            // stop progress
            SetProgressBar(node.Label, 0);
        }

        private void OnProgress(object sender, ProgressChangedEventArgs e)
        {
            var cur = (MyNode)e.UserState;
            SetProgressBar(cur.Label, 1);
            return;
            if (cur.treeNode == null)
            {
                if (cur.parent == null)
                {
                    // root node
                    cur.treeNode = tree.Nodes.Add(cur.Label);
                } else
                {
                    cur.treeNode = cur.parent.treeNode.Nodes.Add(cur.Label);
                }
                cur.treeNode.Tag = cur;
            }
        }

        private void OnWork(object sender, DoWorkEventArgs e)
        {
            var node = (MyNode)e.Argument;
            var st = new List<MyNode>();
            st.Add(node);
            while (st.Count > 0)
            {
                var cur = st[0];
                st.RemoveAt(0);
                cur.nodes = cur.di.GetDirectories().ToList().ConvertAll((tdi) => new MyNode(tdi) { parent = cur });
                st.InsertRange(0, cur.nodes);
                foreach (var f in cur.di.GetFiles())
                {
                    var fNode = new MyNode(f) { parent = cur };
                    fNode.name = f.Name;
                    fNode.Size = f.Length;
                    cur.nodes.Add(fNode);
                }
                ((BackgroundWorker)sender).ReportProgress(1, cur);
            }
            e.Result = node;
        }

        private void OnExit(object sender, EventArgs e)
        {
            throw new NotImplementedException();
        }

        private void OnOpen(object sender, EventArgs e)
        {
            var ff = new CommonOpenFileDialog();
            ff.IsFolderPicker = true;
            ff.Title = "Please Select Folder";
            if (ff.ShowDialog() == CommonFileDialogResult.Ok)
            {
                SetRootDir(ff.FileName);
            }
        }

        private void SetRootDir(string fileName)
        {
            if (rootDir != fileName)
            {
                rootDir = fileName;
                tree.Nodes.Clear();
                var di = new DirectoryInfo(rootDir);
                rootNode = new MyNode(di);
                bg.RunWorkerAsync(rootNode);
            }
        }

        class MyNode
        {
            private long _size = 0;

            public string name;
            public List<MyNode> nodes;
            public long Size
            {
                get => _size;
                set
                {
                    if (_size != value)
                    {
                        // update parent size
                        if (parent != null)
                        {
                            parent.Size += value - _size;
                        }
                    }
                    _size = value;
                }
            }
            public MyNode parent;
            public DirectoryInfo di;

            public MyNode(DirectoryInfo di)
            {
                this.di = di;
                name = di.Name;
            }
            public MyNode(FileInfo fi)
            {
                name = fi.Name;
                isFile = true;
            }
            public bool isFile = false;
            public TreeNode treeNode;
            public string Label => $"{this.name} {SizeToString(Size)}";

        }

        static private string SizeToString(long size)
        {
            return size > GB ? $"{size / GB}GB" :
                size > MB ? $"{size / MB}MB" :
                size > KB ? $"{size / KB}KB" : $"{size}B";
        }
        const long KB = 1024;
        const long MB = 1024 * KB;
        const long GB = 1024 * MB;
    }
}
