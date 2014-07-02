#include <mysql.h>
#include <string.h>
#include <stdlib.h>
#define BUFF_LENGTH 1024
/************************************************************************/
/* class KMySQL                                                         */
/************************************************************************/
class KMySQL{
private:
	char	*table;
	char	*host;
	char	*user;
	char	*passwd;
	unsigned int	port;
	char	*db;
	char	*buffer;
// 	char	*q_buffer;
// 	char	*c_buffer;
	MYSQL	*mysql;
	MYSQL_RES	*result;
	MYSQL_ROW_OFFSET	first_rows;
		//store the first row of result to seek back
	
	char* Kstrcpy_c(char* &des_str, char *src_str);
	char* Kstrcpy_r(char* &des_str, char *src_str);
public:
	KMySQL(char *_table,char *_server_name,char *_db_user,char *_db_userpass,unsigned int _port, char *_db_name);
	KMySQL();

	my_ulonglong	affected_rows();
	void		close();
	MYSQL*		connect();
	bool		create_database();
	bool		create_table(char**_fields,char**_data_types,int _n);
	bool		delete_database();
	bool		delete_table();
	bool		emplty_table();
	void		free_res();
	char*		get_item(unsigned int _row,unsigned int _col);
	char*		get_item_q(unsigned int _row,unsigned int _col);
	const char*	get_last_error();
	char*		get_last_query();
	const char*	get_server_info();
	void		init();
	my_ulonglong	insert_id();
	my_ulonglong	insert_row(char **_row,int _number_fields);
	bool		load_data(char*_file,char*_line_teminate);
	MYSQL_RES*	list_table(const char *wild);

	char*		mk_and(char*_cond1,char*_cond2);
	char*		mk_and(char**_conds,int _n);
	char*		mk_and_eq(char**_fields,char**_values,int _n);
	char*		mk_and_ne(char**_fields,char**_values,int _n);

	char*		mk_or(char**_conds,int _n);
	char*		mk_or(char*_cond1,char*_cond2);
	char*		mk_or_eq(char**_fields,char**_values,int _n);
	char*		mk_or_ne(char**_fields,char**_values,int _n);

	my_ulonglong	num_rows();
		
	bool		query(const char *stmt_str);
	MYSQL_RES*	query_and_store(const char *stmt_str);
	MYSQL*		real_connect();
	my_ulonglong	delete_rows(char* _condition);
	void		reset(char *_table,char *_server_name,char *_db_user,char *_db_userpass,unsigned int _port, char *_db_name);
	MYSQL_ROW_OFFSET row_seek(MYSQL_ROW_OFFSET _offset);
	MYSQL_ROW	row_seek(unsigned int _n_rows);
	void		seek_back();
	bool		select_database();
	bool		select_database(const char *_db_name);
	bool		select_cols(char**_fields,int _n,char*_conditon);
	bool		select_rows(char * _conditon);
	MYSQL_RES*	store_result();
	void		show_result();
	bool		show_table();
	my_ulonglong	update(char**_files,char**_new_values,int _n,char* _conditon);
};