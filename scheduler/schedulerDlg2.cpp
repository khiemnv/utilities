
// schedulerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "scheduler.h"
#include "schedulerDlg.h"
#include <WinBase.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CschedulerDlg dialog

CschedulerDlg::CschedulerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CschedulerDlg::IDD, pParent)
	, m_app(NULL)
	, m_timeSyn(0)
	, m_pData(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CschedulerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_NOW, m_now);
	DDX_Control(pDX, IDC_LIST1, m_list);
	DDX_Control(pDX, IDC_HOURS, m_hours);
	DDX_Control(pDX, IDC_CHECK1, m_nowChk);
	DDX_Control(pDX, IDC_EDIT_BTN, m_editBtn);
	DDX_Control(pDX, IDC_STATIC_GT, m_sGroupT);
	DDX_Control(pDX, IDC_STATIC_G2, m_sGroup2);
	DDX_Control(pDX, IDC_STATIC_REMAIN, m_sRemain);
	DDX_Control(pDX, IDC_STATIC_SUB, m_sSub);
	DDX_Control(pDX, IDC_STATIC_NOTE, m_sNote);
	DDX_Control(pDX, IDC_FARM, m_sub);
	DDX_Control(pDX, IDC_COMMENT, m_note);
	DDX_Control(pDX, IDC_ADD, m_addBtn);
	DDX_Control(pDX, IDC_DEL, m_delBtn);
}

BEGIN_MESSAGE_MAP(CschedulerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_ADD, &CschedulerDlg::OnBnClickedAdd)
	ON_BN_CLICKED(IDC_DEL, &CschedulerDlg::OnBnClickedDel)
//	ON_NOTIFY(NM_SETFOCUS, IDC_HOURS, &CschedulerDlg::OnNMSetfocusHours)
ON_WM_TIMER()
ON_WM_CLOSE()
ON_BN_CLICKED(IDC_CHECK1, &CschedulerDlg::OnBnClickedCheck1)
ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CschedulerDlg::OnNMDblclkList1)
ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CschedulerDlg::OnNMRClickList1)
ON_NOTIFY(LVN_KEYDOWN, IDC_LIST1, &CschedulerDlg::OnLvnKeydownList1)
ON_BN_CLICKED(IDC_EDIT_BTN, &CschedulerDlg::OnBnClickedEditBtn)
ON_NOTIFY(NM_CLICK, IDC_LIST1, &CschedulerDlg::OnNMClickList1)
ON_NOTIFY(HDN_ITEMCLICK, 0, &CschedulerDlg::OnHdnItemclickList1)
ON_COMMAND(ID_OPTION_SETTING, &CschedulerDlg::OnOptionSetting)
ON_WM_SIZE()
ON_COMMAND(ID_FILE_QUIT, &CschedulerDlg::OnFileQuit)
ON_COMMAND(ID_OPTION_HIDE, &CschedulerDlg::OnOptionHide)
ON_WM_LBUTTONDBLCLK()
 ON_MESSAGE(MYWM_NOTIFYICON,&CschedulerDlg::OnSysIconClick)
END_MESSAGE_MAP()


// CschedulerDlg message handlers

