#pragma once
#include "afxcmn.h"
#include "listext.h"
#include "afxwin.h"
#include "kkdir.h"
#include "kkclassifyconf.h"
#include "KKFile.h"
// #include "KKclassifyDlg.h"

#define KKCLASSIFY_CONF_FILE	"classifyDlgConf.txt"
#define KKCOMENTLENGHT	8
#define KK_SETLVI		1
#define KK_CLASSIFY_COL	1
#define KK_SQTSTATE_COL	2
/*#define SIZE_SCALE	11/7*/
//classify dlg config
typedef struct
{
	CString		conf_file;
	CString		path;
	CStringArray SQTstates;
	CStringArray classifySpecies;
}KKCLASSIFYDLGCONF;

// CClassifyDlg dialog

class CClassifyDlg : public CDialog
{
	DECLARE_DYNAMIC(CClassifyDlg)

public:
	CClassifyDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CClassifyDlg();

// Dialog Data
	enum { IDD = IDD_CLASSIFY_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	CListCtrl m_cMainList;
	listExt m_cListEx;
	CListCtrl m_cFInfoList;
	CButton m_cPathBtn;
	CEdit m_cPathEdit;
	int resize(int cx, int cy);
	afx_msg void OnBnClickedPathBtn();
	CStatic m_cStaticClassify;
	CStatic m_cStaticSQTick;
	CButton m_cProcessBtn;
	CComboBox m_cClassifyCmb;
	CEdit m_cSqickTickEdt;
	CComboBox m_cSQTickCmb;
	CListBox m_cCommentList;
	CKKDir m_cPickedDir;
	afx_msg void OnEnChangePathEdit();
// 	afx_msg void onPathEditDropFiles();
// 	afx_msg void onMainListDropFiles();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	/*afx_msg	LRESULT onDrop();*/
	void refresh(void);

// 	BOOL readConf(void);
// 	BOOL writeConf(void);
	BOOL writeProfile();
	BOOL readProFile();

	char* strTtoAstr(CString strT);
	char* strTtoAstr(CString *strT)
	{
		CString newStrT=*strT;
		return strTtoAstr(newStrT);
	}
	/*KKCLASSIFYDLGCONF	config;*/
	CKKClassifyConf config;
	
	afx_msg void OnClose();
	afx_msg void OnCbnSelchangeClassifyCombo();
	void updateClassify(int);
	int	m_vPreMLRow;
//	afx_msg void OnLvnItemchangedMainList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickMainList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnEditchangeClassifyCombo();
//	afx_msg void OnCbnEditupdateClassifyCombo();
//	afx_msg void OnCbnSetfocusClassifyCombo();
//	afx_msg void OnCbnKillfocusClassifyCombo();
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnSysChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	void add_col(CListCtrl &list,int col, CString& heading);
	void add_item(CListCtrl &list, int row, int col, CString &content);
	void add_row(CListCtrl &list,CString* contents, int n_col);

	afx_msg void OnNMDblclkMainList(NMHDR *pNMHDR, LRESULT *pResult);
};
