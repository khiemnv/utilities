#include "stdafx.h"
#include "Kmysql.h"
#include "QLXuatNhap.h"

/*#include "Kmysql.h"*/
/************************************************************************/
/* implement constructor                                                */
/************************************************************************/
/*first u NEED FREE the des_str and SET des_str = 0
u DONT have alloc a memory for des_str cause i will do it.*/
char * KMySQL::Kstrcpy_c(char* &des_str, char *src_str){

	//if (des_str != 0) free(des_str);		
	des_str = (char*) calloc(sizeof(char),strlen(src_str) + 1);
	strcpy (des_str,src_str);
	return des_str;
}


char * KMySQL::Kstrcpy_r(char* &des_str, char *src_str){	
	des_str = (char*)realloc(des_str,sizeof(char) * (strlen(src_str) + 1));
	strcpy (des_str,src_str);
	return des_str;
}

/*constructor*/
KMySQL::KMySQL(CString server_name, CString db_user, CString db_userpass, int server_port, CString db_name)
: table_id(0)
{
	table=0;
	size_t newsizew = (server_name.GetLength() + 1)*2;
	host = new char[newsizew];
	size_t convertedCharsw = 0;
	wcstombs_s(&convertedCharsw, host, newsizew, server_name, _TRUNCATE );
	
	newsizew = (db_user.GetLength() + 1)*2;
	user = new char[newsizew];
	wcstombs_s(&convertedCharsw, user, newsizew, db_user, _TRUNCATE );

	newsizew = (db_userpass.GetLength() + 1)*2;
	passwd = new char[newsizew];
	wcstombs_s(&convertedCharsw, passwd, newsizew, db_userpass, _TRUNCATE );
// 	user = 0; Kstrcpy_c(user,temp);
// 	passwd = 0; Kstrcpy_c(passwd,temp);

	newsizew = (db_name.GetLength() + 1)*2;
	db = new char[newsizew];
	wcstombs_s(&convertedCharsw, db, newsizew, db_name, _TRUNCATE );
// 	db = 0; Kstrcpy_c(db,temp);

	port = server_port;
	mysql = mysql_init(NULL);
	buffer = 0; buffer = (char*)calloc(sizeof(char),CKMYSQL_BUFF_LENGTH);
}
KMySQL::KMySQL(char *_table,char *_server_name,char *_db_user,char *_db_userpass,unsigned int _port, char *_db_name){
	table = 0; Kstrcpy_c(table,_table);
	host = 0; Kstrcpy_c(host,_server_name);
	user = 0; Kstrcpy_c(user,_db_user);
	passwd = 0; Kstrcpy_c(passwd,_db_userpass);
	db = 0; Kstrcpy_c(db,_db_name);
	port = _port;
	mysql = mysql_init(NULL);
	buffer = 0; buffer = (char*)calloc(sizeof(char),CKMYSQL_BUFF_LENGTH);
}
KMySQL::KMySQL(char *_server_name,char *_db_user,char *_db_userpass,unsigned int _port, char *_db_name){
	table = 0;
	host = 0; Kstrcpy_c(host,_server_name);
	user = 0; Kstrcpy_c(user,_db_user);
	passwd = 0; Kstrcpy_c(passwd,_db_userpass);
	db = 0; Kstrcpy_c(db,_db_name);
	port = _port;
	mysql = mysql_init(NULL);
	buffer = 0; buffer = (char*)calloc(sizeof(char),CKMYSQL_BUFF_LENGTH);
}
KMySQL::KMySQL(){
	init();
}
void KMySQL::init(){
	table = 0;
	host = 0;
	user = 0;
	passwd = 0;
	db = 0;
	port = 0;
	mysql = 0;
	buffer = 0;
}

