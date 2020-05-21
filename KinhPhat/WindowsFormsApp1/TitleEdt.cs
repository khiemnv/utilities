#define header_blue

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Data.OleDb;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace WindowsFormsApp1
{
    public partial class TitleEdt : Form
    {
        public EditPanel m_edtPanel;
        public TitleEdt(bool showCmb = false)
        {
            m_edtPanel = new EditPanel(showCmb);
            this.Controls.Add(m_edtPanel.m_tlp);
        }
    }

    public class EditPanel
    {
        public TableLayoutPanel m_tlp;
        public List<MyTitle> m_titles;
        public MyTitle m_title;
        public string m_cnnStr { get { return ConfigMng.getInstance().m_cnnInfo.cnnStr; } }
        public lContentProvider m_content { get { return ConfigMng.getInstance().m_content; } }
        public ComboBox m_titlesCmb;
        public Label m_titleLbl;
        public Button m_closeBtn;
        public EditPanel(bool showCmb = false)
        {
            TableLayoutPanel tlp = new TableLayoutPanel
            {
                Dock = DockStyle.Fill,
                CellBorderStyle = TableLayoutPanelCellBorderStyle.None
            };
            m_tlp = tlp;
            tlp.RowStyles.Add(new RowStyle() { SizeType = SizeType.Absolute, Height = 35 });
            tlp.RowStyles.Add(new RowStyle() { SizeType = SizeType.Percent, Height = 100 });
            tlp.RowStyles.Add(new RowStyle() { SizeType = SizeType.Absolute, Height = 35 });
            //row1
            var titleLbl = new Label
            {
                Text = "Title",
                AutoSize = true,
                Margin = new Padding(0, 5, 0, 0),
                Visible = false
            };
            m_titleLbl = titleLbl;
            var titleCmb = new ComboBox();
            titleCmb.Visible = false;
            m_titlesCmb = titleCmb;
            var spn = new FlowLayoutPanel
            {
                AutoSize = true,
                Anchor = AnchorStyles.Right,
                FlowDirection = FlowDirection.LeftToRight
            };
            var closeBtn = new Button()
            {
                Text = "close",
                AutoSize = true,
            };
            m_closeBtn = closeBtn;
            closeBtn.Click += CloseBtn_Click;
            spn.Controls.AddRange(new Control[] { m_titleLbl, m_titlesCmb, closeBtn });

            //show cmb
            if (showCmb) {
                spn.Anchor = AnchorStyles.Left;
                m_titlesCmb.Visible = true;
                m_titleLbl.Visible = true;
                m_closeBtn.Visible = false;
            }

            tlp.Controls.Add(spn, 0, 0);
            var dgv = new DataGridView
            {
                Dock = DockStyle.Fill
            };
            tlp.Controls.Add(dgv, 0, 1);
            var saveBtn = new Button
            {
                AutoSize = true,
                Text = "submit",
            };
            saveBtn.Click += SaveBtn_Click;
            var cancelBtn = new Button
            {
                AutoSize = true,
                Text = "reload"
            };
            cancelBtn.Click += CancelBtn_Click;
            var spn2 = new FlowLayoutPanel
            {
                AutoSize = true,
                FlowDirection = FlowDirection.LeftToRight,
                Anchor = AnchorStyles.Right
            };
            spn2.Controls.AddRange(new Control[] { saveBtn, cancelBtn });
            tlp.Controls.Add(spn2, 0, 2);


            //data
            m_dataTable = new DataTable();
            m_dataTable.TableNewRow += M_dataTable_TableNewRow ;
            m_bindingSource = new BindingSource();
            m_bindingSource.DataSource = m_dataTable;
            dgv.DataSource = m_bindingSource;
            m_dataGridView = dgv;
            dgv.AutoGenerateColumns = false;
            dgv.DataError += DataGridView1_DataError;
            dgv.CellParsing += OnCellParsing;
            dgv.CellFormatting += Dgv_CellFormatting;
            dgv.RowsAdded += Dgv_RowsAdded;
            m_tblInfo = new ParagraphsTblInfo();
            crtColumns();
        }

        private void M_dataTable_TableNewRow(object sender, DataTableNewRowEventArgs e)
        {
            Debug.Assert(e.Row.Table.Rows.Count > 0, "exiting table not empty");
            DataRow row = e.Row;
            DataRow lastRow = row.Table.Rows[row.Table.Rows.Count - 1];
            row["titleId"] = lastRow["titleId"];
            row["ord"] = (short)lastRow["ord"] + 1;
            //copy above row style
            for (int i = 3; i < 8; i++)
            {
                row[i] = lastRow[i];
            }
        }

        private void Dgv_RowsAdded(object sender, DataGridViewRowsAddedEventArgs e)
        {
            //throw new NotImplementedException();
        }

        public event EventHandler<EventArgs> OnHideEditor;
        private void CloseBtn_Click(object sender, EventArgs e)
        {
            OnHideEditor?.Invoke(sender, e);
        }

        private void TitleCmb_SelectedValueChanged(object sender, EventArgs e)
        {
            var obj = m_titlesCmb.SelectedValue;
            m_title.ID = Convert.ToUInt64(obj);
            loadTitle();
        }


        public void updateCmb()
        {
            var w = 0;
            if (m_titles == null)
            {
                m_titlesCmb.DataSource = m_content.getTitles();
            }
            else
            {
                m_titlesCmb.DataSource = m_titles.Select(value => new { value.ID, value.zPath }).ToList();
            }
            m_titlesCmb.DisplayMember = "zPath";
            m_titlesCmb.ValueMember = "ID";

            //int selected id
            m_titlesCmb.SelectedValue = m_title.ID;

            //calculate cmb width
            foreach (var d in m_titlesCmb.Items)
            {
                string txt = "";
                if (d.GetType() != typeof(DataRowView))
                    txt = (string)d.GetType().GetProperty("zPath").GetValue(d, null);
                else
                    txt = (string)((DataRowView)d)[1];

                Size size = TextRenderer.MeasureText(txt, m_titlesCmb.Font);
                w = Math.Max(w, size.Width);
            }
            m_titlesCmb.Width = w;
            var spanSize = m_titlesCmb.Parent.PreferredSize;
            var formSize = m_tlp.Size;
            formSize.Width = spanSize.Width + 30;
            m_tlp.MinimumSize = formSize;

            //init event
            m_titlesCmb.SelectedValueChanged += TitleCmb_SelectedValueChanged;
        }

        private void CancelBtn_Click(object sender, EventArgs e)
        {
            loadTitle();
        }

        internal void clear()
        {
            m_dataTable.Clear();
        }

        private void SaveBtn_Click(object sender, EventArgs e)
        {
            m_content.UpdateTitle((DataTable)m_bindingSource.DataSource);
        }

        public BindingSource m_bindingSource;
        public DataTable m_dataTable;
        DataGridView m_dataGridView;
        lTableInfo m_tblInfo;
        public void loadTitle()
        {
            m_content.getTitleParagraphs(m_title.ID, m_dataTable);
            updateCols();
        }

        private void crtColumns()
        {
            int i = 0;
            foreach (var field in m_tblInfo.m_cols)
            {
                DataGridViewColumn dgvcol = crtColumns(field);
                i = m_dataGridView.Columns.Add(dgvcol);
                dgvcol.HeaderText = field.m_alias;
                dgvcol.Name = field.m_field;
                dgvcol.DataPropertyName = field.m_field;
                //show hide col
                dgvcol.Visible = field.m_visible;
            }
            //last columns
            var lastCol = m_dataGridView.Columns[i - 1];
            lastCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;
            lastCol.FillWeight = 1;
        }

        public enum alignment
        {
            left,
            center,
            right
        }

        private DataGridViewColumn crtColumns(lTableInfo.lColInfo field)
        {
            if (field.m_type == lTableInfo.lColInfo.lColType.boolean)
            {
                var col = new DataGridViewCheckBoxColumn();
                col.ValueType = typeof(Int16);
                return col;
            }
            else if (field.m_type == lTableInfo.lColInfo.lColType.map)
            {
                var col = new DataGridViewComboBoxColumn();
                col.DisplayMember = "name";
                col.ValueMember = "val";
#if true
                col.DataSource = field.m_mapSrc;
#else
                col.DataSource = new alignment[]
                    { alignment.left, alignment.center,alignment.right }
                    .Select(value => new { name = value.ToString(), val = (Int16)value })
                    .ToList();
#endif
                return col;
            }
            else
            {
                var col = new DataGridViewTextBoxColumn();
                return col;
            }
        }

        private void updateCols()
        {
            m_dataGridView.Columns[0].Visible = false;
            var tblInfo = new ParagraphsTblInfo();
            int i = 1;
            for (; i < m_dataGridView.ColumnCount; i++)
            {
                //show hide columns
                if (tblInfo.m_cols[i].m_visible == false)
                {
                    m_dataGridView.Columns[i].Visible = false;
                    continue;
                }

                m_dataGridView.Columns[i].HeaderText = tblInfo.m_cols[i].m_alias;

#if header_blue
                //header color blue
                m_dataGridView.Columns[i].HeaderCell.Style.BackColor = Color.Blue;
                m_dataGridView.Columns[i].HeaderCell.Style.ForeColor = Color.White;
                m_dataGridView.Columns[i].Width = 50;
#endif
            }
            m_dataGridView.Columns[i - 2].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;
            m_dataGridView.Columns[i - 2].FillWeight = 1;
            m_dataGridView.Columns[i - 2].MinimumWidth = 100;
        }
        private void DataGridView1_DataError(object sender, DataGridViewDataErrorEventArgs anError)
        {
            MessageBox.Show("Error happened " + anError.Context.ToString());

            if (anError.Context == DataGridViewDataErrorContexts.Commit)
            {
                MessageBox.Show("Commit error");
            }
            if (anError.Context == DataGridViewDataErrorContexts.CurrentCellChange)
            {
                MessageBox.Show("Cell change");
            }
            if (anError.Context == DataGridViewDataErrorContexts.Parsing)
            {
                MessageBox.Show("parsing error");
            }
            if (anError.Context == DataGridViewDataErrorContexts.LeaveControl)
            {
                MessageBox.Show("leave control error");
            }

            if ((anError.Exception) is ConstraintException)
            {
                DataGridView view = (DataGridView)sender;
                view.Rows[anError.RowIndex].ErrorText = "an error";
                view.Rows[anError.RowIndex].Cells[anError.ColumnIndex].ErrorText = "an error";

                anError.ThrowException = false;
            }
        }

        private void DataGridView1_CellValuePushed(object sender,
            DataGridViewCellValueEventArgs e)
        {
            if (IsCheckBoxColumn(e.ColumnIndex))
            {
                Debug.Assert(false);
            }
            else if (IsComboBoxColumn(e.ColumnIndex))
            {
                Debug.Assert(false);
            }
        }
        private bool IsCheckBoxColumn(int columnIndex)
        {
            return m_tblInfo.m_cols[columnIndex].m_type == lTableInfo.lColInfo.lColType.boolean;
        }
        private bool IsComboBoxColumn(int columnIndex)
        {
            return m_tblInfo.m_cols[columnIndex].m_type == lTableInfo.lColInfo.lColType.map;
        }
        private void DataGridView1_CellValueNeeded(object sender,
            DataGridViewCellValueEventArgs e)
        {
            if (IsCheckBoxColumn(e.ColumnIndex))
            {
                Debug.Assert(false);
                //e.Value = true;
            }
            else if (IsComboBoxColumn(e.ColumnIndex))
            {
                Debug.Assert(false);
                //do nothing
            }
        }
        void OnCellParsing(object sender, DataGridViewCellParsingEventArgs e)
        {
            if (IsCheckBoxColumn(e.ColumnIndex))
            {
                e.ParsingApplied = true;
                e.Value = (bool)e.Value ? (Int16)(-1) : (Int16)0;
            }
            else if (IsComboBoxColumn(e.ColumnIndex))
            {
                //do nothing
            }
        }

        private void Dgv_CellFormatting(object sender, DataGridViewCellFormattingEventArgs e)
        {
            if (IsComboBoxColumn(e.ColumnIndex))
            {
                //do nothing
                //foreach (DataRow row in m_tblInfo.m_cols[e.ColumnIndex].m_mapSrc.Rows)
                //{
                //    if (row["val"].ToString() == e.Value.ToString())
                //    {
                //        //e.Value =row["name"].ToString();
                //        //e.FormattingApplied = true;
                //    }
                //}
            }
        }
    }
}
