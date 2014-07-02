
// schedulerDlg.h : header file
//

#pragma once
#include "afxdtctl.h"
#include "afxcmn.h"
#include "afxwin.h"
#include <afx.h>

#include "KResizing.h"
#include "SettingDlg.h"

#define KS_CONF_FILE	"ksconfig.txt"
#define KS_MAX_NOTE		100
#define KS_SAVEPOS 1
#define KS_SETPOS 0

#define KS_MAX_ITIMES	100
#define  KS_DELAY	1*60*1000	//1 minute
#define  KS_STORE_TIMER	10*60*1000	//10 minute

#define MYWM_NOTIFYICON		(WM_USER+2)

typedef struct ks_item_data_tag{
	int index;
	CString time;
	CString note;
	CString comment;
} ITEMDATA, *PITEMDATA;

// CschedulerDlg dialog
class CschedulerDlg : public CDialog, public CKResizing
{
// Construction
public:
	CschedulerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SCHEDULER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CDateTimeCtrl m_now;
	CListCtrl m_list;

	CDateTimeCtrl m_hours;
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedDel();

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CButton m_nowChk;
	afx_msg void OnClose();
	UINT showNote(LPVOID lparam = 0);
	afx_msg void OnBnClickedCheck1();
	CWinApp* m_app;
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);

	UINT m_timeSyn;
	int storeNote(void);
	int restoreNote(void);
	afx_msg void OnLvnKeydownList1(NMHDR *pNMHDR, LRESULT *pResult);
	CButton m_editBtn;
	afx_msg void OnBnClickedEditBtn();
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnItemclickList1(NMHDR *pNMHDR, LRESULT *pResult);
	ITEMDATA** m_pData;
	int delItem(int iItem);
	CString time2str(COleDateTime * time);
	int updatePosition(int);
	afx_msg void OnOptionSetting();
	KS_SETTING m_ksSetting;
	CSettingDlg m_settingDlg;
	int updateSetting(void);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	int resize(int cx, int cy);
	CStatic m_sGroupT;
	CStatic m_sGroup2;
	CStatic m_sRemain;
	CStatic m_sSub;
	CStatic m_sNote;
	CEdit m_sub;
	CEdit m_note;
	CButton m_addBtn;
	CButton m_delBtn;
	HANDLE m_mutex;
	afx_msg void OnFileQuit();
	afx_msg void OnOptionHide();
	// put a systemtray icon
	BOOL TrayMessage(DWORD dwMessage);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg LRESULT OnSysIconClick(WPARAM wparam, LPARAM lparam);
};
/*void ( CALLBACK* lpfnTimer )(HWND, UINT, UINT, DWORD)*/

DWORD WINAPI my_thread(LPVOID lparam);
DWORD WINAPI my_thread_storeNote(LPVOID lparam);

int CALLBACK MyCompareProc(LPARAM lParam1, LPARAM lParam2, 
						   LPARAM lParamSort);