/*cal it if you ALREADY call init() or class already CONSTRUCTED*/
void KMySQL::reset(char *_table,char *_server_name,char *_db_user,char *_db_userpass,unsigned int _port, char *_db_name){
	Kstrcpy_r(table,_table);
	Kstrcpy_r(host,_server_name);
	Kstrcpy_r(user,_db_user);
	Kstrcpy_r(passwd,_db_userpass);
	Kstrcpy_r(db,_db_name);
	port = _port;
	mysql_close(mysql);
	mysql = mysql_init(NULL);
	buffer = (char*)realloc(buffer,sizeof(char) * CKMYSQL_BUFF_LENGTH);
}
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
/* implement all methods                                                */
/************************************************************************/
const char* KMySQL::get_server_info(){
	return mysql_get_server_info(mysql);
}
/*+++++++++++++++++++++++++++++++++++*/
MYSQL * KMySQL::real_connect(){
	mysql = mysql_real_connect(mysql,host,user,passwd,db,port,NULL,0);
	if(mysql == NULL)
		sprintf(buffer,"Problem encountered connecting to the %s database on %s.\n",db,host);
	return mysql;
}
/*+++++++++++++++++++++++++++++++++++*/
/*Causes the database specified by db
to become the default (current) database on the 
connection specified by mysql. In subsequent queries, 
this database is the default for table references that do not 
include an explicit database specifier.*/
bool KMySQL::select_database(const char *_db_name){
	if (mysql_select_db(mysql,_db_name)){
		sprintf(buffer, "Error: %s\n",
			mysql_error(mysql));
		return false;
	}
	else {		
		return true;
	}
}
/*+++++++++++++++++++++++++++++++++++*/
MYSQL_ROW_OFFSET KMySQL::row_seek(MYSQL_ROW_OFFSET _offset){
	return mysql_row_seek(result, _offset);
}
/*+++++++++++++++++++++++++++++++++++*/
my_ulonglong KMySQL::insert_id(){
	return mysql_insert_id(mysql);
}
/*+++++++++++++++++++++++++++++++++++*/
/*you DONT need call store_result after query success cause
i ve done it*/
MYSQL_RES * KMySQL::query_and_store(const char *stmt_str){

	char *sql = buffer; // sql statement used to get all rows		
	sprintf(sql,"%s\0\0",stmt_str);
	if (mysql_query(mysql, sql)){
		//sprintf(buffer, "Error: %s\n",mysql_error(mysql));
		//store_result();
		return 0;
	}
	else {
		return store_result();		
	}
}
/*+++++++++++++++++++++++++++++++++++*/
/*Executes the SQL statement pointed to by 
the null-terminated string stmt_str. 
Normally, the string must consist of a single SQL statement and 
you should not add a terminating semicolon (“;”) or \g to the statement. 
If multiple-statement execution has been enabled, 
the string can contain several statements separated by semicolons.*/
/*you NEED call store_result after query success*/
bool KMySQL::query(const char *stmt_str){

	char *sql = buffer; // sql statement used to get all rows		
	sprintf(sql,"%s\0\0",stmt_str);
	if (mysql_query(mysql, sql)){
		sprintf(buffer, "Error: %s\n",
			mysql_error(mysql));
		return false;
	}
	else {
		return true;
	}
}
bool KMySQL::query(CString stmt_str)
{
	char *q = buffer; // sql statement used to get all rows		

	size_t newsizew = (stmt_str.GetLength() + 1)*2;
	size_t convertedCharsw = 0;

	wcstombs_s(&convertedCharsw, q, newsizew, stmt_str, _TRUNCATE );
	
	/*sprintf(sql,"%s\0\0",stmt_str);*/
	if (mysql_query(mysql,q))
	{
		/*sprintf(buffer, "Error: %s\n",mysql_error(mysql));*/
		return false;
	}
	else {
		return true;
	}
}
/*+++++++++++++++++++++++++++++++++++*/
/*Returns a result set consisting of table names in the 
current database that match the simple 
regular expression specified by the wild parameter. 
wild  may contain the wildcard characters “%” or “_”, or 
may be a NULL pointer to match all tables. 
Calling mysql_list_tables() is similar to 
executing the query SHOW TABLES [LIKE wild].
You must free the result set with mysql_free_result().*/
MYSQL_RES* KMySQL::list_table(const char *wild){
	result = mysql_list_tables(mysql, wild);
	if (result) 
		return result;
	else {
		sprintf(buffer, "Error: %s\n",
			mysql_error(mysql));
		return 0;
	}
}
/*+++++++++++++++++++++++++++++++++++*/
/*store result and the first rows*/
MYSQL_RES* KMySQL::store_result(){
	result = mysql_store_result(mysql);
	if(result){
		first_rows = result->data_cursor;
		return result;
	}
	else {
		sprintf(buffer, "Error: %s\n",
			mysql_error(mysql));
		return 0;
	}
}
/*+++++++++++++++++++++++++++++++++++*/
/*the result will be free after show*/
void KMySQL::show_result(){	
	MYSQL_ROW row;
	unsigned int num_fields;
	unsigned int i;

	num_fields = mysql_num_fields(result);
	while ((row = mysql_fetch_row(result)))
	{
		unsigned long *lengths;
		lengths = mysql_fetch_lengths(result);
		for(i = 0; i < num_fields; i++)
		{			
			printf("%s",row[i] ? row[i] : "NULL");
			int col_size = (80/num_fields);
			col_size = col_size < 20 ? col_size : 20;
			for(int j = lengths[i] ? (int)lengths[i] : 4;
				j < col_size; j++)
				printf(" ");
		}
		printf("\n");
	}
	//mysql_free_result(result);
	//return 0;
}

