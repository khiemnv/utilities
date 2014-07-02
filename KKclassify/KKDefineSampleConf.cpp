#include "StdAfx.h"
#include "KKDefineSampleConf.h"


CKKDefineSampleConf::CKKDefineSampleConf(void)
{
	data=0;
}

CKKDefineSampleConf::~CKKDefineSampleConf(void)
{
	delete data;
}
BOOL CKKDefineSampleConf::writeConf(void)
{
	FILE *f;
	char buff[MAX_PATH];
	size_t	convertedCharsw=0;
	wcstombs_s(&convertedCharsw,buff,MAX_PATH,conf_file,_TRUNCATE);
	fopen_s(&f,buff,"wt");
	if(f!=0)
	{
		//write n_record
		fprintf_s(f,"n_record_: %d\n",n_record);
		//write tung record
		for(int j=0;j<n_record;j++)
		{
			//write loai vr
			convertedCharsw=0;
			wcstombs_s(&convertedCharsw,buff,MAX_PATH,data[j].loai_vr,_TRUNCATE);
			fprintf_s(f,"loai_vr__:%s\n",buff);
			//write ten file
			wcstombs_s(&convertedCharsw,buff,MAX_PATH,data[j].ten_file,_TRUNCATE);
			fprintf_s(f,"ten_file_:%s\n",buff);
			//write size min, size max
			fprintf_s(f,"size_m_M_: %d %d\n",data[j].min_size,data[j].max_size);
	
			//write so section
			fprintf_s(f,"n_section: %d\n",data[j].n_section);
			//write thong tin tung section
			for(int k=0;k<data[j].n_section;k++)
			{
				//write ten section
				wcstombs_s(&convertedCharsw,buff,MAX_PATH,data[j].s_names[k],_TRUNCATE);
				fprintf_s(f,"name_____:%s\n",buff);
				//write section size
				fprintf_s(f,"size_____: %d\n",data[j].s_sizes.get_at(k));
				//write md5
				wcstombs_s(&convertedCharsw,buff,MAX_PATH,data[j].s_md5s[k],_TRUNCATE);
				fprintf_s(f,"md5______:%s\n",buff);
			}

			//write array start codes
			fprintf_s(f,"n_start_c: %d\n",data[j].n_start_code);
			for(int k=0; k<data[j].n_start_code;k++)
			{
				//write offsets
				fprintf_s(f,"offset___: %u\n",data[j].offsets.get_at(k));
				//write hex strs
				wcstombs_s(&convertedCharsw,buff,MAX_PATH,data[j].hex_strs[k],_TRUNCATE);
				fprintf_s(f,"hex_str__:%s\n",buff);
			}
			//write dau phan cach
			fprintf_s(f,"##########\n");
			fclose(f);
			return TRUE;
		}

	
		fclose(f);
	}

	return FALSE;
}
BOOL CKKDefineSampleConf::readConf(void)
{
	FILE *f;
	char buff[MAX_PATH];
	int itemp,itemp2;
	UINT uitemp;
	size_t convertedCharsw=0;
	wcstombs_s(&convertedCharsw,buff,MAX_PATH,conf_file,_TRUNCATE);
	fopen_s(&f,buff,"rt");
	if(f!=0)
	{
		//read n_record
		fscanf_s(f,"%s%d",buff,MAX_PATH,&itemp);
		n_record=itemp;
		//cap phat vung nho de luu cac struct define_sample
		data=(DEFINE_SAMPLE*)new DEFINE_SAMPLE[n_record+MAX_N_DEFINE_SAMPLE_ADDED];
		//doc tung record
		for(int j=0;j<n_record;j++)
		{
			//read loai vr
			fscanf_s(f,"%s",buff,MAX_PATH);
			data[j].loai_vr=CString(buff+DEFINE_SAMPLE_COMMENT_LEN);
			//read ten file
			fscanf_s(f,"%s",buff,MAX_PATH);
			data[j].ten_file=CString(buff+DEFINE_SAMPLE_COMMENT_LEN);
			//read size min, size max
			fscanf_s(f,"%s%d%d",buff,MAX_PATH,&itemp,&itemp2);
			data[j].min_size=itemp;
			data[j].max_size=itemp2;

			//read so section
			fscanf_s(f,"%s%d",buff,MAX_PATH,&itemp);
			data[j].n_section=itemp;
			//doc thong tin tung section
			for(int k=0;k<data[j].n_section;k++)
			{
				//read ten section
				fscanf_s(f,"%s",buff,MAX_PATH);
				data[j].s_names.Add(CString(buff+DEFINE_SAMPLE_COMMENT_LEN));
				//read section size
				fscanf_s(f,"%s%d",buff,MAX_PATH,&itemp);
				data[j].s_sizes.add(itemp);
				//read md5
				fscanf_s(f,"%s",buff,MAX_PATH);
				data[j].s_md5s.Add(CString(buff+DEFINE_SAMPLE_COMMENT_LEN));
			}

			//read array start codes
			fscanf_s(f,"%s%d",buff,MAX_PATH,&itemp);
			data[j].n_start_code=itemp;
			for(int k=0; k<data[j].n_start_code;k++)
			{
				//read offsets
				fscanf_s(f,"%s%u",buff,MAX_PATH,&uitemp);
				data[j].offsets.add(uitemp);
				//read hex strs
				fscanf_s(f,"%s",buff,MAX_PATH);
				data[j].hex_strs.Add(CString(buff+DEFINE_SAMPLE_COMMENT_LEN));
			}
			//read dau phan cach
			fscanf_s(f,"%s",buff,MAX_PATH);
		}

		fclose(f);
		return TRUE;
	}

	return FALSE;
}

void CKKDefineSampleConf::updateCtrl(BOOL getset)
{
//int i;
	CString temp;
	switch(getset)
	{
	case FALSE: //set dlg value
		break;
	case TRUE:
		break;
	}
}
void CKKDefineSampleConf::getCtrl(CEdit*_pathEdit,CComboBox*_sqTickCmb,CComboBox*_classifyCmb)
{
}
void CKKDefineSampleConf::clean()
{	
}
bool CKKDefineSampleConf::add_record(DEFINE_SAMPLE& rval)
{
	//neu bo nho da day thi bao loi
	if (n_record==data_max_size)
		return false;
	
	data[n_record].loai_vr=rval.loai_vr;
	data[n_record].ten_file=rval.ten_file;
	data[n_record].min_size=rval.min_size;
	data[n_record].max_size=rval.max_size;

	data[n_record].n_section=rval.n_section;
	data[n_record].s_names.Copy(rval.s_names);
	data[n_record].s_sizes.copy(rval.s_sizes);
	data[n_record].s_md5s.Copy(rval.s_md5s);

	data[n_record].n_start_code=rval.n_start_code;
	data[n_record].offsets.copy(rval.offsets);
	data[n_record].hex_strs.Copy(rval.hex_strs);

	return true;
}
bool CKKDefineSampleConf::refresh()
{
	delete data;
	readConf();
	return true;
}