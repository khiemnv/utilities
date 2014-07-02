// ClassifyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "KKclassify.h"
#include "ClassifyDlg.h"


// CClassifyDlg dialog

IMPLEMENT_DYNAMIC(CClassifyDlg, CDialog)

CClassifyDlg::CClassifyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClassifyDlg::IDD, pParent)
{
	
}

CClassifyDlg::~CClassifyDlg()
{
}

void CClassifyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAIN_LIST, m_cMainList);
	DDX_Control(pDX, IDC_FINFO_LIST, m_cFInfoList);
	DDX_Control(pDX, IDC_PATH_BTN, m_cPathBtn);
	DDX_Control(pDX, IDC_PATH_EDIT, m_cPathEdit);
	DDX_Control(pDX, IDC_CLASSIFY_STATIC, m_cStaticClassify);
	DDX_Control(pDX, IDC_SQICKTICK_STATIC, m_cStaticSQTick);
	DDX_Control(pDX, IDC_PROCESS_BTN, m_cProcessBtn);
	DDX_Control(pDX, IDC_CLASSIFY_COMBO, m_cClassifyCmb);
	DDX_Control(pDX, IDC_SQICKTICK_EDT, m_cSqickTickEdt);
	DDX_Control(pDX, IDC_SQICKTICK_COMBO, m_cSQTickCmb);
	DDX_Control(pDX, IDC_COMMENT_LIST, m_cCommentList);
}


BEGIN_MESSAGE_MAP(CClassifyDlg, CDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CClassifyDlg::OnLvnItemchangedList1)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_PATH_BTN, &CClassifyDlg::OnBnClickedPathBtn)
	ON_EN_CHANGE(IDC_PATH_EDIT, &CClassifyDlg::OnEnChangePathEdit)
	ON_WM_DROPFILES()
	/*ON_MESSAGE(WM_DROPFILES, &CClassifyDlg::onDrop)*/
	ON_WM_CLOSE()
	ON_CBN_SELCHANGE(IDC_CLASSIFY_COMBO, &CClassifyDlg::OnCbnSelchangeClassifyCombo)
//	ON_NOTIFY(LVN_ITEMCHANGED, IDC_MAIN_LIST, &CClassifyDlg::OnLvnItemchangedMainList)
	ON_NOTIFY(NM_CLICK, IDC_MAIN_LIST, &CClassifyDlg::OnNMClickMainList)
	ON_CBN_EDITCHANGE(IDC_CLASSIFY_COMBO, &CClassifyDlg::OnCbnEditchangeClassifyCombo)
//	ON_CBN_EDITUPDATE(IDC_CLASSIFY_COMBO, &CClassifyDlg::OnCbnEditupdateClassifyCombo)
//	ON_CBN_SETFOCUS(IDC_CLASSIFY_COMBO, &CClassifyDlg::OnCbnSetfocusClassifyCombo)
//ON_CBN_KILLFOCUS(IDC_CLASSIFY_COMBO, &CClassifyDlg::OnCbnKillfocusClassifyCombo)
ON_WM_SYSKEYDOWN()
ON_WM_SYSCOMMAND()
ON_WM_SYSCHAR()
ON_NOTIFY(NM_DBLCLK, IDC_MAIN_LIST, &CClassifyDlg::OnNMDblclkMainList)
END_MESSAGE_MAP()


// CClassifyDlg message handlers

void CClassifyDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CClassifyDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if(IsWindow(m_cMainList.GetSafeHwnd()))
	{
		resize(cx,cy);
// 		CRect newRect=CRect(7*SIZE_SCALE,7*SIZE_SCALE,cx-7*SIZE_SCALE,cy-7*SIZE_SCALE);
// 		m_cMainList.MoveWindow(&newRect);
	}
}

BOOL CClassifyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// TODO:  Add extra initialization here
	m_cListEx.chose_clist(&m_cMainList);
	m_cListEx.add_col(0,_T("Files"));
	m_cListEx.add_col(1,_T("Species"));
	m_cListEx.add_col(2,_T("State"));
