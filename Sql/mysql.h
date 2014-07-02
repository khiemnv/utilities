/* Creates the database named by the db  parameter.
This function is deprecated. 
It is preferable to use mysql_query() to issue an 
SQL CREATE DATABASE statement instead. 
*/
/*bool createDatabase (MYSQL *mysql, const char *DB){
	if(mysql_create_db(mysql, "my_database"))
	{
		fprintf(stderr, "Failed to create new database.  Error: %s\n",
			mysql_error(mysql));
		return false;
	}
	return true;
}*/

/*Causes the database specified by db
 to become the default (current) database on the 
 connection specified by mysql. In subsequent queries, 
 this database is the default for table references that do not 
 include an explicit database specifier.
*/
bool selectDatabase(MYSQL *mysql,const char *db){
	if (mysql_select_db(mysql,db)){
		fprintf(stderr, "Failed to create new database.  Error: %s\n",
		mysql_error(mysql));
		return false;
	}
	else {		
		return true;
	}
}

MYSQL_ROW_OFFSET rowSeek(MYSQL_RES *result, MYSQL_ROW_OFFSET offset){
	return mysql_row_seek(result, offset);
}

my_ulonglong insertID(MYSQL *mysql){
	return mysql_insert_id(mysql);
}

/*Executes the SQL statement pointed to by 
the null-terminated string stmt_str. 
Normally, the string must consist of a single SQL statement and 
you should not add a terminating semicolon (“;”) or \g to the statement. 
If multiple-statement execution has been enabled, 
the string can contain several statements separated by semicolons.
*/
bool query(MYSQL *mysql, const char *stmt_str){
	char sql[1024], // sql statement used to get all rows
		commastr[2]; // to put commas in the output	
	
	commastr[0] = commastr[1] = 0;
	sprintf(sql,"%s\0\0",stmt_str);
	if (mysql_query(mysql, sql) == 0){
		fprintf(stderr, "Failed to create new database.  Error: %s\n",
			mysql_error(mysql));
		return false;
	}
	else {
		return true;
	}
}

/*Returns a result set consisting of table names in the 
current database that match the simple 
regular expression specified by the wild parameter. 
wild  may contain the wildcard characters “%” or “_”, or 
may be a NULL pointer to match all tables. 
Calling mysql_list_tables() is similar to 
executing the query SHOW TABLES [LIKE wild].
You must free the result set with mysql_free_result(). 
*/
MYSQL_RES * listTable(MYSQL *mysql, const char *wild){
	MYSQL_RES *result = mysql_list_tables(mysql, wild);
	if (result) 
		return result;
	else {
		fprintf(stderr, "Failed to create new database.  Error: %s\n",
			mysql_error(mysql));
		return 0;
	}
}

MYSQL_RES *storeResult(MYSQL *mysql){
	MYSQL_RES *result = mysql_store_result(mysql);
	if(result){
		return result;
	}
	else {
		fprintf(stderr, "Failed to create new database.  Error: %s\n",
			mysql_error(mysql));
		return 0;
	}
}

int showResult(MYSQL_RES* result){	
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
					printf("[%.*s] ", (int) lengths[i], 
						row[i] ? row[i] : "NULL");
				}
				printf("\n");
	}
	return 0;
}