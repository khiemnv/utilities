#define col_class
#define use_cmd_params

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
    class DataContent
    {
    }

    [DataContract(Name = "TableInfo")]
    public class lTableInfo
    {
        //#define col_class
#if col_class
        [DataContract(Name = "ColInfo")]
        public class lColInfo
        {
            [DataContract(Name = "ColType")]
            public enum lColType
            {
                [EnumMember]
                text,
                [EnumMember]
                dateTime,
                [EnumMember]
                num,
                [EnumMember]
                currency,
                [EnumMember]
                uniqueText,
                [EnumMember]
                map,
                [EnumMember]
                boolean,
            };
            [DataMember(Name = "field", EmitDefaultValue = false)]
            public string m_field;
            [DataMember(Name = "alias", EmitDefaultValue = false)]
            public string m_alias;
            [DataMember(Name = "lookupTbl", EmitDefaultValue = false)]
            public string m_lookupTbl;
            [DataMember(Name = "type", EmitDefaultValue = false)]
            public lColType m_type;
            [DataMember(Name = "visible", EmitDefaultValue = false)]
            public bool m_visible;

            public DataTable m_mapSrc;

            //public lDataSync m_lookupData;
            private void init(string field, string alias, lColType type, string lookupTbl, bool visible)
            {
                m_lookupTbl = lookupTbl;
                m_field = field;
                m_alias = alias;
                m_type = type;
                m_visible = visible;
            }
            public lColInfo(string field, string alias, lColType type, string lookupTbl, bool visible)
            {
                init(field, alias, type, lookupTbl, visible);
            }
            public lColInfo(string field, string alias, lColType type, string lookupTbl)
            {
                init(field, alias, type, lookupTbl, true);
            }
            public lColInfo(string field, string alias, lColType type)
            {
                init(field, alias, type, null, true);
            }
            public lColInfo(string field, string alias, DataTable mapSrc)
            {
                m_field = field;
                m_alias = alias;
                m_type = lColType.map;
                m_mapSrc = mapSrc;
                m_visible = true;
            }
        };

        [DataMember(Name = "cols", EmitDefaultValue = false)]
        public lColInfo[] m_cols;
        [DataMember(Name = "name", EmitDefaultValue = false)]
        public string m_tblName;
        [DataMember(Name = "alias", EmitDefaultValue = false)]
        public string m_tblAlias;
        [DataMember(Name = "crtSql", EmitDefaultValue = false)]
        public string m_crtQry;

        public virtual void LoadData()
        {
            foreach (lColInfo colInfo in m_cols)
            {
                if (colInfo.m_lookupTbl != null)
                {
                    //colInfo.m_lookupData = appConfig.s_contentProvider.CreateDataSync(colInfo.m_lookupTbl);
                    //colInfo.m_lookupData.LoadData();
                }
            }
        }
#else
        public struct lColInfo
        {
            public enum lColType
            {
                text,
                dateTime,
                num
            };
            public string m_field;
            public string m_alias;
            public lColType m_type;
        };
        public virtual lColInfo[] getColsInfo() { return null; }
#endif
        public int getColIndex(string colName)
        {
            int i = 0;
            foreach (lColInfo col in m_cols)
            {
                if (col.m_field == colName)
                {
                    return i;
                }
                i++;
            }
            return -1;
        }
    }

    [DataContract(Name = "TitleTblInfo")]
    public class TitleTblInfo : lTableInfo
    {
#if col_class
        public TitleTblInfo()
        {
            m_tblName = "titles";
            m_tblAlias = "Bài Kinh";
            m_crtQry = "CREATE TABLE if not exists  titles("
            + "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
            + "title char(255),"
            + "pathId INTEGER,"
            + "ord INTEGER,"
            + "note text"
            + ")";
            m_cols = new lColInfo[] {
                   new lColInfo( "ID","ID", lColInfo.lColType.num, null, false),
                   new lColInfo( "title","Tiêu đề", lColInfo.lColType.text),
                   new lColInfo( "pathId","pathId", lColInfo.lColType.num),
                   new lColInfo( "ord","Thứ tự", lColInfo.lColType.text),
                };
        }
#else
        static lColInfo[] m_cols = new lColInfo[] {
               new lColInfo() {m_field = "ID", m_alias = "ID", m_type = lColInfo.lColType.num }
            };
        public override lColInfo[] getColsInfo()
        {
            return m_cols;
        }
#endif

    };
    [DataContract(Name = "ParagraphsTblInfo")]
    public class ParagraphsTblInfo : lTableInfo
    {
        public ParagraphsTblInfo()
        {
            m_tblName = "paragraphs";
            m_tblAlias = "Đoạn Văn";
            m_crtQry = "CREATE TABLE if not exists paragraphs("
            + "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
            + "titleId INTEGER,"
            + "order INTEGER,"
            + "alignment INTEGER,"
            + "leftIndent INTEGER,"
            + "fontSize INTEGER,"
            + "fontBold INTEGER,"
            + "fontItalic INTEGER,"
            + "content text,"
            + "note text"
            + ")";
            
            m_cols = new lColInfo[] {
                   new lColInfo( "ID"        ,"ID"           , lColInfo.lColType.num, null, false),
                   new lColInfo( "titleId"   ,"titleId" , lColInfo.lColType.num, null,false),
                   new lColInfo( "ord"       ,"Thứ tự"   , lColInfo.lColType.num),
                   new lColInfo( "alignment" ,"alignment"    , getAlignmentMapSrc()),
                   new lColInfo( "leftIndent","left indent"  , lColInfo.lColType.num),
                   new lColInfo( "fontSize"  ,"size"    , lColInfo.lColType.num),
                   new lColInfo( "fontBold"  ,"bold"    , lColInfo.lColType.boolean),
                   new lColInfo( "fontItalic","italic"  , lColInfo.lColType.boolean),
                   new lColInfo( "content"   ,"Nội dung"     , lColInfo.lColType.text),
                   new lColInfo( "note"      ,"Ghi Chú"      , lColInfo.lColType.text),
                };
        }

        DataTable getAlignmentMapSrc()
        {
            DataTable dt = new DataTable();
            var col = dt.Columns.Add("name");
            col.DataType = typeof(string);
            col = dt.Columns.Add("val");
            col.DataType = typeof(Int16);
            var newRow = dt.NewRow();
            newRow[0] = "left";
            newRow[1] = 0;
            dt.Rows.Add(newRow);
            newRow = dt.NewRow();
            newRow[0] = "center";
            newRow[1] = 1;
            dt.Rows.Add(newRow);
            return dt;
        }
    };

    

    [DataContract(Name = "DbSchema")]
    public class lDbSchema
    {
        [DataMember(Name = "cnnStr")]
        public string m_cnnStr;
#if crt_qry
            [DataMember(Name ="crtTableSqls")]
            public List<string> m_crtTableSqls;
            [DataMember(Name = "crtViewSqls")]
            public List<string> m_crtViewSqls;
#endif
        [DataMember(Name = "tables")]
        public List<lTableInfo> m_tables;
        [DataMember(Name = "views")]
        public List<lTableInfo> m_views;

        public lDbSchema()
        {
        }
        protected void init()
        {
            m_tables = new List<lTableInfo>() {
                    new TitleTblInfo(),
                    new ParagraphsTblInfo()
                };
            m_views = new List<lTableInfo>()
            {
            };
        }
    }
    [DataContract(Name = "SqlDbSchema")]
    public class lSqlDbSchema : lDbSchema
    {
        public lSqlDbSchema()
        {
            m_cnnStr = @"Data Source=.\SQLEXPRESS;Initial Catalog=accounting;Integrated Security=True;MultipleActiveResultSets=True";
#if crt_qry
                m_crtTableSqls = new List<string> {
                    "CREATE TABLE if not exists  receipts("
                    + "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                    + "date datetime,"
                    + "receipt_number char(31),"
                    + "name char(31),"
                    + "content text,"
                    + "amount INTEGER,"
                    + "note text"
                    + ")",
                    "CREATE TABLE if not exists internal_payment("
                    + "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                    + "date datetime,"
                    + "payment_number char(31),"
                    + "name char(31),"
                    + "content text,"
                    + "group_name char(31),"
                    + "advance_payment INTEGER,"
                    + "reimbursement INTEGER,"
                    + "actually_spent INTEGER,"
                    + "note text"
                    + ")",
                    "CREATE TABLE if not exists external_payment("
                    + "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                    + "date datetime,"
                    + "payment_number char(31),"
                    + "name char(31),"
                    + "content text,"
                    + "group_name char(31),"
                    + "spent INTEGER,"
                    + "note text"
                    + ")",
                    "CREATE TABLE if not exists salary("
                    + "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                    + "month INTEGER,"
                    + "date datetime,"
                    + "payment_number char(31),"
                    + "name char(31),"
                    + "group_name char(31),"
                    + "content text,"
                    + "salary INTEGER,"
                    + "note text"
                    + ")",
                    "CREATE TABLE if not exists receipts_content(ID INTEGER PRIMARY KEY AUTOINCREMENT, content nchar(31));",
                    "CREATE TABLE if not exists group_name(ID INTEGER PRIMARY KEY AUTOINCREMENT, name nchar(31));",
            };
                m_crtViewSqls = new List<string> {
                    "CREATE VIEW if not exists v_receipts "
                    + " as "
                    + " select content, amount, cast(strftime('%Y', date) as integer) as year, (strftime('%m', date) + 2) / 3 as qtr "
                    + " from receipts"
                    + " where strftime('%Y', 'now') - strftime('%Y', date) between 0 and 4;" ,
                    " CREATE VIEW if not exists v_internal_payment"
                    + " as"
                    + " select group_name, actually_spent, cast(strftime('%Y', date) as integer) as year, (strftime('%m', date) + 2) / 3 as qtr"
                    + " from internal_payment"
                    + " where strftime('%Y', 'now') - strftime('%Y', date) between 0 and 4;",

                    "CREATE VIEW if not exists v_external_payment"
                    + " as"
                    + " select group_name, spent, cast(strftime('%Y', date) as integer) as year, (strftime('%m', date) + 2) / 3 as qtr"
                    + " from external_payment"
                    + " where strftime('%Y', 'now') - strftime('%Y', date) between 0 and 4;",
                    "CREATE VIEW if not exists v_salary"
                    + " as"
                    + " select group_name, salary, cast(strftime('%Y', date) as integer) as year, (strftime('%m', date) + 2) / 3 as qtr"
                    + " from salary"
                    + " where strftime('%Y', 'now') - strftime('%Y', date) between 0 and 4;",
                };
#endif  //crt_qry
            init();
        }
    }

    public interface IRefresher
    {
        void Refresh();
    }
    public class lDataContent : IDisposable
    {
        #region fetch_data
#if use_bg_work
        myWorker m_wkr;
#else
        public Form1 m_form;
#endif
        public DataTable m_dataTable { get; private set; }

        #region event
        public class FillTableCompletedEventArgs : EventArgs
        {
            public Int64 Sum { get; set; }
            public DateTime TimeComplete { get; set; }
        }
        static Dictionary<string, EventHandler<FillTableCompletedEventArgs>> m_dict =
            new Dictionary<string, EventHandler<FillTableCompletedEventArgs>>();
        private void addEvent(string zType, ref EventHandler<FillTableCompletedEventArgs> handler, EventHandler<FillTableCompletedEventArgs> value)
        {
            string key = zType + value.Target.ToString();
            if (!m_dict.ContainsKey(key))
            {
                m_dict.Add(key, value);
                handler += value;
            }
            else
            {
                handler -= m_dict[key];
                m_dict[key] = value;
                handler += value;
            }
        }


        #region update_complete_event
        private EventHandler<FillTableCompletedEventArgs> mFillTableCompleted;
        public event EventHandler<FillTableCompletedEventArgs> FillTableCompleted
        {
            add { addEvent("FillTableCompleted", ref mFillTableCompleted, value); }
            remove { }
        }
        protected virtual void OnFillTableCompleted(FillTableCompletedEventArgs e)
        {
            if (mFillTableCompleted != null) { mFillTableCompleted(this, e); }
        }
        private EventHandler<FillTableCompletedEventArgs> mUpdateTableCompleted;
        public event EventHandler<FillTableCompletedEventArgs> UpdateTableCompleted
        {
            add { addEvent("UpdateTableCompleted", ref mUpdateTableCompleted, value); }
            remove { }
        }
        protected virtual void OnUpdateTableCompleted(FillTableCompletedEventArgs e)
        {
            if (mUpdateTableCompleted != null) { mUpdateTableCompleted(this, e); }
        }
        #endregion //update_complete_event

        protected virtual void OnProgessCompleted(EventArgs e)
        {
            if (ProgessCompleted != null)
            {
                ProgessCompleted(this, e);
            }
        }
        public event EventHandler ProgessCompleted;
        #endregion

        protected virtual Int64 getMaxRowId() { return 0; }
        protected virtual Int64 getRowCount() { return 0; } //not used
        protected virtual void fillTable() { throw new NotImplementedException(); }
        protected virtual void updateTable() { throw new NotImplementedException(); }

        //require execute in form's thread
        void fetchLargeData()
        {
            Debug.Assert(!m_isView, "not fectch large data on view");   //rowid not exist


            OnFillTableCompleted(new FillTableCompletedEventArgs() { TimeComplete = DateTime.Now });

            if (m_refresher != null)
                m_refresher.Refresh();
        }
#if use_single_qry
        Int64 m_lastId = 0;
        private void M_tbl_RowChanged(object sender, DataRowChangeEventArgs e)
        {
            //Debug.WriteLine("{0}.M_tbl_RowChanged {1}", this, e.Row[0]);
            m_lastId = Math.Max(m_lastId, (Int64)e.Row[0]);
        }
        #region cursor
        public Int64 getPos()
        {
            return m_lastId;
        }
        public void setPos(Int64 pos) { m_lastId = pos; }
        string m_msgStatus;
        public void setStatus(string msg)
        {
            m_msgStatus = msg;
        }
        public string getStatus()
        {
            return m_msgStatus;
        }
        #endregion
#endif
        delegate void noParamDelegate();
        protected virtual void fetchData()
        {
            Debug.Assert(!m_isView, "not fectch data on view");
#if use_bg_work
            fetchLargeData();
#else
            //if (getMaxRowId() > 1000)
            //    invokeFetchLargeData();
            //else
            fetchSmallData();
#endif
        }
        void fetchSmallData()
        {
            DataTable table = m_dataTable;
            table.Clear();
            table.Locale = System.Globalization.CultureInfo.InvariantCulture;
            fillTable();

            OnFillTableCompleted(new FillTableCompletedEventArgs() { TimeComplete = DateTime.Now });

            if (m_refresher != null)
                m_refresher.Refresh();
        }
        #endregion
        public bool m_isView;
        public BindingSource m_bindingSource { get; private set; }
        protected string m_table;
        public IRefresher m_refresher;
        public lDataContent()
        {
            m_dataTable = new DataTable();
            m_bindingSource = new BindingSource();
            m_bindingSource.DataSource = m_dataTable;
        }
        protected void init(lTableInfo tbl)
        {
            if (m_dataTable.Columns.Count == 0)
            {
                //lTableInfo tbl = appConfig.s_config.getTable(m_table);
                if (tbl == null) return;

                //not need to init cols for views
                if (tbl.m_cols == null) return;

                foreach (var col in tbl.m_cols)
                {
                    DataColumn dc = m_dataTable.Columns.Add(col.m_field);
                    switch (col.m_type)
                    {
                        case lTableInfo.lColInfo.lColType.num:
                        case lTableInfo.lColInfo.lColType.currency:
                            dc.DataType = typeof(Int64);
                            break;
                        case lTableInfo.lColInfo.lColType.dateTime:
                            dc.DataType = typeof(DateTime);
                            break;
                    }
                }
            }

#if use_bg_work
            m_wkr = myWorker.getWorker();
#endif
        }
#if !use_cmd_params
        public virtual void Search(string exprs)
        {
            string sql = string.Format("select * from {0} ", m_table);
            if (exprs != null)
            {
                sql += " where " + exprs;
            }
            GetData(sql);
        }
#endif
#if use_cmd_params
        //public virtual void Search(List<string> exprs, List<lSearchParam> srchParams) { throw new NotImplementedException(); }
        //public virtual void AddRec(List<string> exprs, List<lSearchParam> srchParams) { throw new NotImplementedException(); }
#endif
        bool m_changed = true;
        public virtual void Load(bool isView) { throw new NotFiniteNumberException(); }
        public virtual void Load() { if (m_changed) { Reload(); } }
        public virtual void Reload() { m_changed = false; }
        public virtual void Submit()
        {
            m_changed = false;
#if use_bg_work
            updateTable();
#else
            Task delayUpdate = Task.Run(() =>
            {
                Thread.Sleep(100);
                this.m_form.Invoke(new noParamDelegate(() =>
                {
                    updateTable();
                    OnUpdateTableCompleted(new FillTableCompletedEventArgs() { TimeComplete = DateTime.Now });
                }));
            });
#endif
        }
        protected virtual void GetData(string sql) { throw new NotImplementedException(); }
        public virtual void BeginTrans() { }
        public virtual void EndTrans() { }
        #region dispose
        // Dispose() calls Dispose(true)  
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }
        // NOTE: Leave out the finalizer altogether if this class doesn't   
        // own unmanaged resources itself, but leave the other methods  
        // exactly as they are.   
        ~lDataContent()
        {
            // Finalizer calls Dispose(false)  
            Dispose(false);
        }
        // The bulk of the clean-up code is implemented in Dispose(bool)  
        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                m_bindingSource.Dispose();
                m_dataTable.Dispose();
            }
            // free native resources if there are any. 
        }
        #endregion
    }
    public class lContentProvider : IDisposable
    {
        protected lContentProvider()
        {
            //m_dataSyncs = new Dictionary<string, lDataSync>();
            //m_dataContents = new Dictionary<string, lDataContent>();
        }

        protected Form1 m_form;
        private Dictionary<string, lDataSync> m_dataSyncs;
        private Dictionary<string, lDataContent> m_dataContents;

        protected virtual lDataContent newDataContent(string tblName) { return null; }
        protected virtual lDataSync newDataSync(string tblName)
        {
            lDataContent dataContent = CreateDataContent(tblName);
            lDataSync dataSync = new lDataSync(dataContent);
            return dataSync;
        }

        public virtual DataTable GetData(string qry) { return null; }
        public lDataContent CreateDataContent(string tblName)
        {
            if (!m_dataContents.ContainsKey(tblName))
            {
                lDataContent data = newDataContent(tblName);
                m_dataContents.Add(tblName, data);
                return data;
            }
            else
            {
                return m_dataContents[tblName];
            }
        }
        public bool ReleaseDataContent(string tblName)
        {
            if (!m_dataContents.ContainsKey(tblName))
            {
                return false;
            }
            else
            {
                lDataContent data = m_dataContents[tblName];
                m_dataContents.Remove(tblName);
                data.Dispose();
                return true;
            }
        }
        public lDataSync CreateDataSync(string tblName)
        {
            if (!m_dataSyncs.ContainsKey(tblName))
            {
                lDataContent dataContent = CreateDataContent(tblName);
                lDataSync dataSync = new lDataSync(dataContent);
                m_dataSyncs.Add(tblName, dataSync);
                return dataSync;
            }
            else
            {
                return m_dataSyncs[tblName];
            }
        }

        public virtual object GetCnn() { throw new NotImplementedException(); }

        public virtual void getTitleParagraphs(UInt64 titleId, DataTable dataTable)
        {
            throw new NotImplementedException();
        }
        public virtual DataTable getTitles() { throw new NotImplementedException(); }
        public virtual void UpdateTitle(DataTable dt) { throw new NotImplementedException(); }
        public virtual void initCnn(string m_cnnStr) { throw new NotImplementedException(); }
        public virtual void closeCnn() { throw new NotImplementedException(); }
        public virtual List<MyTitle> getTitles2() { throw new NotImplementedException(); }
        public virtual List<MyParagraph> getTitleParagraphs(DataTable dt) { throw new NotImplementedException(); }
        public virtual List<MyParagraph> getTitleParagraphs(UInt64 titleId) { throw new NotImplementedException(); }
        #region dispose
        // Dispose() calls Dispose(true)  
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }
        // NOTE: Leave out the finalizer altogether if this class doesn't   
        // own unmanaged resources itself, but leave the other methods  
        // exactly as they are.   
        ~lContentProvider()
        {
            // Finalizer calls Dispose(false)  
            Dispose(false);
        }
        // The bulk of the clean-up code is implemented in Dispose(bool)  
        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                foreach (lDataSync ds in m_dataSyncs.Values)
                {
                    ds.Dispose();
                }
                foreach (lDataContent dc in m_dataContents.Values)
                {
                    dc.Dispose();
                }
            }
            // free native resources if there are any.
            //m_dataSyncs.Clear();
            //m_dataContents.Clear();
        }
        #endregion
    }
    public class lOleDbContentProvider : lContentProvider
    {
        static lOleDbContentProvider m_instance;
        public static lContentProvider getInstance(Form1 parent)
        {
            if (m_instance == null)
            {
                m_instance = new lOleDbContentProvider();
                m_instance.m_form = parent;
            }
            return m_instance;
        }

        lOleDbContentProvider() : base()
        {
            //string cnnStr = "";
            //m_cnn = new OleDbConnection(cnnStr);
            //m_cnn.Open();
        }

        private OleDbConnection m_cnn;
        OleDbDataAdapter m_dataAdapter;
        public override void initCnn(string m_cnnStr)
        {
            //init cnn
            m_cnn = new OleDbConnection(m_cnnStr);
            m_cnn.Open();
            m_dataAdapter = new OleDbDataAdapter
            {
                SelectCommand = new OleDbCommand(
                string.Format("select * from paragraphs where titleId = ? order by ord"),
                m_cnn)
            };
            m_dataAdapter.SelectCommand.Parameters.Add(
                new OleDbParameter() { DbType = DbType.UInt64 });
        }
        public override void closeCnn()
        {
            if(m_cnn != null){
                m_dataAdapter.Dispose();
                m_cnn.Close();
                m_cnn.Dispose();
                m_cnn = null;
            }
        }
        public override void getTitleParagraphs(UInt64 titleId, DataTable dataTable)
        {
            m_dataAdapter.SelectCommand.Parameters[0].Value = titleId;
            dataTable.Clear();
            dataTable.Locale = System.Globalization.CultureInfo.InvariantCulture;
            m_dataAdapter.Fill(dataTable);
        }
        public override DataTable getTitles()
        {
            var dataAdapter = new OleDbDataAdapter
            {
                SelectCommand = new OleDbCommand(
                "SELECT titles.ID as ID, path & '/' & title as zPath"
                + " FROM titles INNER JOIN paths ON titles.pathId = paths.ID"
                + " order by paths.ord, titles.ord; ", m_cnn)
            };
            var dt = new DataTable();
            dataAdapter.Fill(dt);
            dataAdapter.Dispose();
            return dt;
        }
        public override void UpdateTitle(DataTable dt)
        {
            using (OleDbCommandBuilder builder = new OleDbCommandBuilder(m_dataAdapter))
            {
                if (dt != null)
                {
                    m_dataAdapter.UpdateCommand = builder.GetUpdateCommand();
                    m_dataAdapter.Update(dt);
                }
            }
        }
        class pathItem
        {
            public UInt64 id;
            public string path;
        }
        public override List<MyTitle> getTitles2()
        {
            OleDbConnection cnn = m_cnn;
            //get paths
            var pathLst = new List<pathItem>();
            var qry = "select * from paths order by ord ASC";
            var cmd = new OleDbCommand(qry, cnn);
            var reader = cmd.ExecuteReader();
            while (reader.Read())
            {
                pathLst.Add(new pathItem()
                {
                    id = Convert.ToUInt64(reader["ID"]),
                    path = Convert.ToString(reader["path"])
                });
            }
            reader.Close();

            //get title
            var titleLst = new List<MyTitle>();
            var qry2 = "select * from titles WHERE pathId = @pathId order by ord ASC";
            var cmd2 = new OleDbCommand(qry2, cnn);
            cmd2.Parameters.Add("@pathId", OleDbType.BigInt);
            foreach (var pi in pathLst)
            {
                cmd2.Parameters[0].Value = pi.id;
                var reader2 = cmd2.ExecuteReader();
                int ord = 0;
                while (reader2.Read())
                {
                    var title = new MyTitle();
                    title.ID = Convert.ToUInt64(reader2["ID"]);
                    title.zTitle = Convert.ToString(reader2["title"])
                        .TrimEnd(new char[] { '\r', '\n', '\v' });
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
        public override List<MyParagraph> getTitleParagraphs(UInt64 titleId)
        {
            var paragraphLst = new List<MyParagraph>();
            //get title
            var qry = "select * from paragraphs where titleId = @id ";
            var cmd = new OleDbCommand(qry, m_cnn);
            cmd.Parameters.AddWithValue("@id", titleId);
            var reader = cmd.ExecuteReader();
            while (reader.Read())
            {
                var par = new MyParagraph();
                par.titleId = Convert.ToUInt64(reader["titleId"]);
                par.order = Convert.ToInt32(reader["ord"]);
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
        public override List<MyParagraph> getTitleParagraphs(DataTable dt)
        {
            var paragraphLst = new List<MyParagraph>();
            foreach (DataRow row in dt.Rows)
            {
                if (row.RowState == DataRowState.Deleted) continue;

                var par = new MyParagraph
                {
                    titleId = Convert.ToUInt64(row["titleId"]),
                    order = Convert.ToInt32(row["ord"]),
                    alignment = Convert.ToInt32(row["alignment"]),
                    leftIndent = Convert.ToInt32(row["leftIndent"]),
                    fontSize = Convert.ToInt32(row["fontSize"]),
                    fontBold = Convert.ToInt32(row["fontBold"]),
                    fontItalic = Convert.ToInt32(row["fontItalic"]),
                    content = Convert.ToString(row["content"])
                };
                paragraphLst.Add(par);
            }
            return paragraphLst;
        }
        protected override lDataContent newDataContent(string tblName)
        {
            lOleDbDataContent data = new lOleDbDataContent(tblName, m_cnn);
#if !use_bg_work
            data.m_form = m_form;
#endif
            return data;
        }

        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Security", "CA2100:Review SQL queries for security vulnerabilities")]
        public override DataTable GetData(string qry)
        {
            OleDbDataAdapter dataAdapter = new OleDbDataAdapter();
            dataAdapter.SelectCommand = new OleDbCommand(qry, m_cnn);
            // Populate a new data table and bind it to the BindingSource.
            DataTable table = new DataTable();
            table.Locale = System.Globalization.CultureInfo.InvariantCulture;
            dataAdapter.Fill(table);
            return table;
        }

        public override object GetCnn()
        {
            return m_cnn;
        }

        #region dispose
        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                m_cnn.Dispose();
            }
            // free native resources if there are any.
            base.Dispose(disposing);
        }
        #endregion
    }


    public class lDataSync : IRefresher, IDisposable
    {
        private lDataContent m_data;
        public AutoCompleteStringCollection m_colls;
        public Dictionary<string, string> m_maps;
        public lDataSync(lDataContent data)
        {
            m_data = data;
            m_data.m_refresher = this;
        }

        static Dictionary<string, string> dict = new Dictionary<string, string>() {
                {"[áàảãạ]", "a"  },
                {"[ăằắẵẳặ]", "a"},
                {"[âầấẫẩậ]", "a"},
                {"[đ]", "d"     },
                {"[éèẻẽẹ]", "e" },
                {"[êềếễểệ]", "e"},
                {"[íìĩỉị]", "i" },
                {"[òóỏõọ]",  "o"},
                {"[ồôỗốộổ]",  "o"},
                {"[ơờớỡởợ]", "o"},
                {"[úùủũụ]", "u" },
                {"[ừưữứửự]", "u"},
            };
        static string genKey(string value)
        {
            string key = value.ToLower();
            foreach (var i in dict)
            {
                key = Regex.Replace(key, i.Key, i.Value);
            }
            return key;
        }
        public void Refresh()
        {
            m_colls = new AutoCompleteStringCollection();
            m_maps = new Dictionary<string, string>();
            DataTable tbl = m_dataSource;
            foreach (DataRow row in tbl.Rows)
            {
                string val = row[1].ToString();
                string key = genKey(val);
                m_colls.Add(val);
                m_colls.Add(key);
                try
                {
                    m_maps.Add(key, val);
                }
                catch { }
            }
        }
        public void LoadData()
        {
            m_data.Load();
            Refresh();
        }
        public BindingSource m_bindingSrc
        {
            get { return m_data.m_bindingSource; }
        }
        public DataTable m_dataSource
        {
            get { return m_data.m_dataTable; }
        }
        public void Update(string selectedValue)
        {
            Debug.WriteLine("{0}.Update {1}", this, selectedValue);
            string key = genKey(selectedValue);
            if (!m_maps.ContainsKey(key))
            {
                m_colls.Add(key);
                m_colls.Add(selectedValue);
                m_maps.Add(key, selectedValue);
                //update database
                Add(selectedValue);
            }
        }
        private void Add(string newValue)
        {
            //single col tables
            DataRow newRow = m_dataSource.NewRow();
            newRow[1] = newValue;
            m_dataSource.Rows.Add(newRow);
            m_data.Submit();
        }
        public string find(string key)
        {
            key = genKey(key);
            if (m_maps.ContainsKey(key))
                return m_maps[key];
            else
                return null;
        }

        #region dispose
        // Dispose() calls Dispose(true)  
        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }
        // NOTE: Leave out the finalizer altogether if this class doesn't   
        // own unmanaged resources itself, but leave the other methods  
        // exactly as they are.   
        ~lDataSync()
        {
            // Finalizer calls Dispose(false)  
            Dispose(false);
        }
        // The bulk of the clean-up code is implemented in Dispose(bool)  
        protected virtual void Dispose(bool disposing)
        {
            if (disposing)
            {
                // free managed resources
                m_colls.Clear();
                m_maps.Clear();
            }
            // free native resources if there are any.  
        }
        #endregion
    }

    public class lOleDbDataContent : lDataContent
    {
        private OleDbConnection m_cnn;
        private OleDbDataAdapter m_dataAdapter;
        private OleDbTransaction m_trans;

        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Security", "CA2100:Review SQL queries for security vulnerabilities")]
        public lOleDbDataContent(string tblName, OleDbConnection cnn)
            : base()
        {
            m_table = tblName;
            m_cnn = cnn;
            m_dataAdapter = new OleDbDataAdapter();
            m_dataAdapter.SelectCommand = new OleDbCommand(string.Format("select * from {0}", tblName), cnn);
#if init_datatable_cols
            init();
#endif
        }

#if !use_cmd_params
        public override void Search(string exprs)
        {
            string sql = string.Format("select * from {0} ", m_table);
            if (exprs != null)
            {
                sql += " where " + exprs;
            }
            GetData(sql);
        }
#endif
#if use_cmd_params
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Security", "CA2100:Review SQL queries for security vulnerabilities")]
        //public override void Search(List<string> exprs, List<lSearchParam> srchParams)
        //{
        //    string sql = string.Format("select * from {0} ", m_table);

        //    if (exprs.Count > 0)
        //    {
        //        sql += " where " + string.Join(" and ", exprs);
        //        SqlCommand selectCommand = new SqlCommand(sql, m_cnn);
        //        foreach (var param in srchParams)
        //        {
        //            var p = selectCommand.Parameters.AddWithValue(param.key, param.val);
        //            if (param.type == DbType.String) { p.DbType = DbType.String; }
        //        }
        //        GetData(selectCommand);
        //    }
        //    else
        //    {
        //        GetData(sql);
        //    }
        //}
#endif
        public override void Reload()
        {
            base.Reload();
            GetData(m_dataAdapter.SelectCommand.CommandText);
        }
        protected override void updateTable()
        {
            using (OleDbCommandBuilder builder = new OleDbCommandBuilder(m_dataAdapter))
            {
                DataTable dt = (DataTable)m_bindingSource.DataSource;
                if (dt != null)
                {
                    m_dataAdapter.UpdateCommand = builder.GetUpdateCommand();
                    m_dataAdapter.Update(dt);
                }
            }
        }
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Security", "CA2100:Review SQL queries for security vulnerabilities")]
        protected override void GetData(string selectStr)
        {
            OleDbCommand selectCommand = new OleDbCommand(selectStr, m_cnn);
            GetData(selectCommand.CommandText);
        }
        private void GetData(OleDbCommand selectCommand)
        {
            m_dataAdapter.SelectCommand = selectCommand;
            // Populate a new data table and bind it to the BindingSource.
            fetchData();
        }
        protected override void fillTable()
        {
            m_dataAdapter.Fill(m_dataTable);
        }

        public override void BeginTrans()
        {
            m_trans = m_cnn.BeginTransaction();
        }
        public override void EndTrans()
        {
            m_trans.Commit();
            m_trans.Dispose();
        }
    }

}
