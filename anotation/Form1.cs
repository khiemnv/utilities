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
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml.Linq;

namespace annotation
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
        private PictureBox pb;

        public Form1()
        {
            InitializeComponent();

            // split
            var spt = new SplitContainer();
            spt.Dock = DockStyle.Fill;

            // tree
            tree = new TreeView();
            tree.AllowDrop = true;
            tree.Dock = DockStyle.Fill;
            spt.Panel1.Controls.Add(tree);
            tree.NodeMouseClick += Tree_NodeMouseClick;

            tree.ItemDrag += new ItemDragEventHandler(tree_ItemDrag);
            tree.DragEnter += new DragEventHandler(tree_DragEnter);
            tree.DragOver += new DragEventHandler(tree_DragOver);
            tree.DragDrop += new DragEventHandler(tree_DragDrop);


            // pb
            pb = new PictureBox();
            pb.Dock = DockStyle.Fill;
            pb.SizeMode = PictureBoxSizeMode.Zoom;
            spt.Panel2.Controls.Add(pb);

            // menu
            // Convert
            var convMenuStrip = new ContextMenuStrip();
            convMenuStrip.Items.Add("Labelme2yolo").Click += (s, e) =>
            {
                var ff = new CommonOpenFileDialog();
                ff.IsFolderPicker = true;
                ff.Title = "Please Select Folder";
                if (ff.ShowDialog() == CommonFileDialogResult.Ok)
                {
                    var outDir = Labelme2yolo(ff.FileName);
                    SetRootDir(outDir);
                }
            };

            // File
            var fileContextMenuStrip = new ContextMenuStrip();
            fileContextMenuStrip.Items.Add("Open").Click += OnOpen;
            fileContextMenuStrip.Items.Add("-");
            fileContextMenuStrip.Items.Add("Exit").Click += OnExit;
            var tsmi = new ToolStripMenuItem
            {
                Text = "ObjectView",
                Checked = BaseConfig.GetBaseConfig().ObjectViewMode,
                CheckOnClick = true
            };
            tsmi.CheckedChanged += (s, e) =>
            {
                // sw view
                swViewMode(tsmi.Checked);

                // save cfg
                var b = tsmi.Checked;
                var baseConfig = BaseConfig.GetBaseConfig();
                baseConfig.ObjectViewMode = b;
                BaseConfig.WriteBaseConfig(baseConfig);
            };
            fileContextMenuStrip.Items.Add(tsmi);


            var file = new ToolStripMenuItem("File")
            {
                DropDown = fileContextMenuStrip
            };
            var conv = new ToolStripMenuItem("Tools")
            {
                DropDown = convMenuStrip
            };
            var ms = new MenuStrip();
            ms.Items.Add(file);
            ms.Items.Add(conv);

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
        private string Labelme2yolo(string inputDir)
        {
            // inputDir:
            //   train
            //   val
            //   dict.txt
            // ouputDir: data
            //   images
            //     train
            //     val
            //   labels
            //     train
            //     val

            var di = new DirectoryInfo(inputDir);
            var dict = new Dictionary<string, int>();
            var outDir = Path.Combine(inputDir, "YOLODataset");

            // read label dict
            var dictF = di.GetFiles("dict.txt").FirstOrDefault();
            if (dictF != null) { 
                var labelDict = ReadLabelDict(dictF.FullName);
                labelDict.ToList().ForEach(p => dict.Add(p.Value,p.Key));
            }

            // create output dir
            di.GetDirectories().Where(s => s.Name != "YOLODataset").ToList().ForEach(sub =>
            {
                var imgSD = Directory.CreateDirectory(Path.Combine(outDir, "images", sub.Name));
                var labelSD = Directory.CreateDirectory(Path.Combine(outDir, "labels", sub.Name));
                foreach (var img in sub.GetFiles("*.jpeg"))
                {
                    var label = Path.Combine(sub.FullName, Path.GetFileNameWithoutExtension(img.Name) + ".json");
                    var txt = File.ReadAllText(label);
                    Labelme data = File.ReadAllText(label).FromJson<Labelme>();
                    var lines = data.shapes.ConvertAll(shape =>
                    {
                        if (!dict.ContainsKey(shape.label)) { dict.Add(shape.label, dict.Count); }
                        var idx = dict[shape.label];
                        var line = $"{idx} " + string.Join(" ", shape.points.ConvertAll(p => $"{p[0] / data.imageWidth} {p[1] / data.imageHeight}"));
                        return line;
                    });

                    File.WriteAllLines(Path.Combine(labelSD.FullName, Path.GetFileNameWithoutExtension(img.Name) + ".txt"), lines);
                    File.Copy(img.FullName, Path.Combine(imgSD.FullName, img.Name), true);
                }
            });

            // create dataset.yaml
            var path = outDir.Replace("\\", "/");
            var tmpl = $"path: {path}\r\ntrain: images/train\r\nval: images/val\r\ntest: images/test\r\nnames:\r\n";
            var cfgFile = Path.Combine(outDir, "dataset.yaml");
            File.WriteAllText(cfgFile, tmpl + string.Join("\r\n", dict.ToList().ConvertAll(p =>$"  {p.Value}: {p.Key}")));

            return outDir;
        }

        private void tree_ItemDrag(object sender, ItemDragEventArgs e)
        {
            // Move the dragged node when the left mouse button is used.
            if (e.Button == MouseButtons.Left)
            {
                DoDragDrop(e.Item, DragDropEffects.Move);
            }

            // Copy the dragged node when the right mouse button is used.
            else if (e.Button == MouseButtons.Right)
            {
                DoDragDrop(e.Item, DragDropEffects.Copy);
            }
        }

        // Set the target drop effect to the effect 
        // specified in the ItemDrag event handler.
        private void tree_DragEnter(object sender, DragEventArgs e)
        {
            e.Effect = e.AllowedEffect;
        }

        // Select the node under the mouse pointer to indicate the 
        // expected drop location.
        private void tree_DragOver(object sender, DragEventArgs e)
        {
            // Retrieve the client coordinates of the mouse position.
            Point targetPoint = tree.PointToClient(new Point(e.X, e.Y));

            // Select the node at the mouse position.
            var node = tree.GetNodeAt(targetPoint);
            MyNode data = (MyNode)node.Tag;
            if (data != null && !data.isFile)
            {
                tree.SelectedNode = tree.GetNodeAt(targetPoint);
                e.Effect = DragDropEffects.Move;
            }
            else
            {
                e.Effect = DragDropEffects.None;
            }
        }

        private void tree_DragDrop(object sender, DragEventArgs e)
        {
            // Retrieve the client coordinates of the drop location.
            Point targetPoint = tree.PointToClient(new Point(e.X, e.Y));

            // Retrieve the node at the drop location.
            TreeNode targetNode = tree.GetNodeAt(targetPoint);

            // Retrieve the node that was dragged.
            TreeNode draggedNode = (TreeNode)e.Data.GetData(typeof(TreeNode));

            // Confirm that the node at the drop location is not 
            // the dragged node or a descendant of the dragged node.
            if (!draggedNode.Equals(targetNode) && !ContainsNode(draggedNode, targetNode))
            {
                // If it is a move operation, remove the node from its current 
                // location and add it to the node at the drop location.
                if (e.Effect == DragDropEffects.Move)
                {
                    //draggedNode.Remove();
                    //targetNode.Nodes.Add(draggedNode);

                    // move image
                    MoveImage((MyNode)draggedNode.Tag, (MyNode)targetNode.Tag);
                    RenderTree(rootNode);
                }

                // If it is a copy operation, clone the dragged node 
                // and add it to the node at the drop location.
                else if (e.Effect == DragDropEffects.Copy)
                {
                    targetNode.Nodes.Add((TreeNode)draggedNode.Clone());
                }

                // Expand the node at the location 
                // to show the dropped node.
                targetNode.Expand();
            }
        }

        private void MoveImage(MyNode imgN, MyNode dirN)
        {
            if (!imgN.isFile || dirN.isFile) { return; }

            var parent = imgN.parent;
            var src = Path.Combine(parent.di.FullName, imgN.name);
            var des = Path.Combine(dirN.di.FullName, imgN.name);
            File.Move(src, des);

            if (imgN.state == 1)
            {
                // move .txt
                var src2 = GetLabelFile(src);
                var des2 = GetLabelFile(des);
                File.Move(src2, des2);
            }

            parent.nodes.Remove(imgN);
            dirN.nodes.Add(imgN);
            imgN.parent = dirN;
        }

        private string GetLabelFile(string imgPath)
        {
            var labelD = ReplaceLastOccurrence(Path.GetDirectoryName(imgPath), "images", "labels");
            return Path.Combine(labelD, Path.GetFileNameWithoutExtension(imgPath) + ".txt");
        }
        public static string ReplaceLastOccurrence(string source, string find, string replace)
        {
            int place = source.LastIndexOf(find);

            if (place == -1)
                return source;

            return source.Remove(place, find.Length).Insert(place, replace);
        }

        private bool ContainsNode(TreeNode node1, TreeNode node2)
        {
            // Check the parent node of the second node.
            if (node2.Parent == null) return false;
            if (node2.Parent.Equals(node1)) return true;

            // If the parent node is not null or equal to the first node, 
            // call the ContainsNode method recursively using the parent of 
            // the second node.
            return ContainsNode(node1, node2.Parent);
        }

        private void swViewMode(bool @checked)
        {
            throw new NotImplementedException();
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
                    if (n.isFile)
                    {
                        var f = Path.Combine(n.parent.di.FullName, n.name);
                        switch (Path.GetExtension(n.name).ToLower())
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
                    var f = (n.isFile) ? n.parent.di.FullName : n.di.FullName;
                    Process.Start(f);
                };
                base.ContextMenuStrip.Items.Add("Refresh").Click += (_s, _e) =>
                {
                    Refresh(n);
                };
                tree.SelectedNode = e.Node;
            }
            else if (e.Button == MouseButtons.Left)
            {
                var n = (MyNode)e.Node.Tag;
                if (n.isFile)
                {
                    var path = Path.Combine(n.parent.di.FullName, n.name);
                    var bmp = FromFile(path);
                    if (n.state == 1)
                    {
                        var lPath = Path.Combine(n.parent.di.FullName.Replace("images", "labels"), Path.GetFileNameWithoutExtension(n.name) + ".txt");
                        var lines = File.ReadAllLines(lPath);
                        var font = new Font("Arial", 50);
                        lines.ToList().ForEach(line =>
                        {
                            var arr = line.Split();
                            if (arr.Length == 5)
                            {
                                //index x_center y_center width height
                                var x_center = float.Parse(arr[1]) * bmp.Width;
                                var y_center = float.Parse(arr[2]) * bmp.Height;
                                var width = float.Parse(arr[3]) * bmp.Width;
                                var height = float.Parse(arr[4]) * bmp.Height;
                                var name = labelConfig._dict[int.Parse(arr[0])];
                                var xmin = x_center - width / 2;
                                var ymin = y_center - height / 2;
                                var xmax = x_center + width / 2;
                                var ymax = y_center + height / 2;
                                var obj = new ImgObject(name, xmin, ymin, xmax, ymax);
                                var color = getCorlor(name);
                                Pen pen = new Pen(color, 12);
                                // draw label in original images
                                using (Graphics G = Graphics.FromImage(bmp))
                                {
                                    int w = obj.width;
                                    int h = obj.height;
                                    G.DrawRectangle(pen, new Rectangle((int)obj.xmin, (int)obj.ymin, w, h));
                                    G.DrawString(obj.Name, font, color, (int)obj.xmin, (int)obj.ymin);
                                }
                            }
                            else
                            {
                                var points = new List<PointF>();
                                for (int i = 1; i < arr.Length; i += 2)
                                {
                                    points.Add(new PointF(float.Parse(arr[i]) * bmp.Width, float.Parse(arr[i + 1]) * bmp.Height));
                                }

                                var name = labelConfig._dict[int.Parse(arr[0])];
                                var color = getCorlor(name);
                                Pen pen = new Pen(color, 12);
                                // draw label in original images
                                using (Graphics G = Graphics.FromImage(bmp))
                                {
                                    G.DrawPolygon(pen, points.ToArray());
                                    G.DrawString(name, font, color, points[0]);
                                }
                            }
                        });
                    }

                    pb.Image = bmp;
                }
            }
        }
        public static Bitmap FromFile(string path)
        {
            var bytes = File.ReadAllBytes(path);
            var ms = new MemoryStream(bytes);
            var img = Image.FromStream(ms);
            return (Bitmap)img;
        }

        class ImgObject
        {
            public string Name { get; set; }
            public Double xmin { get; set; }
            public Double ymin { get; set; }
            public Double xmax { get; set; }
            public Double ymax { get; set; }
            //public Vector2D CenterPos;
            public int width { get; set; }
            public int height { get; set; }
            public int percent { get; set; }
            public ImgObject(string name, Double xmin, Double ymin, Double xmax, Double ymax, int percent)
            {
                this.Name = name;
                this.xmin = xmin;
                this.ymin = ymin;
                this.xmax = xmax;
                this.ymax = ymax;
                this.width = (int)(xmax - xmin);
                this.height = (int)(ymax - ymin);
                this.percent = percent;
                //CenterPos = new Vector2D((xmin + xmax) / 2, (ymin + ymax) / 2);
            }
            public ImgObject(string name, Double xmin, Double ymin, Double xmax, Double ymax)
            {
                this.Name = name;
                this.xmin = xmin;
                this.ymin = ymin;
                this.xmax = xmax;
                this.ymax = ymax;
                this.width = (int)(xmax - xmin);
                this.height = (int)(ymax - ymin);
                //CenterPos = new Vector2D((xmin + xmax) / 2, (ymin + ymax) / 2);
            }
        }


        LabelConfig labelConfig = new LabelConfig();
        private Brush getCorlor(string name)
        {
            if (labelConfig.d == null)
            {
                var solidColorBrushList = new List<Brush>()
                {
                     new SolidBrush(Color.FromArgb(255,27,161,226)),
                     new SolidBrush(Color.FromArgb(255,160,80,0)),
                     new SolidBrush(Color.FromArgb(255, 51, 153, 51)),
                     new SolidBrush(Color.FromArgb(255, 162, 193, 57)),
                     new SolidBrush(Color.FromArgb(255, 216, 0, 115)),
                     new SolidBrush(Color.FromArgb(255, 240, 150, 9)),
                     new SolidBrush(Color.FromArgb(255, 230, 113, 184)),
                     new SolidBrush(Color.FromArgb(255, 162, 0, 255)),
                     new SolidBrush(Color.FromArgb(255, 229, 20, 0)),
                     new SolidBrush(Color.FromArgb(255, 0, 171, 169))
                };
                labelConfig.d = new Dictionary<string, Brush>();
                labelConfig._dict.ToList().ForEach(p => labelConfig.d[p.Value] = solidColorBrushList[(int)p.Key % solidColorBrushList.Count]);
                labelConfig._dict2.ToList().ForEach(p => labelConfig.d[p.Value] = labelConfig.d[p.Key]);
            }


            if (labelConfig.d.ContainsKey(name))
                return labelConfig.d[name];

            return Brushes.Red;
        }
        private Brush getCorlor(ImgObject imgObject)
        {
            return getCorlor(imgObject.Name);
        }
        private void Refresh(MyNode n)
        {
            //var f = (n.isFile) ? n.parent.di.FullName : n.di.FullName;
            //n.nodes.Clear();
            //n.treeNode.Nodes.Clear();
            //n.Size = 0;
            bg.RunWorkerAsync(rootNode);
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
            RenderTree(node);

            // stop progress
            SetProgressBar(node.Label, 0);
        }

        private void RenderTree(MyNode node)
        {
            // clear
            tree.Nodes.Clear();
            // create root node
            var st = new List<(MyNode, TreeNode)> { (node, new TreeNode(node.Label) { Tag = node }) };
            // add root treeNode
            tree.Nodes.Add(st[0].Item2);

            while (st.Count > 0)
            {
                var cur = st[0].Item1;
                var curTreeNode = st[0].Item2;
                st.RemoveAt(0);
                if (cur.nodes != null)
                {
                    // create treeNode
                    st.InsertRange(0, cur.nodes.ConvertAll(n =>
                    {
                        var newTreeNode = new TreeNode(n.Label) { Tag = n };
                        curTreeNode.Nodes.Add(newTreeNode);
                        return (n, newTreeNode);
                    }));
                }
            }

            // expand parent
            tree.ExpandAll();
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
                }
                else
                {
                    cur.treeNode = cur.parent.treeNode.Nodes.Add(cur.Label);
                }
                cur.treeNode.Tag = cur;
            }
        }

        class MyImg
        {
            public string path;

            public bool hasLabel;
        }
        class MyData
        {
            public string path; // train|val
            public List<MyImg> images;
        }
        private List<MyData> ReadData(string dir)
        {
            // C:\work\investigate\yolov5\datasets\dd\data607
            // images
            //   train
            //   val
            // labels
            //   train
            //   val
            var imagesD = new DirectoryInfo(Path.Combine(dir, "images"));
            var lst = imagesD.GetDirectories().ToList().ConvertAll(
            di =>
            {
                var newData = new MyData { path = di.FullName };
                var files = di.GetFiles("*.*").Where(f => Regex.IsMatch(f.Extension, "^\\.(jpeg|png)$", RegexOptions.IgnoreCase)).ToList();
                var labelD = di.FullName.Replace("images", "labels");
                var h = Directory.Exists(labelD) ? new DirectoryInfo(labelD).GetFiles("*.txt").Select(f => Path.GetFileNameWithoutExtension(f.Name)).ToHashSet() : new HashSet<string>();
                newData.images = files.ToList().ConvertAll((file) => new MyImg
                {
                    path = file.FullName,
                    hasLabel = h.Contains(Path.GetFileNameWithoutExtension(file.Name))
                });
                return newData;
            });

            // read dict
            labelConfig = ReadLabelConfig(dir);

            return lst;
        }

        private LabelConfig ReadLabelConfig(string dir)
        {
            var labelConfig = new LabelConfig();
            var cfg = new DirectoryInfo(dir).GetFiles("*.yaml").FirstOrDefault();
            if (cfg != null)
            {
                labelConfig._dict = ReadLabelDict(cfg.FullName);
            }
            return labelConfig;
        }

        private Dictionary<int, string> ReadLabelDict(string path)
        {
            var dict = new Dictionary<int, string>();
            var lines = File.ReadAllLines(path);
            var s = 0;
            lines.ToList().ForEach(line =>
            {
                switch (s)
                {
                    case 0:
                        if (Regex.IsMatch(line, "names:"))
                        {
                            s = 1;
                        }
                        break;
                    case 1:
                        var arr = line.Split(new char[] { ' ', ':' }, StringSplitOptions.RemoveEmptyEntries);
                        dict.Add(int.Parse(arr[0]), arr[1]);
                        break;
                }
            });
            return dict;
        }

        private void OnWork(object sender, DoWorkEventArgs e)
        {
            var root = (MyNode)e.Argument;
            var lst = ReadData(root.di.FullName);
            root.nodes = lst.ConvertAll(data =>
            {
                var n = new MyNode(new DirectoryInfo(data.path)) { parent = root };
                n.nodes = data.images.ConvertAll(img => new MyNode(new FileInfo(img.path)) { parent = n, Size = 1, state = img.hasLabel ? 1 : 0 });
                return n;
            });
            e.Result = root;
        }


        private void OnWork2(object sender, DoWorkEventArgs e)
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
            public int state;
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
            public string Label => $"{this.name} {(isFile ? state.ToString() : nodes.Count.ToString())}";

        }

        static private string SizeToString(long size)
        {
            return size.ToString("N0");
            return size > GB ? $"{size / GB}GB" :
                size > MB ? $"{size / MB}MB" :
                size > KB ? $"{size / KB}KB" : $"{size}B";
        }
        const long KB = 1024;
        const long MB = 1024 * KB;
        const long GB = 1024 * MB;
    }

    public class LabelConfig
    {
        public IDictionary<int, string> _dict = new Dictionary<int, string>();
        public IDictionary<string, string> _dict2 = new Dictionary<string, string>();
        public Dictionary<string, Brush> d;
    }
}