/*get item at row i >=0, col(filed) j >=0 of result
NOTE i HAVENT SEEK BACK to the first row yet*/
char * KMySQL::get_item(unsigned int _row,unsigned int _col){	
	char *item = buffer;
	unsigned int numf = mysql_num_fields(result);
	if(_col > numf) return NULL;

	MYSQL_ROW row = mysql_fetch_row(result);
	for(unsigned int j = 0; j < _row; j++ )
		row = mysql_fetch_row(result);
	if(row == NULL) return NULL;

	unsigned long *lengths = mysql_fetch_lengths(result);	
	sprintf(item,"%s",row[_col]);

	//	seek_back();//seek back to the first row
	return item;
}
/*an faster method to get item at (row>=0,col>=0)*/
const char * KMySQL::get_item_q(unsigned int _row,unsigned int _col){	

	MYSQL_ROWS *rows = result->data_cursor;

	for(unsigned int j = 0; j < _row; j++ ){
		rows = rows->next;
		if(rows == NULL) return NULL;
	}
	//	MYSQL_ROW row = rows->data;
	//	if (item != 0) free (item);
	//	item = (char*)calloc(sizeof(char),1 + strlen(row[_col]));
	//	sprintf(item,"%s",row[_col]);

	return rows->data[_col];
}
//seek back to the first row in result data
void KMySQL::seek_back(){
	mysql_row_seek(result,first_rows);
}

/*seek to next n rows from current row*/
MYSQL_ROW KMySQL::row_seek(unsigned int _n_rows){
	MYSQL_ROWS *rows = result->data_cursor;

	for(unsigned int j = 0; j < _n_rows; j++ ){
		rows = rows->next;
		if(rows == NULL) return NULL;
	}

	mysql_row_seek(result,rows);
	return rows->data;
}

/*insert a row to table*/
my_ulonglong KMySQL::insert_row(char **_row,int number_fields){
	char *q = buffer;
	//INSERT INTO pet VALUES ('Puffball2'
	//,'Diane2','hamster2','f','1999-03-30',NULL
	//)
	int temp = sprintf(q,"INSERT INTO %s VALUES (%s",table,_row[0]);
	for(int j = 1; j < number_fields; j++)
		temp += sprintf(q+temp,",%s",_row[j]);		
	sprintf(q+temp,")");
	if (query(q))
		return mysql_affected_rows(mysql);
	else
		return 0;
}
my_ulonglong KMySQL::insert_row(CString *_values,int number_fields){
	char *q = buffer;
	//INSERT INTO pet VALUES ('Puffball2'
	//,'Diane2','hamster2','f','1999-03-30',NULL
	//)
	/*int temp = sprintf(q,"INSERT INTO %s VALUES (%s",table,_row[0]);*/
	CString temp;
	/*temp=CString("INSERT INTO ") + CString(table) + CString(" VALUES (") + _values[0];*/
	temp.Format(_T("INSERT INTO %s VALUES("),CString(table));

	int j;
	for(j = 0; j < number_fields; j++)
	{
		if (_values[j].IsEmpty())
			temp+=CString("NULL,");
		else
		{
 			temp.Append(_T("'"));
 			temp.Append(_values[j]);
 			temp.Append(_T("',"));
		}
		/*temp+=CString::Format(_T("'%s',"),_values[j]);*/
	}

	temp.TrimRight(',');
	temp.Append(_T(");"));

	size_t newsizew = (temp.GetLength() + 1)*2;
	size_t convertedCharsw = 0;
	wcstombs_s(&convertedCharsw, q, newsizew, temp, _TRUNCATE );
	//wcstombs(q,temp,CKMYSQL_BUFF_LENGTH);

	if (mysql_query(mysql, q)){
		sprintf(buffer, "Error: %s\n",
			mysql_error(mysql));
		return 0;
	}
	else {
		return mysql_affected_rows(mysql);
	}
// 	if (query(q))
// 		return mysql_affected_rows(mysql);
// 	else
// 		return 0;
}
my_ulonglong KMySQL::insert_row(CString _table,CString* _values,int number_fields)
{
 	char *q = buffer;
// 	//INSERT INTO pet VALUES ('Puffball2'
// 	//,'Diane2','hamster2','f','1999-03-30',NULL
// 	//)
// 	/*int temp = sprintf(q,"INSERT INTO %s VALUES (%s",table,_row[0]);*/
 	CString temp;
	/*temp=CString("INSERT INTO ") + CString(table) + CString(" VALUES (") + _values[0];*/
	temp.Format(_T("INSERT INTO %s VALUES("),_table);

	int j;
	for(j = 0; j < number_fields; j++)
	{
		if (_values[j].IsEmpty())
			temp.Append(_T("NULL,"));
		else
			temp.AppendFormat(_T("'%s',"),_values[j]);		
	}

	temp.TrimRight(',');
	temp.Append(_T(");"));

	size_t newsizew = (temp.GetLength() + 1)*2;
	size_t convertedCharsw = 0;
	wcstombs_s(&convertedCharsw, q, newsizew, temp, _TRUNCATE );
	//wcstombs(q,temp,CKMYSQL_BUFF_LENGTH);

	if (mysql_query(mysql, q)){
		sprintf(buffer, "Error: %s\n",
			mysql_error(mysql));
		return 0;
	}
	else {
		return mysql_affected_rows(mysql);
	}
	// 	if (query(q))
	// 		return mysql_affected_rows(mysql);
	// 	else
	return 0;
}
/*show table content*/
bool KMySQL::show_table(){
	char * q = buffer;
	sprintf(q,"SELECT * FROM %s",table);
	if(query(q)){
		if(store_result()){
			show_result();
			return true;
		}
		else return false;
	}
	else return false;
}
bool KMySQL::show_table(char *table_name){
	char * q = buffer;
	sprintf(q,"SELECT * FROM %s",table_name);
	if(query(q)){
		if(store_result()){
			show_result();
			return true;
		}
		else return false;
	}
	else return false;
}

