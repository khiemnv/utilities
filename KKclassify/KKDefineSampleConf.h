#pragma once
#define KKDEFINESAMPLE_CONF_FILE "defineSampleConf.txt"
#include "KKString.h"
#include "kkintarray.h"
#include "KKUintArray.h"
#define MAX_N_DEFINE_SAMPLE_ADDED	10
#define DEFINE_SAMPLE_COMMENT_LEN	10

#ifndef _define_sample_struct
#define _define_sample_struct
typedef struct _define_sample
{
	CString loai_vr;
	CString ten_file;
	int	min_size;
	int max_size;

	int n_section;
	CStringArray s_names;
	KKIntArray s_sizes;
	CStringArray s_md5s;

	int n_start_code;
	KKUintArray offsets;
	CStringArray hex_strs;
}DEFINE_SAMPLE;
#endif

class CKKDefineSampleConf
{
public:
	CKKDefineSampleConf(void);
	~CKKDefineSampleConf(void);
	//
	void clean();
	BOOL readConf(void);
	BOOL writeConf(void);
	void updateCtrl(BOOL);
// 	BOOL loadConf(CKKClassifyConf* pClassifyConf);
// 	BOOL saveConf(CKKClassifyConf* pClassifyConf);
	void getCtrl(CEdit*path,CComboBox*sqTick,CComboBox*classsify);
	bool add_record(DEFINE_SAMPLE& rval);
	bool refresh();
	//
	CString		conf_file;
	CString		path;
	int	n_record;
	int data_max_size;
	DEFINE_SAMPLE *data;
};
