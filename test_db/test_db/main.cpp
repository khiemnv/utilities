#include <Windows.h>
#include <stdio.h>

#include "sqlite3.h"
#include "sqlite3ext.h"

#define MAX_STRQRY_SIZE 2048
#define MAX_STRFIELD_SIZE 32

#define assert(...)

static sqlite3_api_routines s_api;
HMODULE s_sqlite3Mod;
static const sqlite3_api_routines *sqlite3_api = &s_api;

#define LOAD_API(zApi)\
	s_api.zApi = (zApi)GetProcAddress(s_sqlite3Mod, "sqlite3_"#zApi);\
	assert(s_api.zApi)

int loadDbModule() {
	TCHAR *zSqlite3Dll = TEXT("sqlite3.dll");
	//load dll
	s_sqlite3Mod = LoadLibrary(zSqlite3Dll);
	assert(s_sqlite3Mod);
	//load api routines
	typedef int(*open)(const char*, sqlite3**);
	typedef int(*close)(sqlite3*);
	typedef int(*exec)(sqlite3*, const char*, sqlite3_callback, void*, char**);
	typedef int(*prepare_v2)(sqlite3*, const char*, int, sqlite3_stmt**, const char**);
	typedef int(*reset)(sqlite3_stmt*pStmt);
	typedef int(*step)(sqlite3_stmt*);
	typedef int(*bind_text)(sqlite3_stmt*, int, const char*, int n, void(*)(void*));
	typedef int(*bind_int)(sqlite3_stmt*, int, int);
	typedef int(*finalize)(sqlite3_stmt*pStmt);
	s_api.open = (open)GetProcAddress(s_sqlite3Mod, "sqlite3_open");
	assert(s_api.open);
	LOAD_API(close);
	LOAD_API(exec);
	LOAD_API(prepare_v2);
	LOAD_API(reset);
	LOAD_API(step);
	LOAD_API(bind_text);
	LOAD_API(bind_int);
	LOAD_API(finalize);

	return 0;
}
int unloadDbModule() {
	assert(s_sqlite3Mod);
	int ret = FreeLibrary(s_sqlite3Mod);
	return ret;
}

// db connect & qry
sqlite3 *m_db;
int closeDb() {
	int ret = sqlite3_close(m_db);
	assert(ret == 0);
	return ret;
}
int exec(char* qryStr) {
	return sqlite3_exec(m_db, qryStr, 0, 0, 0);
}
int openDb() {
	int ret;
	ret = sqlite3_open("test.db", &m_db);
	assert(ret == 0);
	return ret;
}
int prepare_v2(const char* sql, sqlite3_stmt** ppstmt) {
	return sqlite3_prepare_v2(m_db, sql, -1, ppstmt, 0);
}
int reset(sqlite3_stmt* pstmt) {
	return sqlite3_reset(pstmt);
}
int step(sqlite3_stmt* pstmt) {
	return sqlite3_step(pstmt);
}
int crtTbls()
{
	int ret;
	char* crtTblUser = "CREATE TABLE if not exists user(\
			ID ROWID,\
			name char(31),\
			area_id INTEGER \
			)";
	ret = sqlite3_exec(m_db, crtTblUser, 0, 0, 0);
	assert(ret == 0);
	char* crtTblArea = "CREATE TABLE if not exists area(\
			ID ROWID,\
			name char(31),\
			level INTEGER \
			)";
	ret = sqlite3_exec(m_db, crtTblArea, 0, 0, 0);
	assert(ret == 0);
	char* crtTblMap = "CREATE TABLE if not exists map_lv1_lv2(\
			ID ROWID,\
			lv1 INTEGER,\
			lv2 INTEGER \
			)";
	ret = sqlite3_exec(m_db, crtTblMap, 0, 0, 0);
	assert(ret == 0);

	return ret;
}
int insert(char* tblName, int nFields, char* fields[], char* values[])
{
	int ret;
	char qryStr[MAX_STRQRY_SIZE];
	char *buff = qryStr;
	int size = MAX_STRQRY_SIZE;

	//make qry
	//INSERT INTO area(name, level)
	int len = 0;
	len = sprintf_s(buff, size, "INSERT INTO %s(", tblName);
	for (int i = 0; i < nFields; i++) {
		len += sprintf_s(buff + len, size - len, " %s,", fields[i]);
	}
	buff[len - 1] = ')';

	//VALUES('area1 1', '1' );
	len += sprintf_s(buff + len, size - len, " VALUES (");
	for (int i = 0; i < nFields; i++) {
		len += sprintf_s(buff + len, size - len, " '%s',", values[i]);
	}
	buff[len - 1] = ')';
	assert(buff[len] == 0);

	ret = exec(buff);
	return ret;
}
int genAreaData()
{
	int ret;

	char* tblName = "area";
	char* fields[] = {"name", "level" };
	char areaBuff[16];
	char lvlBuff[4] = "1";
	//gen lvl1
	char* values[] = { areaBuff, lvlBuff };
	for (int i = 0; i < 54; i++)
	{
		sprintf_s(areaBuff, 16, "area1 %d", i + 1);
		ret = insert(tblName, 2, fields, values);
	}
	//gen lvl2
	lvlBuff[0] = '2';
	char lv1Buff[8];
	char lv2Buff[8];
	char *lvls[2] = { lv1Buff, lv2Buff };
	char *mapFiles[] = { "lv1", "lv2" };

	char *sql1 = "insert into area(name, level) Values(?, ?)";
	sqlite3_stmt *addStmt1;
	ret = prepare_v2(sql1, &addStmt1);
	assert(ret == SQLITE_OK);
	char *sql2 = "insert into map_lv1_lv2(lv1, lv2) Values(?, ?)";
	sqlite3_stmt *addStmt2;
	ret = prepare_v2(sql2, &addStmt2);
	assert(ret == SQLITE_OK);
	for (int i = 0; i < 1000; i++)
	{
		int lv1 = rand() % 54 + 1;
		int lv2 = i + 1;
		sprintf_s(areaBuff, 16, "area2 %d_%d", lv1, i + 1);
		//ret = insert(tblName, 2, fields, values);
		ret = sqlite3_bind_text(addStmt1, 1, areaBuff, -1, SQLITE_TRANSIENT);
		ret = sqlite3_bind_int(addStmt1, 2, lv2);
		ret = step(addStmt1);
		assert(ret == SQLITE_DONE);
		ret = reset(addStmt1);

		sprintf_s(lv1Buff, 8, "%d", lv1);
		sprintf_s(lv2Buff, 8, "%d", i + 1);
		//ret = insert("map_lv1_lv2", 2, mapFiles, lvls);
		ret = sqlite3_bind_int(addStmt2, 1, lv1);
		ret = sqlite3_bind_int(addStmt2, 2, lv2);
		ret = step(addStmt2);
		assert(ret == SQLITE_DONE);
		ret = reset(addStmt2);
	}

	sqlite3_finalize(addStmt1);
	sqlite3_finalize(addStmt2);
	
	return ret;
}
int genUserData()
{
	int ret;
	char *sql = "insert into user(name, area_id) Values(?, ?)";
	sqlite3_stmt *addStmt;
	ret = prepare_v2(sql, &addStmt);
	assert(ret == SQLITE_OK);

	char* tblName = "user";
	char* fields[] = { "name", "area_id" };
	char areaBuff[8];
	char nameBuff[32];
	//gen area id
	char* values[] = { nameBuff, areaBuff };
	char* firsts[] = { "Nguyen","Dam","Dao","Le","Vu" };
	char* mids[] = { "","Thi","Van","Hoang","Anh" };
	char* lasts[] = { "Hien","Hai","An","Quang","Tuan","Dieu" };
	for (int i = 0; i < 1000*1000; i++)
	{
		int areaId = rand() % 1000 + 1;
		int iF = rand() % (sizeof(firsts) / sizeof(char*));
		int iM = rand() % (sizeof(mids) / sizeof(char*));
		int iL = rand() % (sizeof(lasts) / sizeof(char*));
		sprintf_s(nameBuff, 32, "%s %s %s", firsts[iF], mids[iM], lasts[iL]);
		sprintf_s(areaBuff, 8, "%d", areaId);
		//ret = insert(tblName, 2, fields, values);
		ret = sqlite3_bind_text(addStmt, 1, nameBuff, -1, SQLITE_TRANSIENT);
		ret = sqlite3_bind_int(addStmt, 2, areaId);
		ret = step(addStmt);
		assert(ret == SQLITE_DONE);
		ret = reset(addStmt);
	}

	sqlite3_finalize(addStmt);

	return ret;
}
int main()
{
//	int ret;
	
	//open db
	loadDbModule();
	openDb();

	//begin trans
	exec("BEGIN");

	//crt tbl
	//crtTbls();

	//generate test data
	genAreaData();

	genUserData();

	//end trans
	exec("COMMIT");

	//close db
	closeDb();
	unloadDbModule();
	return 0;
}