//read config and set some init state
	config.getCtrl(&m_cPathEdit,&m_cSQTickCmb,&m_cClassifyCmb);
	config.conf_file=_T(KKCLASSIFY_CONF_FILE);
	config.readConf();
	
 	config.updateCtrl(KKSET_DLG_VALUE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

#ifndef __KKSCALCE
#define __KKSCALCE
#define KKSCALCE(x) ((x)*14/11)
#define KKUNSCALCE(x) ((x)*11/14)
#define KK_Y_SPACE	5
#define KK_X_SPACE	2
#define KK_L_SPACE	7
#define KK_R_SPACE	7
#define KK_T_SPACE	7
#define KK_B_SPACE	2
#define KK_NY_BTN	20
#define KK_NY_EDT	17
#define KK_NY_CMB	14
#define KK_NX_INFO	130
#define KK_NY_C		32
#endif
int CClassifyDlg::resize(int cx, int cy)
{
	int x,y,nx,ny;
	/*int temp;*/
//
	cx=KKUNSCALCE(cx);
	cy=KKUNSCALCE(cy);
//resize the path btn
	nx=25; ny=KK_NY_EDT;
	x=cx-7-nx; y=KK_T_SPACE;
	m_cPathBtn.MoveWindow(KKSCALCE(x),KKSCALCE(y),KKSCALCE(nx),KKSCALCE(ny));
//resize the path edit	
	nx=x-9;//(7+2)
	x=KK_L_SPACE;
	m_cPathEdit.MoveWindow(KKSCALCE(x),KKSCALCE(y),KKSCALCE(nx),KKSCALCE(ny));

//resize the process btn
	nx=50; ny=KK_NY_BTN;
	x=cx-KK_R_SPACE-nx; y=cy-KK_B_SPACE-ny;
	cy=y;
	m_cProcessBtn.MoveWindow(KKSCALCE(x),KKSCALCE(y),KKSCALCE(nx),KKSCALCE(ny));
//resize classify group
	nx=KK_NX_INFO; ny=KK_NY_C;
	x=KK_L_SPACE; 
	cy=cy-KK_Y_SPACE-ny;
	y=cy;
	m_cStaticClassify.MoveWindow(KKSCALCE(x),KKSCALCE(y),KKSCALCE(nx),KKSCALCE(ny));
//resize s qick tick
	x+=nx+KK_X_SPACE;
	nx=cx-KK_R_SPACE-x;
	m_cStaticSQTick.MoveWindow(KKSCALCE(x),KKSCALCE(y),KKSCALCE(nx),KKSCALCE(ny));
	//resize sqicktick cmb box
	x+=5; y+=12;
	nx=82; ny=KK_NY_CMB;
	m_cSQTickCmb.MoveWindow(KKSCALCE(x),KKSCALCE(y),KKSCALCE(nx),KKSCALCE(ny));
	//resize sqicktick edit
	x+=nx+KK_X_SPACE;
	nx=cx-x-KK_X_SPACE-5-KK_R_SPACE;
	ny=KK_NY_EDT;
	m_cSqickTickEdt.MoveWindow(KKSCALCE(x),KKSCALCE(y),KKSCALCE(nx),KKSCALCE(ny));
//resize classify cmb
	nx=KK_NX_INFO-10;
	ny=KK_NY_CMB;
	x=KK_L_SPACE+5;
	m_cClassifyCmb.MoveWindow(KKSCALCE(x),KKSCALCE(y),KKSCALCE(nx),KKSCALCE(ny));

//resize file info list
	cy=y-KK_Y_SPACE-KK_Y_SPACE-KK_Y_SPACE;
	x=KK_L_SPACE; y=KK_T_SPACE+KK_NY_EDT+KK_Y_SPACE;
	nx=KK_NX_INFO; ny=cy-y;
	m_cFInfoList.MoveWindow(KKSCALCE(x),KKSCALCE(y),KKSCALCE(nx),KKSCALCE(ny));
//resize main list
	x=x+nx+KK_X_SPACE;
	nx=cx-x-KK_R_SPACE;
	ny=ny-KK_NY_C-KK_Y_SPACE;
	m_cMainList.MoveWindow(KKSCALCE(x),KKSCALCE(y),KKSCALCE(nx),KKSCALCE(ny));
//re size comment list
	y+=ny+KK_Y_SPACE;
	ny=KK_NY_C;
	m_cCommentList.MoveWindow(KKSCALCE(x),KKSCALCE(y),KKSCALCE(nx),KKSCALCE(ny));
	return 0;
}

void CClassifyDlg::OnBnClickedPathBtn()
{
	// TODO: Add your control notification handler code here
	if(m_cPickedDir.pick())
	{
		m_cPathEdit.SetWindowText(m_cPickedDir.dirName);
		m_cPickedDir.listFiles();
		refresh();
	}
}

void CClassifyDlg::OnEnChangePathEdit()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	if (m_cPathEdit.GetFocus()==&m_cPathEdit)
	{
		m_cPickedDir.pick(&m_cPathEdit);
		m_cPickedDir.listFiles();
// 		CString test;
// 		test=m_cPickedDir.getFFileName(0);
		refresh();
	}
}


void CClassifyDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: Add your message handler code here and/or call default
	TCHAR	fileName[MAX_PATH];
	int count = DragQueryFile(hDropInfo,-1,fileName,MAX_PATH);
	int i=0;

	POINT dragPoint;
	if(DragQueryPoint(hDropInfo,&dragPoint))
	{
		if(ChildWindowFromPoint(dragPoint)==&m_cPathEdit)
		{
			m_cPathEdit.SetFocus();
			DragQueryFile(hDropInfo,0,fileName,MAX_PATH);
			if(PathIsDirectory(fileName))
				m_cPathEdit.SetWindowText(fileName);
			else
			{
// 				CString path=fileName;
// 				i=path.ReverseFind('\\');
				i=wcslen(fileName);
				for(;fileName[i]!='\\';i--)
					fileName[i]=0;
				m_cPathEdit.SetWindowText(fileName);
			}
		}
		else
			for(i=0;i<count;i++)
			{
				DragQueryFile(hDropInfo,i,fileName,MAX_PATH);
				m_cPickedDir.addFile(fileName);
			}
	}

	refresh();
	CDialog::OnDropFiles(hDropInfo);
}

void CClassifyDlg::refresh(void)
{
	int i;
	m_cListEx.chose_clist(&m_cMainList);
	m_cListEx.clean();
	for(i=0;i<m_cPickedDir.getNFile();i++)
	{
		m_cListEx.add_item(i,0,m_cPickedDir.getFile(i));
	}
}

// BOOL CClassifyDlg::readConf(void)
// {
// 	FILE *f;
// 	char buff[MAX_PATH];
// 	int i;
// 	fopen_s(&f,strTtoAstr(config.conf_file),"rt");
// 	if(f!=0)
// 	{
// 		//read path
// 		fscanf_s(f,"%s",buff,MAX_PATH);
// 		config.path = CString(buff+KKCOMENTLENGHT);
// 		//read state list
// 		if(!fscanf_s(f,"%s%d",buff,MAX_PATH,&i))
// 			return FALSE;
// 
// 		while(i>0)
// 		{
// 			i--;
// 			fscanf_s(f,"%s",buff,MAX_PATH);
// 			config.SQTstates.Add(CString(buff));
// 		}
// 		//read classifySpecies list
// 		if(!fscanf_s(f,"%s%d",buff,MAX_PATH,&i))
// 			return FALSE;
// 		
// 		while(i>0)
// 		{
// 			i--;
// 			fscanf_s(f,"%s",buff,MAX_PATH);
// 			config.classifySpecies.Add(CString(buff));
// 		}
// 
// 		fclose(f);
// 	}
// 
// 	return FALSE;
// }