BOOL CschedulerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	//mutex
	m_mutex = OpenMutex(MUTEX_ALL_ACCESS ,FALSE,_T("herb"));
	
	if (m_mutex==0)
	{
		CreateMutex(NULL,FALSE,_T("herb"));
	}
	else
	{
		EndDialog(1);
		return 0;
	}


	m_app=AfxGetApp();

	
	//set min size
	CRect temprect(0,0,300,200);
	setMinRect(temprect);

	m_pData=(ITEMDATA**)calloc(KS_MAX_ITIMES,sizeof(ITEMDATA*));
	for(int i=0;i<KS_MAX_ITIMES;i++)
		m_pData[i]=0;

	m_now.SetFormat(_T("dd  HH:mm:ss"));
	m_list.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
	m_hours.SetFormat(_T("HH:mm:ss"));

	COleDateTime mytime;
	/*mytime.Format(_T("dd HH:MM"));*/
	mytime.SetTime(0,0,0);
	m_hours.SetTime(mytime);

	//m_list.InsertColumn(0,_T("Time"),0,LVCF_DEFAULTWIDTH);
	m_list.InsertColumn(1,_T("Sub"),0,LVCF_DEFAULTWIDTH);
	m_list.InsertColumn(2,_T("Note"),0,LVCF_DEFAULTWIDTH);
	m_list.InsertColumn(0,_T("Date"),0,LVCF_DEFAULTWIDTH);


	SetTimer(1,KS_DELAY,0);
	SetTimer(2,1000,0);
	SetTimer(3,KS_STORE_TIMER,0);

	m_timeSyn=m_app->GetProfileInt(_T("KScheduler"),_T("update_time"),0);	
	m_nowChk.SetCheck(m_timeSyn);

	restoreNote();
	updatePosition(KS_SETPOS);
	updateSetting();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CschedulerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CschedulerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CschedulerDlg::OnBnClickedAdd()
{
	// TODO: Add your control notification handler code here
	COleDateTime temptime,temphours;
	
	CString strtime;
	int nitem;

	nitem=m_list.GetItemCount();
	if(nitem>=KS_MAX_ITIMES) return;

	if(m_pData[nitem]==0)
		m_pData[nitem] = new ITEMDATA;

	m_pData[nitem]->index=nitem;

	m_now.GetTime(temptime);
	m_hours.GetTime(temphours);
	temptime=temptime+temphours;

	//show time on list
	strtime=time2str(&temptime);
	//luu time
	m_pData[nitem]->time=strtime;
	m_list.InsertItem(nitem,strtime);
	
	CString tempstr;
	//get the note
	GetDlgItemText(IDC_FARM,tempstr);
	m_pData[nitem]->note=tempstr;	//save note
	m_list.SetItem(nitem,1,LVIF_TEXT ,tempstr,0,LVIF_STATE,0,0);
	//get the content
	GetDlgItemText(IDC_COMMENT,tempstr);
	m_pData[nitem]->comment=tempstr;	//save comment
	m_list.SetItem(nitem,2,LVIF_TEXT ,tempstr,0,LVIF_STATE,0,0);
	//set item data
	m_list.SetItemData(nitem,(DWORD_PTR)m_pData[nitem]);
}

void CschedulerDlg::OnBnClickedDel()
{
	// TODO: Add your control notification handler code here	
// 	POSITION pos = m_list.GetFirstSelectedItemPosition();
// 	while (pos)
// 	{
// 		int nItem = m_list.GetNextSelectedItem(pos);
// 		m_list.DeleteItem(nItem);
// 	}
	int nsel=m_list.GetSelectedColumn();
	int cursel;
//	ITEMDATA* pItemData;
//	int index;
	cursel=m_list.GetNextItem(-1,LVIS_SELECTED);
	while(cursel != -1)
	{
		//delete item data
// 		pItemData=(ITEMDATA*)m_list.GetItemData(cursel);
// 		index=pItemData->index;
// 		while(index < KS_MAX_ITIMES)
// 		{
// 			if (m_pData[index+1]==0) break;
// 			m_pData[index]=m_pData[index+1];
// 			m_pData[index]->index--;
// 			index++;
// 		}
// 		m_pData[index]=0;
// 		delete pItemData;
// 
// 		m_list.DeleteItem(cursel);
		delItem(cursel);
		cursel=m_list.GetNextItem(-1,LVIS_SELECTED|LVIS_FOCUSED);
	}
}

//void CschedulerDlg::OnNMSetfocusHours(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	// TODO: Add your control notification handler code here
//	AfxMessageBox(_T(""));
//	*pResult = 0;
//}

void CschedulerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	//update now date time
	switch(nIDEvent)	
	{
	case 2:
		if(m_timeSyn)
		{		
			CTime temptime = CTime::GetCurrentTime();
			temptime.GetCurrentTime();
			m_now.SetTime(&temptime);
		}
		break;
	case 1:
		//show note		
		CreateThread(0,0,&my_thread,this,0,0);		
		break;
	case 3:
		CreateThread(0,0,&my_thread_storeNote,this,0,0);
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

void CschedulerDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	m_app->WriteProfileInt(_T("KScheduler"),_T("update_time"),m_timeSyn);
	KillTimer(1);
	KillTimer(2);
	KillTimer(3);
	storeNote();
	updatePosition(KS_SAVEPOS);

	//mutex
	ReleaseMutex(m_mutex);

	//Remove the Icon from the Systray
	TrayMessage( NIM_DELETE );	

	CDialog::OnClose();

}

