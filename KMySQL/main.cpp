#include <conio.h>
#include <stdio.h>
#include <winsock.h>
#include <mysql.h>
//#include "mysql.h"
#include "Kmysql.h"
#include "Kmysql-imp.h"

#pragma comment (lib, "libmysql.lib")

// change these to suit your setup
#define TABLE_OF_INTEREST "pet"
#define SERVER_NAME "localhost"
#define DB_USER "root"
#define DB_USERPASS "rabbit"
#define DB_NAME "menagerie"
#define SERVER_PORT 3306

int main(){
	//contruct class
	KMySQL my_class = KMySQL(TABLE_OF_INTEREST,
							SERVER_NAME,
							DB_USER,
							DB_USERPASS,
							SERVER_PORT,
							DB_NAME);
	//conect to server
	if (0 == my_class.real_connect()) {
		fprintf(stdout,"%s\n",my_class.get_last_error());
		return 0;
	}
	else
		fprintf(stdout,"%s\n",my_class.get_server_info());
	
	//CREATE TABLE pet (name VARCHAR(20), owner VARCHAR(20),
    //-> species VARCHAR(20), sex CHAR(1), birth DATE, death DATE);	
	int n = 6;
	char *fields[] = {"name","owner","species","sex","birth","death"};
	char *data_types[] = {"VARCHAR(20)","VARCHAR(20)","VARCHAR(20)","CHAR(1)","DATE","DATE"};
	my_class.create_table(fields,data_types,n);
	
	//empty table
	my_class.emplty_table();

	//load data from file
	if (my_class.load_data("d:/pet.txt","\\r\\n"))
		printf("load data success!\n");
	else{
		printf("%s\n%s\n",my_class.get_last_query(),my_class.get_last_error());
		return 0;
	}
	//update pet set sex = NULL,death = '2008-11-30' 
	//->where name = 'Whistler';
	n = 2;
	fields[0] = "sex";
	fields[1] = "death";
	char *new_values[] = {NULL,"'2008-11-30'"};
	char *conditon = "name \= 'Whistler'";
	if(my_class.update(fields,new_values,n,conditon))
		printf("%s\n",my_class.get_last_query());
	else{
		printf("%s\n",my_class.get_last_query());
		printf("%s\n",my_class.get_last_error());
	}
	
	//query a command
	if(!my_class.query("SELECT * FROM pet WHERE sex = 'm'")){
		printf("%s\n",my_class.get_last_query());
		printf("%s\n",my_class.get_last_error());
	}
	else{
		printf("%s\n",my_class.get_last_query());
		my_class.store_result();
		my_class.show_result();
	}

	//SELECT * FROM pet WHERE species = 'snake' OR species = 'bird' OR death = NULL;
	n = 3;
	fields[0] = "sex";
	fields[1] = "species";
	fields[2] = "death";
	char *values[] = {"'f'","'bird'",NULL};
	
	if(my_class.select_rows(my_class.mk_or_eq(fields,values,n))){
		printf("%s\n",my_class.get_last_query());
		my_class.store_result();
		my_class.show_result();
	}
	else{
		printf("%s\n",my_class.get_last_query());
		printf("%s\n",my_class.get_last_error());
	}
	
	//SELECT sex,species,death FROM pet
	if(my_class.select_cols(fields,n,NULL)){
		printf("%s\n",my_class.get_last_query());
		my_class.store_result();
		my_class.show_result();
	}
	else{
		printf("%s\n",my_class.get_last_query());
		printf("%s\n",my_class.get_last_error());
	}
	
	//SELECT * FROM pet WHERE (name != 'Slim')
	fields[0] = "name";
	values[0] = "'Slim'";
	if(my_class.select_rows(my_class.mk_and_ne(fields,values,1))){
		printf("%s\n",my_class.get_last_query());
		my_class.store_result();
		my_class.show_result();
	}
	else{
		printf("%s\n",my_class.get_last_query());
		printf("%s\n",my_class.get_last_error());
	}
	//printf("%s\n",my_class.mk_and_ne(fields,values,1));

	//query and store result
	if(!my_class.query_and_store("SELECT * FROM pet Where sex = 'f'"))
		printf("query error!\n");
	else{
		printf("%s\n",my_class.get_last_query());
		my_class.show_result();

		my_class.seek_back();
		//get item at row 2 col 2 in result
		printf("item at row 2 col 2: %s\n",my_class.get_item_q(1,2));

		//seek 1 row in result
		my_class.row_seek(1);
		printf("after seek 1 row, item at row 1 col 2: %s\n",my_class.get_item_q(1,2));
		my_class.show_result();
		//cause i ve just seek 1 row

		printf("now i seek back\n");
		my_class.seek_back();
		my_class.show_result();
	}

	//insert a row to table
	char *row[] = {NULL,"'khiem'","'hamster'",NULL,"'1999-03-30'",NULL};
	int n_fields = 6;
	if(my_class.insert_row(row,n_fields)){
		printf("%s\n",my_class.get_last_query());
		my_class.show_table();
	}
	else{
		printf("%s\n",my_class.get_last_query());
		printf("%s\n",my_class.get_last_error());
	}
	
	//delete a row
	//DELETE FROM pet WHERE owner = 'khiem' AND birth = '1999-03-30';
	n = 2;
	fields[0] = "owner";
	fields[1] = "birth";
	values[0] = "'khiem'";
	values[1] = "'1999-03-30'";
	if(my_class.delete_rows(my_class.mk_and_eq(fields,values,n)))
		printf("%s\n",my_class.get_last_query());
	else{
		printf("%s\n",my_class.get_last_query());
		printf("%s\n",my_class.get_last_error());
	}

	my_class.show_table();
	my_class.free_res();
	my_class.close();
	
	return  0;
}