char* CClassifyDlg::strTtoAstr(CString strT)
{
	size_t	newsize=(strT.GetLength()+1)*2;
	char* Astr=new char[newsize];
	size_t convertedCharsw=0;
	wcstombs_s(&convertedCharsw,Astr,newsize,strT,_TRUNCATE);
	return Astr;
}
// BOOL CClassifyDlg::writeProfile()
// {
// 	BOOL retval;
// 	CWinApp* kkapp=(CWinApp*)
// 		this->GetParent()->GetParent();
// 	
// 
// 	TCHAR* section=_T("kksection");
// 	TCHAR* entry=_T("kkentry");
// 	/*TCHAR* _value=_T("kkvalue");*/
// 
// //	retval=kkapp->WriteProfileString(section,entry,_T("kk faf 123"));
// 
// 	CString retstr=kkapp->GetProfileString(section,entry,0);
// // 	WritePrivateProfileString(
// // 		__in  LPCTSTR lpAppName,
// // 		__in  LPCTSTR lpKeyName,
// // 		__in  LPCTSTR lpString,
// // 		__in  LPCTSTR lpFileName
// // 		);
// 
// 	
// 
// 	return	retval;
// }
// 
// BOOL CClassifyDlg::writeConf(void)
// {
// 	FILE *f;
// 	char buff[MAX_PATH];
// 	int i;
// 	fopen_s(&f,strTtoAstr(config.conf_file),"wt");
// 	if(f!=0)
// 	{
// 		//print path
// 		fprintf_s(f,"path---:%s\n",strTtoAstr(config.path));
// 	//print state list
// 		fprintf_s(f,"SQTstates-: %d\n",config.SQTstates.GetSize());
// 		for(i=0;i<config.SQTstates.GetSize();i++)
// 			fprintf(f,"%s",strTtoAstr(config.SQTstates.GetAt(i)));
// 	//write classifySpecies list
// 		fprintf_s(f,"classifySpecies: %d\n",config.classifySpecies.GetSize());
// 
// 		for(i=0;i<config.classifySpecies.GetSize();i++)
// 			fprintf_s(f,"%s\n",strTtoAstr(config.classifySpecies.GetAt(i)));
// 
// 		fclose(f);
// 	}
// 
// 	return FALSE;
// }
void CClassifyDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	config.updateCtrl(KKGET_DLG_VALUE);
	config.writeConf();

	CDialog::OnClose();
}

void CClassifyDlg::OnCbnSelchangeClassifyCombo()
{
	// TODO: Add your control notification handler code here
	int selectedItem=m_cMainList.GetNextItem(-1,LVIS_FOCUSED|LVIS_SELECTED);
	int cursel=m_cClassifyCmb.GetCurSel();
	CString lable;
	m_cClassifyCmb.GetLBText(cursel,lable);
	m_cListEx.add_item(selectedItem,KK_CLASSIFY_COL,lable);
}

// void CClassifyDlg::updateClassify(int setORget)
// {
// 	CString	label;
// 	int	i = m_cClassifyCmb.GetCurSel();
// 	switch(setORget)
// 	{
// 	case KK_SETLVI:
// 		//m_cMainList.GetNextItem(-1,LVIS_FOCUSED|LVIS_SELECTED);
// 		//m_cClassifyCmb.GetLBText(i,label);
// 		//listExt.add_item(KK_CLASSIFY_COL,label);
// 		break;
// 	}
// }
//void CClassifyDlg::OnLvnItemchangedMainList(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
//	// TODO: Add your control notification handler code here
//	if(GetFocus()==&m_cMainList)
//		m_vPreMLRow=m_cMainList.GetNextItem(-1,LVIS_FOCUSED |LVIS_SELECTED);
//	*pResult = 0;
//}