UINT CschedulerDlg::showNote(LPVOID lparam)
{
	COleDateTime curtime;
	COleDateTime temptime;
	CString tempstr;
	CString notemsg;

	int nitem=m_list.GetItemCount();
	int count=0,i;
	TCHAR tempch;

	curtime = COleDateTime::GetCurrentTime();
	for(i=0;i<nitem;)
	{
		tempstr = m_list.GetItemText(i,0);
		if(!temptime.ParseDateTime(tempstr,0))
			return 0;

		tempstr = m_list.GetItemText(i,2);
		tempstr = tempstr.Left(1);
		tempch = tempstr[0];

		if(tempch != L'0')
		if((m_ksSetting.alertBeforeChk == BST_CHECKED) || (tempch==L'D'))
		{	
			temptime =temptime - m_ksSetting.alertBeforeTime;
		}

	//	tempstr=m_ksSetting.alertBeforeTime.Format(_T("%m/%d/%Y  %H:%M:%S"));
	//	tempstr=temptime.Format(_T("%m/%d/%Y  %H:%M:%S"));

		if (curtime>=temptime){
			count++;
			notemsg.AppendFormat(_T("%s: %s\n"),m_list.GetItemText(i,1),m_list.GetItemText(i,2));
			delItem(i);
			//m_list.DeleteItem(i);
			nitem--;			
		}
		else
			i++;
	}
	if(count > 0)
	{
//		AfxMessageBox(notemsg);
		MessageBox(notemsg.GetBuffer(),0,MB_SYSTEMMODAL);
	}

	
	return 0;
}

void CschedulerDlg::OnBnClickedCheck1()
{
	// TODO: Add your control notification handler code here
// 	m_timeSyn =0;
// 	if( IsDlgButtonChecked(IDC_NOW))
// 		m_timeSyn =1;

	m_timeSyn = m_timeSyn ^ 1;
	//m_app->WriteProfileInt(_T("KScheduler"),_T("update_time"),m_timeSyn);
}

void CschedulerDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	showNote();
	*pResult = 0;
}

void CschedulerDlg::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	//our menu
	CMenu contextMenu; 
	//the actual popup menu which is the first
	//sub menu item of IDR_CONTEXT
	CMenu* tracker; 

	LPNMITEMACTIVATE pactitem=(LPNMITEMACTIVATE)pNMHDR;
	POINT point=pactitem->ptAction;
	
	//at first we'll load our menu
	contextMenu.LoadMenu(IDR_MENU1); 

	//we have to translate the mouse coordinates first:
	//these are relative to the window (or view) but the context menu
	//needs coordinates that are relative to the whole screen
	ClientToScreen(&point);

	//tracker now points to IDR_CONTEXT's first submenu 
	//GetSubMenu(0) -&gt; returns the first submenu,
	// GetSubMenu(1)-&gt; second one etc..
	tracker = contextMenu.GetSubMenu(0); 

	//show the context menu
	//tracker-&gt;
	contextMenu.TrackPopupMenu(
		TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, 
		point.x , 
		point.y , 
		AfxGetMainWnd()); 


	*pResult = 0;
}

// void CALLBACK updatetime(HWND,UINT,UINT_PTR,DWORD)
// {
// 	//update now date time
// 	if(IsDlgButtonChecked(IDC_CHECK1))
// 	{
// 		CTime temptime = CTime::GetCurrentTime();
// 		temptime.GetCurrentTime();
// 		m_now.SetTime(&temptime);
// 	}
// }

