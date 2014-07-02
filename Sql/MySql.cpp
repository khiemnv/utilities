#include <conio.h>
#include <stdio.h>
#include <winsock.h>
#include <mysql.h>
//#include "mysql.h"
#include "KMySQLClass.h"

#pragma comment (lib, "libmysql.lib")

// change these to suit your setup
#define TABLE_OF_INTEREST "pet"
#define SERVER_NAME "localhost"
#define DB_USER "root"
#define DB_USERPASS "rabbit"
#define DB_NAME "menagerie"
#define SERVER_PORT 3306

// prototypes
void showTables(MYSQL*);
void showContents(MYSQL*,const char*);

int main(){
	KMySQL my_class = KMySQL(TABLE_OF_INTEREST,
							SERVER_NAME,
							DB_USER,
							DB_USERPASS,
							SERVER_PORT,
							DB_NAME);
	if (0 == my_class.real_connect()) 
		printf(my_class.get_last_error());
	else
		printf(my_class.get_server_info());
	
	my_class.list_table(NULL);
	my_class.show_result();
	my_class.select_database(TABLE_OF_INTEREST);
	if (!my_class.query("SELECT * FROM pet"))
		printf("query error!\n");
	else {
		printf("query success!\n");
		my_class.store_result();
		my_class.show_result();
	}
	
	if(!my_class.query("SELECT * FROM pet WHERE sex = 'm'"))
		printf("query error!\n");
	else{
		printf("query success!\n");
		my_class.store_result();
		my_class.show_result();
	}

	if(!my_class.query_and_store("SELECT * FROM pet Where sex = 'f'"))
		printf("query error!\n");
	else{
		printf("query success!\n");
		my_class.row_seek(1);
		printf("item at row 2 col 2: %s\n",my_class.get_item_q(1,2));
		//cause i ve just seek 1 row
		my_class.seek_back();
		my_class.show_result();
	}

	my_class.close();

	return 0;
}

int main_old(int argc, char* argv[])
{
	MYSQL *hnd=NULL; // mysql connection handle
	const char *sinf=NULL; // mysql server information
	
	hnd = mysql_init(NULL);
	if (NULL == mysql_real_connect(hnd,SERVER_NAME,DB_USER,DB_USERPASS,DB_NAME,3306,NULL,0))
	{
		fprintf(stderr,"Problem encountered connecting to the %s database on %s.\n",DB_NAME,SERVER_NAME);
	}
	else
	{
		fprintf(stdout,"Connected to the %s database on %s as user '%s'.\n",DB_NAME,SERVER_NAME,DB_USER);
		sinf = mysql_get_server_info(hnd);
		
		if (sinf != NULL)
		{
			fprintf(stdout,"Got server information: '%s'\n",sinf);
			showTables(hnd);
			MYSQL_RES* result;
		//list per table
			// =list_table(hnd,"per");
			//showResult(result);
			//mysql_free_result(result);
			
			//selectDatabase(hnd,"menagerie");	
		//insert a row
			//query(hnd,"INSERT INTO pet VALUES ('Puffball2','Diane2','hamster2','f','1999-03-30',NULL)");
		//delete a row which has 'name' field = 'Puffball'
			//query(hnd,"DELETE FROM pet WHERE name = 'Puffball'");
		//create a table
			//query(hnd,"CREATE TABLE pet3 (name VARCHAR(20), owner VARCHAR(20), species VARCHAR(20), sex CHAR(1), birth DATE, death DATE)");
		//insert table from a file
			//query(hnd,"LOAD DATA LOCAL INFILE 'C:/pet.txt' INTO TABLE pet3");

			showContents(hnd,"pet");
		}
		else
		{
			fprintf(stderr,"Failed to retrieve the server information string.\n");
		}
		
		mysql_close(hnd);
	}
	
	getch();
	return 0;
} 

void showTables(MYSQL *handle)
{
	MYSQL_RES *result=NULL; // result of asking the database for a listing of its tables
	MYSQL_ROW row; // one row from the result set
	
	result = mysql_list_tables(handle,NULL);
	row = mysql_fetch_row(result);
	fprintf(stdout,"Tables found:\n\n");
	while (row)
	{
		fprintf(stdout,"\t%s\n",row[0]);
		row = mysql_fetch_row(result);
	}
	mysql_free_result(result);
	
	fprintf(stdout,"\nEnd of tables\n");
	
	return;
}

void showContents
(
 MYSQL *handle,
 const char *tbl
 )
{
	MYSQL_RES *res=NULL; // result of querying for all rows in table
	MYSQL_ROW row; // one row returned
	char sql[1024], // sql statement used to get all rows
		commastr[2]; // to put commas in the output
	int i,numf=0; // number of fields returned from the query
	
	sprintf(sql,"select * from %s",tbl);
	fprintf(stdout,"Using sql statement: '%s' to extract all rows from the specified table.\n",sql);
	
	if (!mysql_query(handle,sql))
	{
		res = mysql_use_result(handle);
		if (res)
		{
			numf = mysql_num_fields(res);
			row = mysql_fetch_row(res);
			fprintf(stdout,"Rows returned:\n\n");
			while (row)
			{
				commastr[0]=commastr[1]=(char)NULL;
				for (i=0;i<numf;i++)
				{
					if (row[i] == NULL)
					{
						fprintf(stdout,"%sNULL",commastr);
					}
					else
					{
						fprintf(stdout,"%s%s",commastr,row[i]);
					}
					commastr[0]=',';
				}
				fprintf(stdout,"\n");
				
				row = mysql_fetch_row(res);
			}
			fprintf(stdout,"\nEnd of rows\n");
			
			mysql_free_result(res);
		}
		else
		{
			fprintf(stderr,"Failed to use the result acquired!\n");
		}
	}
	else
	{
		fprintf(stderr,"Failed to execute query. Ensure table is valid!\n");
	}
	
	return;
}
