#include <mysql.h>
#include <string.h>
#include <stdlib.h>
#define ERROR_MESS_LENGTH 1024
/************************************************************************/
/* class KMySQL                                                         */
/************************************************************************/
class KMySQL{
public:
	//contructor

	KMySQL(char *_table,char *_server_name,char *_db_user,char *_db_userpass,unsigned int port,char *_db_name);
	void init();
	void reset(char *_table,char *_server_name,char *_db_user,char *_db_userpass,unsigned int _port, char *_db_name);

	/*methods*/

	MYSQL* real_connect();
	const char* get_server_info();
	//select database to query
	bool select_database(const char *_db_name);
	//seek to a row(this row will be the result->current row
	MYSQL_ROW_OFFSET row_seek(MYSQL_ROW_OFFSET _offset);
	MYSQL_ROW row_seek(unsigned int row);
	//
	my_ulonglong insert_id();
	//query
	bool query(const char *stmt_str);
	MYSQL_RES *query_and_store(const char *stmt_str);
	MYSQL_RES *list_table(const char *wild);
	//store result to MYSQL_RES * result and the first rows
	MYSQL_RES *store_result();
	//show and free result
	int show_result();
	//get an item at (row>=0,col>=0) in result
	const char * get_item(unsigned int _row,unsigned int _col);
	const char * get_item_q(unsigned int _row,unsigned int _col);
	//seek to the first row of result
	void seek_back();
	//get last error
	char *get_last_error(){	return error_mess;}
	//get MYSQL * handle
	MYSQL *get_handle(){ return mysql;}
	//get MYSQL_RES * result
	MYSQL_RES * get_result(){return result;}
	void close(){mysql_close(mysql);}
private:
	char *table;
	char *host;
	char *user;
	char *passwd;
	unsigned int port;
	char *db;
	char *item;
	char *error_mess;
	MYSQL *mysql;
	MYSQL_RES *result;
	MYSQL_ROW_OFFSET first_rows;//store the first row to seek back
	
	char* Kstrcpy_c(char* &des_str, char *src_str);
	char* Kstrcpy_r(char* &des_str, char *src_str);
};
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
KMySQL::KMySQL(char *_table,char *_server_name,char *_db_user,char *_db_userpass,unsigned int _port, char *_db_name){	
	table = 0; Kstrcpy_c(table,_table);
	host = 0; Kstrcpy_c(host,_server_name);
	user = 0; Kstrcpy_c(user,_db_user);
	passwd = 0; Kstrcpy_c(passwd,_db_userpass);
	db = 0; Kstrcpy_c(db,_db_name);
	item = 0;
	port = _port;
	mysql = mysql_init(NULL);
	error_mess = 0; error_mess = (char*)calloc(sizeof(char),ERROR_MESS_LENGTH);
}

void KMySQL::init(){
	table = 0;
	host = 0;
	user = 0;
	passwd = 0;
	db = 0;
	port = 0;
	mysql = 0;
	item = 0;
	error_mess = 0;
}

/*cal it if you ALREADY call init() or class already CONSTRUCTED*/
void KMySQL::reset(char *_table,char *_server_name,char *_db_user,char *_db_userpass,unsigned int _port, char *_db_name){
	Kstrcpy_r(table,_table);
	Kstrcpy_r(host,_server_name);
	Kstrcpy_r(user,_db_user);
	Kstrcpy_r(passwd,_db_userpass);
	Kstrcpy_r(db,_db_name);
	if(item != 0) {free(item); item = 0;}
	port = _port;
	mysql_close(mysql);
	mysql = mysql_init(NULL);
	error_mess = (char*)realloc(error_mess,sizeof(char) * ERROR_MESS_LENGTH);
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
			sprintf(error_mess,"Problem encountered connecting to the %s database on %s.\n",db,host);
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
		sprintf(error_mess, "Error: %s\n",
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

	char *sql = error_mess; // sql statement used to get all rows		
	sprintf(sql,"%s\0\0",stmt_str);
	if (mysql_query(mysql, sql)){
		sprintf(error_mess, "Error: %s\n",
			mysql_error(mysql));
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

	char *sql = error_mess; // sql statement used to get all rows		
	sprintf(sql,"%s\0\0",stmt_str);
	if (mysql_query(mysql, sql)){
		sprintf(error_mess, "Error: %s\n",
			mysql_error(mysql));
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
		sprintf(error_mess, "Error: %s\n",
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
		sprintf(error_mess, "Error: %s\n",
			mysql_error(mysql));
		return 0;
	}
}
/*+++++++++++++++++++++++++++++++++++*/
/*the result will be free after show*/
int KMySQL::show_result(){	
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
			for(int j = lengths[i] ? (int)lengths[i] : 4;
			j < (80/num_fields); j++)
				printf(" ");
		}
		printf("\n");
	}
	mysql_free_result(result);
	return 0;
}

/*get item at row i >=0, col(filed) j >=0 of result
NOTE i HAVENT SEEK BACK to the first row yet*/
const char * KMySQL::get_item(unsigned int _row,unsigned int _col){	

	unsigned int numf = mysql_num_fields(result);
	if(_col > numf) return NULL;

	MYSQL_ROW row = mysql_fetch_row(result);
	for(unsigned int j = 0; j < _row; j++ )
		row = mysql_fetch_row(result);
	if(row == NULL) return NULL;
		
	unsigned long *lengths = mysql_fetch_lengths(result);	
	if (item != 0) free (item);
	item = (char*)calloc(sizeof(char),lengths[_col]);
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

//seek to a row
MYSQL_ROW KMySQL::row_seek(unsigned int _row){
	MYSQL_ROWS *rows = result->data_cursor;
	
	for(unsigned int j = 0; j < _row; j++ ){
		rows = rows->next;
		if(rows == NULL) return NULL;
	}

	mysql_row_seek(result,rows);
	return rows->data;
}