int CschedulerDlg::storeNote(void)
{
// 	FILE	*f;
// 	char	abuff[KS_MAX_NOTE];
// 	TCHAR	wbuff[KS_MAX_NOTE];
// 	int		count,i;
// 	CString tempstr;
// 
// 	fopen_s(&f,KS_CONF_FILE,"w+");
// 	if(f!=0)
// 	{
// 		count=m_list.GetItemCount();
// 		fprintf(f,"%d\n",count);
// 		for(i=0;i<count;i++)
// 		{	
// 			abuff[0]=0;
// 			tempstr=m_list.GetItemText(i,0);
// 			tempstr.Replace(' ','*');
// 			WideCharToMultiByte(CP_UTF8,0,tempstr,-1,abuff,KS_MAX_NOTE,0,0);
// 			fprintf(f,"%s\n",abuff);
// 
// 			abuff[0]=0;
// 			//m_list.GetItemText(i,1,wbuff,KS_MAX_NOTE);			
// 			//WideCharToMultiByte(CP_UTF8,0,wbuff,-1,abuff,KS_MAX_NOTE,0,0);
// 			tempstr=m_list.GetItemText(i,1);
// 			tempstr.Replace(' ','*');
// 			WideCharToMultiByte(CP_UTF8,0,tempstr,-1,abuff,KS_MAX_NOTE,0,0);
// 			fprintf(f,"%s\n",abuff);
// 
// 			abuff[0]=0;
// // 			m_list.GetItemText(i,2,wbuff,KS_MAX_NOTE);			
// // 			WideCharToMultiByte(CP_UTF8,0,wbuff,-1,abuff,KS_MAX_NOTE,0,0);
//  			tempstr=m_list.GetItemText(i,2);
// 			tempstr.Replace(' ','*');
// 			WideCharToMultiByte(CP_UTF8,0,tempstr,-1,abuff,KS_MAX_NOTE,0,0);
// 			fprintf(f,"%s\n",abuff);
// 		}
// 		fclose(f);
// 		return 1;
// 	}

	CStdioFile iof;
	int count;
	CString tempstr;
	TCHAR wbuff[KS_MAX_NOTE];	

	if(iof.Open(_T(KS_CONF_FILE),
		CFile::modeCreate |CFile::modeWrite|CFile::typeText))
	{
	count=m_list.GetItemCount();
	_itow_s(count,wbuff,KS_MAX_NOTE,10);
	iof.WriteString(wbuff);
	iof.WriteString(_T("\n"));

	for (int i=0;i<count;i++)
	{
		tempstr=m_list.GetItemText(i,0);
		iof.WriteString(tempstr);
		iof.WriteString(_T("\n"));

		tempstr=m_list.GetItemText(i,1);
		iof.WriteString(tempstr);
		iof.WriteString(_T("\n"));

		tempstr=m_list.GetItemText(i,2);
		iof.WriteString(tempstr);
		iof.WriteString(_T("\n"));
	}
	iof.Close();
	return 1;
	}
	iof.Close();
	return 0;
}

int CschedulerDlg::restoreNote(void)
{
//  	FILE	*f;
// 	char	abuff[KS_MAX_NOTE];
//  	TCHAR	wbuff[KS_MAX_NOTE];
//  	int		count,i;
// 	CString tempstr;
// 
//  	fopen_s(&f,KS_CONF_FILE,"r+");
//  	if(f!=0)
//  	{
//  		fscanf_s(f,"%d",&count);
//  		for(i=0;i<count;i++)
//  		{
//  			fscanf_s(f,"%s",abuff,KS_MAX_NOTE);
//  			wbuff[0]=0;
// 			MultiByteToWideChar(CP_UTF8,0,abuff,-1,wbuff,KS_MAX_NOTE);
//  			m_list.InsertItem(i,wbuff);
//  
//  			fscanf_s(f,"%s",abuff,KS_MAX_NOTE);
//  			wbuff[0]=0;
//  			MultiByteToWideChar(CP_UTF8,0,abuff,-1,wbuff,KS_MAX_NOTE);
//  			m_list.SetItem(i,1,LVIF_TEXT ,wbuff,0,LVIF_STATE,0,0);
//  
//  			fscanf_s(f,"%s",abuff,KS_MAX_NOTE);
//  			wbuff[0]=0;
//  			MultiByteToWideChar(CP_UTF8,0,abuff,-1,wbuff,KS_MAX_NOTE);
//  			m_list.SetItem(i,2,LVIF_TEXT ,wbuff,0,LVIF_STATE,0,0);
//  		}
//  		fclose(f);
//  		return 1;
// 	}

	CStdioFile iof;
	int count;
	CString tempstr;

	if(iof.Open(_T(KS_CONF_FILE),CFile::modeRead))
	{
		iof.ReadString(tempstr);
		count=_wtoi(tempstr);
		for (int i=0;i<count;i++)
		{
			if(i>=KS_MAX_ITIMES) break;
			if(m_pData[i]==0) m_pData[i] = new ITEMDATA;

			m_pData[i]->index=i;

			iof.ReadString(tempstr);
			m_pData[i]->time=tempstr;
			m_list.InsertItem(i,tempstr);

			iof.ReadString(tempstr);
			m_pData[i]->note=tempstr;
			m_list.SetItem(i,1,LVIF_TEXT ,tempstr,0,LVIF_STATE,0,0);

			iof.ReadString(tempstr);
			m_pData[i]->comment=tempstr;
			m_list.SetItem(i,2,LVIF_TEXT ,tempstr,0,LVIF_STATE,0,0);

			m_list.SetItemData(i,(DWORD_PTR)m_pData[i]);
		}
		CreateThread(0,0,&my_thread,this,0,0);
		//showNote();
		iof.Close();
		return 1;
	}
	return 0;
}

