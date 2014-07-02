#pragma once
#include "KKString.h"
#ifdef UNICODE
#define strToAstr CKKString::cstrWtoAstr
#endif
#define KKCOMENTLENGHT 8
#define KKSET_DLG_VALUE FALSE
#define KKGET_DLG_VALUE TRUE

class CKKClassifyConf
{
public:
	CKKClassifyConf(void);    
	~CKKClassifyConf(void);
	//
	void clean();
	BOOL readConf(void);
	BOOL writeConf(void);
	void updateCtrl(BOOL);
	BOOL loadConf(CKKClassifyConf* pClassifyConf);
	BOOL saveConf(CKKClassifyConf* pClassifyConf);
	void getCtrl(CEdit*path,CComboBox*sqTick,CComboBox*classsify);

	CString getClassifySpecie(int i){return classifySpecies[i];}
	CString getSQTState(int i){return SQTstates[i];}
	int		getSpeciesCount(){return classifySpecies.GetSize();}
	int		getSQTStateCount(){return SQTstates.GetSize();}
	void	addSQTstate(CString state){SQTstates.Add(state);}
	void	addClassifySpecies(CString species){classifySpecies.Add(species);}
	//
	CString		conf_file;
	CString		path;
	CStringArray SQTstates;
	CStringArray classifySpecies;
	CEdit*		ptrPathEdit;
	CComboBox*	pSQTickCmb;
	CComboBox*	pClassifyCmb;

};
