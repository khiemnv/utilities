#include <stdlib.h>
#include <string.h>
#include <stdio.h>
/*
first u need free the des_str and set des_str = 0
u dont have malloc a memory for des_str cause i will do it.
*/
char* Kstrcpy(char* &des_str, char *src_str){
	//if (des_str != 0) {
	free(des_str);
	//	printf("des_str has just free: des_str = %d\n",(int)des_str);
	//}
	char * temp = (char*) calloc (sizeof(char),3);
	des_str = (char*)calloc(1,sizeof(char) * (strlen(src_str) + 1));
	free(temp);
	strcpy (des_str,src_str);
	return des_str;
}
int main()
{
	char * mess = "hello world!";
	char * mess_cpy = 0;
	char * des = 0;
	char * olddes = 0;
	mess_cpy = (char*)calloc(sizeof(char),1 + strlen(mess));
	strcpy(mess_cpy,mess);
	printf("%d %s\n",(int)mess_cpy,mess_cpy);
	printf("%d\n",(int)Kstrcpy(des,mess));
	printf("%d %s\n",(int)des,des);
	olddes = des;
	printf("%d\n",(int)Kstrcpy(des,"hello 2"));
	printf("%d %s\n",(int)des,des);
	printf("%d %s\n",(int)olddes,olddes);
	
	//free (temp);
	//free (mess_cpy);
	return 0;
}