void CschedulerDlg::OnLvnKeydownList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	if (pLVKeyDow->wVKey==VK_DELETE)
		OnBnClickedDel();
	*pResult = 0;
}
DWORD WINAPI my_thread(LPVOID lparam)
{
	CschedulerDlg * pmyscheduler = (CschedulerDlg *)lparam;
	pmyscheduler ->showNote(0);

	return 0;
}
DWORD WINAPI my_thread_storeNote(LPVOID lparam)
{
	CschedulerDlg * pmyscheduler = (CschedulerDlg *)lparam;
	pmyscheduler->storeNote();

	return 0;
}
void CschedulerDlg::OnBnClickedEditBtn()
{
	// TODO: Add your control notification handler code here
	ITEMDATA* pEditItemData;
	COleDateTime temptime,temphours;
	
	CString strtime;
	CString tempstr;
	int nitem;

	//get select item
	nitem=m_list.GetNextItem(-1,LVIS_FOCUSED|LVIS_SELECTED);
	pEditItemData=(ITEMDATA*)m_list.GetItemData(nitem);
	if(nitem ==-1) return;
//now + remain time
	m_now.GetTime(temptime);
	m_hours.GetTime(temphours);
	temptime=temptime+temphours;

	//strtime=temptime.Format(_T("%d  %H:%M"));
	strtime=time2str(&temptime);
	//edit item data
	pEditItemData->time=strtime;
	m_list.SetItemText(nitem,0,strtime);


	GetDlgItemText(IDC_FARM,tempstr);
	//edit item data
	pEditItemData->note=tempstr;
	m_list.SetItemText(nitem,1,tempstr);

	GetDlgItemText(IDC_COMMENT,tempstr);
	//edit item data
	pEditItemData->comment=tempstr;
	m_list.SetItemText(nitem,2,tempstr);
}

void CschedulerDlg::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	int iItem=pNMItemActivate->iItem;
	COleDateTime curtime;
	COleDateTime temptime;
	CString tempstr;
	CString notemsg;
	
	if(iItem!=-1)
	{
	//	m_nowChk.SetCheck(BST_UNCHECKED);
	//	m_timeSyn = 0;
		
		tempstr = m_list.GetItemText(iItem,0);
		if(temptime.ParseDateTime(tempstr,0))
		{
			curtime = COleDateTime::GetCurrentTime();
			temptime = temptime - curtime;			
		}
		else
			temptime.SetTime(0,0,0);
		//update remain time dlg
		m_hours.SetTime(temptime);
		//update the note edit
		tempstr = m_list.GetItemText(iItem,1);
		SetDlgItemText(IDC_FARM,tempstr);
		//update the conten edit
		tempstr = m_list.GetItemText(iItem,2);
		SetDlgItemText(IDC_COMMENT,tempstr);
	}
// 	else
// 	{
// 
// 	}
	*pResult = 0;
}

void CschedulerDlg::OnHdnItemclickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: Add your control notification handler code here
	if(phdr->iItem != -1)
	{
		m_list.SortItems(&MyCompareProc,phdr->iItem);
	}

	*pResult = 0;
}

