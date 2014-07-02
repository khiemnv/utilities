#ifndef _KMYSQL_HFILE_
#define _KMYSQL_HFILE_
#include <mysql.h>
#include <string.h>
#include <stdlib.h>
#include <afxcoll.h>

#include "stdafx.h"
//#include "qlXuatNhap.h"
// #include <stdio.h>
// #include <winsock.h>
 #pragma comment (lib, "libmysql.lib")
#define CKMYSQL_BUFF_LENGTH 1024
/************************************************************************/
/* class KMySQL                                                         */
/************************************************************************/
class KMySQL{
private:
	char *table;
	char *host;
	char *user;
	char *passwd;
	unsigned int port;
	char *db;
	char *buffer;
	MYSQL *mysql;
	MYSQL_RES *result;
	MYSQL_ROW_OFFSET first_rows;
	//store the MYSQL_ROW_OFFSET of first row in result to seek back

	char* Kstrcpy_c(char* &des_str, char *src_str);
	char* Kstrcpy_r(char* &des_str, char *src_str);
public:
//	KMySQL(CStringA server_name, CStringA db_user, CStringA db_userpass, int server_port, CStringA db_name);
	KMySQL(CString server_name, CString db_user, CString db_userpass, int server_port, CString db_name);
	KMySQL(char *_table,char *_server_name,char *_db_user,char *_db_userpass,unsigned int _port, char *_db_name);
	KMySQL(char *_server_name,char *_db_user,char *_db_userpass,unsigned int _port, char *_db_name);
	KMySQL();

	//Returns the number of rows changed/deleted/inserted by the last UPDATE, DELETE, or INSERT query.
	my_ulonglong affected_rows()
	{
		return mysql_affected_rows(mysql);
	}

	//close connection to server
	void		close();
	//connect to server, you should call select_db after connect success
	MYSQL*		connect();
	//create database has name: "db"
	bool		create_database();
	//create table has name: "table"
	bool		create_table(char**_fields,char**_data_types,int _n);
	//drop database: "db"
	bool		delete_database();
	//drop table: "table"
	bool		delete_table();
	//clean table ("table") content
	bool		emplty_table();
	//free result
	void		free_res();
	//get one item at (row i,col j) in table AND SEEK to row i
	char*		get_item(unsigned int _row,unsigned int _col);
	//get one item at (row,col)
	const char*	get_item_q(unsigned int _row,unsigned int _col);

	const char*	get_last_error();
	char*		get_last_query();

	//total row of last result
	my_ulonglong	get_row_count()
	{
		return	result->row_count;
	}

	const char*	get_server_info();

	void		init();
	my_ulonglong	insert_id();
	//insert a new row in the table
	my_ulonglong	insert_row(char **_values,int number_fields);
	my_ulonglong	insert_row(CString *_values,int number_fields);
	my_ulonglong	insert_row(CString _table,CString *_values,int number_fields);
	/*my_ulonglong	insert_row(CString _table,CStringArray* _values);*/
	//load data from file to the table
	//vd: file="d:/pet.txt" line_terminate = "\\r\\n"
	bool		load_data(char*_file,char*_line_terminate);
	//make condition str
	//vd: "filed[0] = value[0] AND file[1] = value[1]"
	char*		mk_and_eq(char**_fields,char**_values,int _n);
	char*		mk_and_ne(char**_fields,char**_values,int _n);
	char*		mk_or_eq(char**_fields,char**_values,int _n);
	char*		mk_or_ne(char**_fields,char**_values,int _n);
	MYSQL_RES*	list_table(const char *wild);

	bool		query(const char *stmt_str);
	bool		query(CString stmt_str);
	MYSQL_RES*	query_and_store(const char *stmt_str);
	MYSQL_RES*	query_and_store(CString *stmt_str);
	//connect to server include select database
	MYSQL*		real_connect();
	//delete rows where match the condition
	my_ulonglong	delete_rows(char* _condition);
	//reset connect with all new vars
	void		reset(char *_table,char *_server_name,char *_db_user,char *_db_userpass,unsigned int _port, char *_db_name);

	MYSQL_ROW_OFFSET row_seek(MYSQL_ROW_OFFSET _offset);
	//seek to row n in the result
	//the row n is now the current row
	MYSQL_ROW	row_seek(unsigned int _n_rows);
	//seek to the first row in the result
	void		seek_back();
	//select database: "db" (where table is placed)
	bool		select_database();
	bool		select_database(const char *_db_name);
	//select all col (row) where match the condition
	bool		select_cols(char**_fields,int _n,char*_conditon);
	bool		select_rows(char * _conditon);
	//chon table
	bool		select_table(char* table_name);
	//store result
	MYSQL_RES*	store_result();
	//show result
	void		show_result();
	//show table content
	bool		show_table();
	bool		show_table(char *table_name);
	//edit rows where condition match
	my_ulonglong	update(char**_files,char**_new_values,int _n,char* _conditon);
	int table_id;
	MYSQL_RES * get_res(void);
	//CStringArray getFieldsName(void);
};
#endif