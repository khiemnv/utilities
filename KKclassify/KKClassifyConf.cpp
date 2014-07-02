#include "StdAfx.h"
#include "KKClassifyConf.h"

CKKClassifyConf::CKKClassifyConf(void)
{
}

CKKClassifyConf::~CKKClassifyConf(void)
{
}
BOOL CKKClassifyConf::writeConf(void)
{
	FILE *f;
	/*char buff[MAX_PATH];*/
	int i;
	fopen_s(&f,strToAstr(conf_file),"wt");
	if(f!=0)
	{
		//print path
		fprintf_s(f,"path---:%s\n",strToAstr(path));
		//print state list
		fprintf_s(f,"SQTstates-: %d\n",SQTstates.GetSize());
		for(i=0;i<SQTstates.GetSize();i++)
			fprintf(f,"%s\n",strToAstr(SQTstates.GetAt(i)));
		//write classifySpecies list
		fprintf_s(f,"classifySpecies: %d\n",classifySpecies.GetSize());

		for(i=0;i<classifySpecies.GetSize();i++)
			fprintf_s(f,"%s\n",strToAstr(classifySpecies.GetAt(i)));

		fclose(f);
	}

	return FALSE;
}
BOOL CKKClassifyConf::readConf(void)
{
	FILE *f;
	char buff[MAX_PATH];
	int i;
	fopen_s(&f,strToAstr(conf_file),"rt");
	if(f!=0)
	{
		//read path
		fscanf_s(f,"%s",buff,MAX_PATH);
		path = CString(buff+KKCOMENTLENGHT);
		//read state list
		if(!fscanf_s(f,"%s%d",buff,MAX_PATH,&i))
			return FALSE;

		while(i>0)
		{
			i--;
			fscanf_s(f,"%s",buff,MAX_PATH);
			SQTstates.Add(CString(buff));
		}
		//read classifySpecies list
		if(!fscanf_s(f,"%s%d",buff,MAX_PATH,&i))
			return FALSE;

		while(i>0)
		{
			i--;
			fscanf_s(f,"%s",buff,MAX_PATH);
			classifySpecies.Add(CString(buff));
		}

		fclose(f);
	}

	return FALSE;
}

void CKKClassifyConf::updateCtrl(BOOL getset)
{
	int i;
	CString temp;
	switch(getset)
	{
	case FALSE: //set dlg value
		ptrPathEdit->SetFocus();
		ptrPathEdit->SetWindowText(path);
		//set classify combo list
		for(i=0;i<classifySpecies.GetSize();i++)
			pClassifyCmb->AddString(classifySpecies.GetAt(i));
		//set state combo list
		for(i=0;i<SQTstates.GetSize();i++)
			pSQTickCmb->AddString(SQTstates[i]);
		break;
	case TRUE:
		clean();
		ptrPathEdit->GetWindowText(path);
		//get classify classifySpecies		
		for(i=0;i<pClassifyCmb->GetCount();i++)
		{
			pClassifyCmb->GetLBText(i,temp);
			classifySpecies.Add(temp);
		}
		for(i=0;i<pSQTickCmb->GetCount();i++)
		{
			pSQTickCmb->GetLBText(i,temp);
			SQTstates.Add(temp);
		}
		break;
	}
}
void CKKClassifyConf::getCtrl(CEdit*_pathEdit,CComboBox*_sqTickCmb,CComboBox*_classifyCmb)
{
	ptrPathEdit=_pathEdit;
	pSQTickCmb=_sqTickCmb;
	pClassifyCmb=_classifyCmb;
}
void CKKClassifyConf::clean()
{	
	path.Empty();
	SQTstates.RemoveAll();
	classifySpecies.RemoveAll();
}