int CALLBACK MyCompareProc(LPARAM lParam1, LPARAM lParam2, 
						   LPARAM lParamSort)
{
	int nRetVal;

	PITEMDATA pData1 = (PITEMDATA)lParam1;
	PITEMDATA pData2 = (PITEMDATA)lParam2;

	COleDateTime time1;
	COleDateTime time2;

	switch(lParamSort)
	{
	case 0:	// Last Name
		time1.ParseDateTime(pData1->time,0);
		time2.ParseDateTime(pData2->time,0);
		nRetVal = time1>time2;
		break;

	case 1:	// First Name
		nRetVal = pData1->note > pData2->note;
		break;

	case 2: // Term
		nRetVal = pData1->comment > pData2->comment;
		break;

	default:
		break;
	}

	return nRetVal;

}
int CschedulerDlg::delItem(int iItem)
{
	ITEMDATA * pItemData;
	int index;
	pItemData=(ITEMDATA*)m_list.GetItemData(iItem);
	index=pItemData->index;
	while(index < KS_MAX_ITIMES)
	{
		if (m_pData[index+1]==0) break;
		m_pData[index]=m_pData[index+1];
		m_pData[index]->index--;
		index++;
	}
	m_pData[index]=0;
	delete pItemData;

	m_list.DeleteItem(iItem);
	return 0;
}

CString CschedulerDlg::time2str(COleDateTime * time)
{
	return time->Format(_T("%m/%d/%Y  %H:%M:%S"));
}
//move or save the position of the dialog 
//option :  1 save posiont
//			0 set to last posiont

int CschedulerDlg::updatePosition(int option=KS_SAVEPOS)
{
	CRect currect;
	switch (option)
	{
	case KS_SAVEPOS://save current position
		GetWindowRect(currect);
		//GetClientRect(currect);
		//ClientToScreen(currect);
		m_app->WriteProfileInt(_T("dlgrect"),_T("top"),currect.top);
		m_app->WriteProfileInt(_T("dlgrect"),_T("left"),currect.left);
		m_app->WriteProfileInt(_T("dlgrect"),_T("bottom"),currect.bottom);
		m_app->WriteProfileInt(_T("dlgrect"),_T("right"),currect.right);
		break;
	case KS_SETPOS://set dlg to last positon
		currect.top=m_app->GetProfileInt(_T("dlgrect"),_T("top"),100);
		currect.left=m_app->GetProfileInt(_T("dlgrect"),_T("left"),100);
		currect.bottom=m_app->GetProfileInt(_T("dlgrect"),_T("bottom"),100);
		currect.right=m_app->GetProfileInt(_T("dlgrect"),_T("right"),100);
		MoveWindow(currect);
		break;
	}
	return 0;
}

void CschedulerDlg::OnOptionSetting()
{
	// TODO: Add your command handler code here
	m_settingDlg.m_pKSSetting=& m_ksSetting;
	m_settingDlg.DoModal();
}

int CschedulerDlg::updateSetting(void)
{
	CString timestr;

	timestr=m_app->GetProfileString(_T("ks_setting"),_T("alert_before_time"),0);

	m_ksSetting.alertBeforeChk=m_app->GetProfileInt(_T("ks_setting"),_T("alert_before_chk"),0);
	m_ksSetting.alertBeforeTime.ParseDateTime(timestr);

	return 0;
}

void CschedulerDlg::OnSize(UINT nType, int cx, int cy)
{
	CRect wndrect;
	static CRect prerect;

	__super::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	if (nType==SIZE_MINIMIZED)
	{

		// Add Icon to Systray
		TrayMessage(NIM_ADD);

		ShowWindow(SW_HIDE);
		return;
	}

	if(IsWindow(m_list.GetSafeHwnd()))
	{
		GetWindowRect(wndrect);
		if(!isValidRect(wndrect)) 
		{
			//fixInvalidRect(wndrect);			
			this->MoveWindow(prerect);
		}
		else
		{
			prerect=wndrect;
			resize(cx,cy);
		}
	}
}