/*make and condition*/
char * KMySQL::mk_and_eq(char**_fields,char**_values,int _n){
	char * condition = buffer + (CKMYSQL_BUFF_LENGTH/3);
	//species = 'dog' AND sex = 'f'
	int temp = sprintf(condition,"(%s = %s",_fields[0],_values[0]);
	for(int j = 1; j< _n;j++){
		temp += sprintf(condition+temp," AND %s = %s",_fields[j],_values[j]);
	}
	sprintf(condition+temp,")");
	//printf("%s\n",condition);
	return condition;
}
char * KMySQL::mk_and_ne(char**_fields,char**_values,int _n){
	char * condition = buffer + (CKMYSQL_BUFF_LENGTH/3);
	//species = 'dog' AND sex = 'f'
	int temp = sprintf(condition,"(%s != %s",_fields[0],_values[0]);
	for(int j = 1; j< _n;j++){
		temp += sprintf(condition+temp," AND %s != %s",_fields[j],_values[j]);
	}
	sprintf(condition+temp,")");
	//printf("%s\n",condition);
	return condition;
}
/*make or conditon*/
char * KMySQL::mk_or_eq(char**_fields,char**_values,int _n){
	char * condition = buffer + (CKMYSQL_BUFF_LENGTH / 3 * 2);
	// (name = species = 'dog' OR sex = 'f')
	int temp = sprintf(condition,"(%s = %s",_fields[0],_values[0]);
	for(int j = 1; j< _n;j++){
		temp += sprintf(condition+temp," OR %s = %s",_fields[j],_values[j]);
	}
	sprintf(condition+temp,")");
	return condition;
}
char * KMySQL::mk_or_ne(char**_fields,char**_values,int _n){
	char * condition = buffer + (CKMYSQL_BUFF_LENGTH / 3 * 2);
	// (name = species = 'dog' OR sex = 'f')
	int temp = sprintf(condition,"(%s != %s",_fields[0],_values[0]);
	for(int j = 1; j< _n;j++){
		temp += sprintf(condition+temp," OR %s != %s",_fields[j],_values[j]);
	}
	sprintf(condition+temp,")");
	return condition;
}
/*select paticular row,col*/
bool KMySQL::select_rows(char * _conditon){
	//SELECT * FROM pet WHERE species = 'dog' AND sex = 'f';
	char * q = buffer;
	if (_conditon != NULL)
		sprintf(q,"SELECT * FROM %s WHERE %s",table,_conditon);
	else
		sprintf(q,"SELECT * FROM %s",table);
	return query(q);
}
bool KMySQL::select_cols(char ** _fields,int _n, char * _conditon){
	//SELECT name, birth FROM pet;
	char * q = buffer;
	int temp = sprintf(q,"SELECT %s",_fields[0]);
	for(int j = 1; j< _n; j++)
		temp += sprintf(q+temp,", %s",_fields[j]);
	if(_conditon != NULL)
		sprintf(q+temp," FROM %s WHERE %s",table,_conditon);
	else
		sprintf(q+temp," FROM %s",table);
	return query(q);
}
/*empty the table*/
bool KMySQL::emplty_table(){
	//DELETE FROM pet;
	char * q = buffer;
	sprintf(q,"DELETE FROM %s",table);
	return(query(q));
}
/*update  rows (record)*/
my_ulonglong KMySQL::update(char**_files,char**_new_values,int _n,char* _conditon){
	if (_conditon == NULL) return false;
	//update pet set sex = 'f',death = '2008-12-30' where name = 'Whistler';
	char *q = buffer;
	int temp = sprintf(q,"UPDATE %s SET %s = %s",table,_files[0],_new_values[0]);
	for (int j = 1; j<_n;j++)
		temp += sprintf(q+temp,",%s = %s",_files[j],_new_values[j]);
	sprintf(q+temp," WHERE %s",_conditon);
	if(query(q))
		return mysql_affected_rows(mysql);
	else 
		return 0;
}
/*load data from file to table,
in windows lines teminated by \r\n*/
bool KMySQL::load_data(char*_file,char*_line_teminate){
	//LOAD DATA LOCAL INFILE '/path/pet.txt' INTO TABLE pet
	//LINES TERMINATED BY '\r\n';
	char *q = buffer;
	sprintf(q,"LOAD DATA LOCAL INFILE '%s' INTO TABLE %s LINES TERMINATED BY '%s'",
		_file,table,_line_teminate);
	return(query(q));
}
/*delete rows*/
my_ulonglong KMySQL::delete_rows(char* _condition){
	//DELETE FROM pet WHERE name = 'Slim' AND sex = 'm';
	char *q = buffer;
	sprintf(q,"DELETE FROM %s WHERE %s",table,_condition);
	//printf("%s\n",q);
	if(query(q))
		return mysql_affected_rows(mysql);
	else
		return 0;
}