void CClassifyDlg::OnNMClickMainList(NMHDR *pNMHDR, LRESULT *pResult)
{
//	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	int select_item;
	CTabCtrl* pMainTab=(CTabCtrl*)GetParent();
	int	tabID=1;
	TCHAR buffer[MAX_PATH];
	TCITEM tcitem;
	 
	select_item=m_cMainList.GetNextItem(-1,LVIS_FOCUSED |LVIS_SELECTED);
	if(select_item!=-1)
	{		
		tcitem.pszText=buffer;
		pMainTab->GetItem(tabID,&tcitem);
		tcitem.mask=tcitem.mask|TCIF_TEXT;
		//set tab title	
		m_cMainList.GetItemText(select_item,0,buffer,MAX_PATH);	
		pMainTab->SetItem(tabID,&tcitem);
		//pMainTab->GetParent()->SendMessage(WM_PAINT,0,0);
	}

	*pResult = 0;
}

void CClassifyDlg::OnCbnEditchangeClassifyCombo()
{
	// TODO: Add your control notification handler code here
//	CString temp=0;	
//	m_cClassifyCmb.GetWindowText(temp);
// 	int i=0,start=0;
// 	while(start!=-1)
// 	{
// 		i=m_cClassifyCmb.FindString(start,temp);
// 		start=i>start?i:-1;
// 		m_cClassifyCmb.SetTopIndex(i);
// 	}
	//m_cClassifyCmb.SetTopIndex(retval);
	//m_cClassifyCmb.SelectString(-1,temp);
	//m_cClassifyCmb.ShowDropDown(TRUE );

//	m_cClassifyCmb.ShowDropDown(TRUE );
}

//void CClassifyDlg::OnCbnEditupdateClassifyCombo()
//{
//	// TODO: Add your control notification handler code here
//}

//void CClassifyDlg::OnCbnSetfocusClassifyCombo()
//{
//	// TODO: Add your control notification handler code here
//}

//void CClassifyDlg::OnCbnKillfocusClassifyCombo()
//{
//	// TODO: Add your control notification handler code here
//	m_cClassifyCmb.ShowWindow(SW_SHOW);
//}

void CClassifyDlg::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if(GetFocus()==&m_cClassifyCmb);
	CDialog::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

void CClassifyDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnSysCommand(nID, lParam);
}

void CClassifyDlg::OnSysChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default

	CDialog::OnSysChar(nChar, nRepCnt, nFlags);
}
void CClassifyDlg::add_item(CListCtrl &list, int row, int col, CString &content)
{
	LVITEM	lvi;

	lvi.state=LVIS_SELECTED | LVIS_FOCUSED;
	lvi.mask = LVIF_TEXT |LVIF_STATE;
	lvi.iItem = row;
	lvi.iSubItem = col;
	lvi.pszText = (LPTSTR)(LPCTSTR)(content);

	if(col!=0)
		list.SetItem(&lvi);
	else
		list.InsertItem(&lvi);
}
void CClassifyDlg::add_row(CListCtrl &list,CString* contents, int n_col)
{
	int row=list.GetItemCount();
	for(int i=0;i<n_col;i++)
		add_item(list,row,i,contents[i]);
}

void CClassifyDlg::add_col(CListCtrl &list,int col, CString& heading)
{
	list.InsertColumn(col,heading,LVCFMT_LEFT,LVCF_MINWIDTH);
}
void CClassifyDlg::OnNMDblclkMainList(NMHDR *pNMHDR, LRESULT *pResult)
{
//	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here

	//char buffer[1024];
	CKKFile m_KKfile;
	int select_item=m_cMainList.GetNextItem(-1,LVIS_FOCUSED |LVIS_SELECTED);

	//get file info
	CString temp=m_cPickedDir.getFFileName(select_item);

	char *t=strTtoAstr(temp);
	m_KKfile.do_work(t);
	free(t);
	*pResult = 0;
}