int CschedulerDlg::resize(int cx, int cy)
{
	CRect gtop;
	CRect gbot;
	CRect wndrect(0,0,cx,cy);

	setSpaces(5,5,5,5,0,0);
	//gtop=wndrect;
	//gtop.bottom=gtop.top + KR_SCALE(25);
	gtop=grid((CWnd*)&m_sGroupT,KR_LEFT,KR_TOP,0,KR_SCALE(22),wndrect);
	wndrect.top=gtop.bottom;

	gbot=grid((CWnd*)&m_sGroup2,KR_LEFT,KR_BOT,0,0,wndrect);

	//resize nowchk ctrl and s_remain ctrl
	setSpaces(5,5,KR_SCALE(7),KR_SCALE(3),2,2);

	CRect lgtop,rgrop;
	//size of now chk and remain
	lgtop=grid(0,KR_LEFT,KR_TOP,KR_SCALE(120),0,gtop);
	//resize nowchk,.. remain
	grid((CWnd*)&m_nowChk,0,1,0,1,3,1,lgtop);
	grid((CWnd*)&m_now,1,2,0,1,3,1,lgtop);
	grid((CWnd*)&m_sRemain,3,1,0,1,3,1,lgtop);
	grid((CWnd*)& m_hours,4,2,0,1,3,1,lgtop);

	grid((CWnd*)& m_sSub,6,1,0,1,12,1,gtop);
	grid((CWnd*)& m_sub,7,2,0,1,12,1,gtop);
	grid((CWnd*)& m_sNote,9,1,0,1,12,1,gtop);
	grid((CWnd*)& m_note,10,2,0,1,12,1,gtop);
	//grid((CWnd*)&,,,,,,,gtop);
	CRect gbtn;
	setSpaces(0,2,0,2,2,2);
	gbtn.SetRect(0,0,KR_SCALE(120),KR_SCALE(15));
	gbtn=grid(0,KR_RIGHT,KR_BOT,gbtn,gbot);
	grid((CWnd*)&m_addBtn,0,1,0,1,3,1,gbtn);
	grid((CWnd*)&m_editBtn,1,1,0,1,3,1,gbtn);
	grid((CWnd*)&m_delBtn,2,1,0,1,3,1,gbtn);
	//
	setSpaces(5,5,12,5,0,0);
	gbot.DeflateRect(0,0,0,gbtn.Height());
	grid((CWnd*)&m_list,0,1,0,1,1,1,gbot);

	return 0;
}

void CschedulerDlg::OnFileQuit()
{
	// TODO: Add your command handler code here
	m_app->WriteProfileInt(_T("KScheduler"),_T("update_time"),m_timeSyn);
	KillTimer(1);
	storeNote();
	updatePosition(KS_SAVEPOS);

	//mutex
	ReleaseMutex(m_mutex);

	EndDialog(0);
}

void CschedulerDlg::OnOptionHide()
{
	// TODO: Add your command handler code here
	ShowWindow(SW_HIDE);
	TrayMessage(NIM_ADD);
}

// put a systemtray icon
BOOL CschedulerDlg::TrayMessage(DWORD dwMessage)
{
	CString sTip(_T("Schedule"));	

	NOTIFYICONDATA tnd;

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= IDR_TRAYICON;

	tnd.uFlags		= NIF_MESSAGE|NIF_ICON;

	tnd.uCallbackMessage	= MYWM_NOTIFYICON;


	VERIFY( tnd.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE (IDR_TRAYICON)) );

	tnd.uFlags		= NIF_MESSAGE|NIF_ICON|NIF_TIP;

	lstrcpyn(tnd.szTip, (LPCTSTR)sTip, sizeof(tnd.szTip)/sizeof(tnd.szTip[0]) );

	return Shell_NotifyIcon(dwMessage, &tnd);
}

void CschedulerDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	/*ShowWindow(SW_SHOW);*/
	//SetForegroundWindow();
	//SetFocus();

	__super::OnLButtonDblClk(nFlags, point);
}

LRESULT CschedulerDlg::OnSysIconClick(WPARAM wparam, LPARAM lparam)
{
	if(lparam == WM_LBUTTONDOWN)
	{
		ShowWindow(SW_NORMAL);
		SetForegroundWindow();
		SetFocus();
		TrayMessage(NIM_DELETE);
	}
	return 0;
}