/*create table*/
bool KMySQL::create_table(char**_fields,char**_data_types,int _n){
	//CREATE TABLE event (name VARCHAR(20), date DATE,
	//type VARCHAR(15), remark VARCHAR(255));
	char *q = buffer;
	int temp = sprintf(q,"CREATE TABLE %s (%s %s",table,_fields[0],_data_types[0]);
	for(int j = 1; j<_n; j++){
		temp += sprintf(q+temp,", %s %s",_fields[j],_data_types[j]);
	}
	sprintf(q+temp,"%s",")");
	return query(q);
}

/*delete table*/
bool KMySQL::delete_table(){
	//DROP [TEMPORARY] TABLE [IF EXISTS]
	//	tbl_name [, tbl_name] ...
	//	[RESTRICT | CASCADE]
	char *q = buffer;
	sprintf(q,"DROP TABLE %s",table);
	return query(q);
}

const char *KMySQL::get_last_error(){
	return mysql_error(mysql);
	//return buffer;
}
char *KMySQL::get_last_query(){
	return buffer;
}
void KMySQL::close(){
	mysql_close(mysql);
}
void KMySQL::free_res(){
	mysql_free_result(result);
}
/*create database*/
bool KMySQL::create_database(){
	//CREATE DATABASE menagerie;
	char *q = buffer;
	sprintf_s(q,CKMYSQL_BUFF_LENGTH,"CREATE DATABASE %s",db);
	return query(q);
}
/*delete database*/
bool KMySQL::delete_database(){
	//DROP {DATABASE | SCHEMA} [IF EXISTS] db_name
	char *q = buffer;
	sprintf_s(q,CKMYSQL_BUFF_LENGTH,"DROP DATABASE %s",db);
	return query(q);
}

MYSQL*	KMySQL::connect(){
	return mysql_real_connect(mysql, host, user, passwd, NULL, NULL, NULL, NULL);
}

/*select database*/
bool	KMySQL::select_database(){
	char *q = buffer;
	sprintf_s(q,CKMYSQL_BUFF_LENGTH,"USE %s",db);
	return query(q);
}
bool	KMySQL::select_table(char* table_name)
{
	Kstrcpy_r(table,table_name);
	return true;
}

MYSQL_RES * KMySQL::get_res(void)
{
	return result;
}

// CStringArray KMySQL::getFieldsName(void)
// {
// 	CStringArray fieldsName;
// 	unsigned int num_fields;
// 	unsigned int i;
// 	MYSQL_FIELD *fields;
// 
// 	num_fields = mysql_num_fields(result);
// 	fields = mysql_fetch_fields(result);
// 	for(i = 0; i < num_fields; i++)
// 	{
// 		fieldsName.Add(CString(fields[i].name));
// 	}
// 
// 	return fieldsName;
// }