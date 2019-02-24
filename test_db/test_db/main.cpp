#include <Windows.h>
#include <stdio.h>

#include "sqlite3.h"
#include "sqlite3ext.h"

#define MAX_STRQRY_SIZE 2048
#define MAX_STRFIELD_SIZE 32

#define assert(...)

#define solution 3		//1:khiem, 2:hien, 3:tung

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
#if solution == 1
	ret = sqlite3_open("test2.db", &m_db)
#elif solution == 2
	ret = sqlite3_open("test_hien.db", &m_db);
#elif solution == 3
	ret = sqlite3_open("test_tung.db", &m_db);
#endif
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
			ID INT PRIMARY KEY,\
			name char(31),\
			area_id INTEGER \
			)";
	ret = sqlite3_exec(m_db, crtTblUser, 0, 0, 0);
	assert(ret == 0);
#if solution == 1
	char* crtTblArea = "CREATE TABLE if not exists area(\
			ID INT PRIMARY KEY,\
			name char(31),\
			level INTEGER \
			)";
	ret = sqlite3_exec(m_db, crtTblArea, 0, 0, 0);
	assert(ret == 0);
	char* crtTblMap = "CREATE TABLE if not exists map_lv1_lv2(\
			lv1 INTEGER,\
			lv2 INTEGER \
			)";
	ret = sqlite3_exec(m_db, crtTblMap, 0, 0, 0);
	assert(ret == 0);
#elif solution == 2
	char* crtTblArea = "CREATE TABLE if not exists area(\
			ID INT PRIMARY KEY,\
			kv1 INTEGER, \
			name char(31),\
			level INTEGER \
			)";
	ret = sqlite3_exec(m_db, crtTblArea, 0, 0, 0);
	assert(ret == 0);
#elif solution == 3
	char* crtTblArea = "CREATE TABLE if not exists place1(\
			ID INT PRIMARY KEY,\
			name char(31) \
			)";
	ret = sqlite3_exec(m_db, crtTblArea, 0, 0, 0);
	assert(ret == 0);
	char* crtTblMap = "CREATE TABLE if not exists place2(\
			ID INT PRIMARY KEY,\
			lv1 INTEGER,\
			name char(31) \
			)";
	ret = sqlite3_exec(m_db, crtTblMap, 0, 0, 0);
	assert(ret == 0);
#endif

	return ret;
}
int insert(char* tblName, int nFields, char* fields[], char* values[])
{
	int ret;
	char qryStr[MAX_STRQRY_SIZE];
	char *buff = qryStr;
	int size = MAX_STRQRY_SIZE;

	//make qry
	//INSERT INTO area(id, name, level)
	int len = 0;
	len = sprintf_s(buff, size, "INSERT INTO %s(", tblName);
	for (int i = 0; i < nFields; i++) {
		len += sprintf_s(buff + len, size - len, " %s,", fields[i]);
	}
	buff[len - 1] = ')';

	//VALUES(1, 'area1 1', '1' );
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
#if solution == 3
	char* tblName = "place1";
#elif
	char* tblName = "area";
#endif
	char* fields[] = {"ID", "name", "level" };
	char idBuff[8];
	char areaBuff[16];
	char lvlBuff[4] = "1";
	//gen area lvl1
	char* values[] = { idBuff, areaBuff, lvlBuff };
	for (int i = 0; i < 54; i++)
	{
		sprintf_s(idBuff, 8, "%d", i + 1);
		sprintf_s(areaBuff, 16, "area1 %d", i + 1);
#if solution == 3
		ret = insert(tblName, 2, fields, values);
#else
		ret = insert(tblName, 3, fields, values);
#endif
	}
	//gen lvl2
	lvlBuff[0] = '2';
	char lv1Buff[8];
	char lv2Buff[8];
	char *lvls[2] = { lv1Buff, lv2Buff };
	char *mapFiles[] = { "lv1", "lv2" };

#if solution == 1
	char *sql1 = "insert into area(ID, name, level) Values(?, ?, ?)";
#elif solution == 2
	char *sql1 = "insert into area(ID, name, level, kv1) Values(?, ?, ?, ?)";
#elif solution == 3
	char *sql1 = "insert into place2(ID, name, lv1) Values(?, ?, ?)";
#endif
	sqlite3_stmt *addStmt1;
	ret = prepare_v2(sql1, &addStmt1);
	assert(ret == SQLITE_OK);
#if solution == 1
	char *sql2 = "insert into map_lv1_lv2(lv1, lv2) Values(?, ?)";
	sqlite3_stmt *addStmt2;
	ret = prepare_v2(sql2, &addStmt2);
	assert(ret == SQLITE_OK);
#endif
	for (int i = 0; i < 1000; i++)
	{
		int idLv1 = rand() % 54 + 1;
		int idLv2 = 54 + i + 1;
		sprintf_s(areaBuff, 16, "area2 %d_%d", idLv1, idLv2);
		//ret = insert(tblName, 2, fields, values);
		ret = sqlite3_bind_int(addStmt1, 1, idLv2);
		ret = sqlite3_bind_text(addStmt1, 2, areaBuff, -1, SQLITE_TRANSIENT);
#if solution == 3
		ret = sqlite3_bind_int(addStmt1, 3, idLv1);
#elif
		ret = sqlite3_bind_int(addStmt1, 3, 2);
#endif
#if solution == 2
		ret = sqlite3_bind_int(addStmt1, 4, idLv1);
#endif
		ret = step(addStmt1);
		assert(ret == SQLITE_DONE);
		ret = reset(addStmt1);

#if solution == 1
		sprintf_s(lv1Buff, 8, "%d", idLv1);
		sprintf_s(lv2Buff, 8, "%d", idLv2);
		//ret = insert("map_lv1_lv2", 2, mapFiles, lvls);
		ret = sqlite3_bind_int(addStmt2, 1, idLv1);
		ret = sqlite3_bind_int(addStmt2, 2, idLv2);
		ret = step(addStmt2);
		assert(ret == SQLITE_DONE);
		ret = reset(addStmt2);
#endif
	}

	sqlite3_finalize(addStmt1);
#if solution == 1
	sqlite3_finalize(addStmt2);
#endif
	return ret;
}
int genUserData()
{
	int ret;
	char *sql = "insert into user(ID, name, area_id) Values(?, ?, ?)";
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
		int userId = i + 1;
		int areaId = 54 + (rand() % 1000 + 1);
		int iF = rand() % (sizeof(firsts) / sizeof(char*));
		int iM = rand() % (sizeof(mids) / sizeof(char*));
		int iL = rand() % (sizeof(lasts) / sizeof(char*));
		sprintf_s(nameBuff, 32, "%s %s %s", firsts[iF], mids[iM], lasts[iL]);
		sprintf_s(areaBuff, 8, "%d", areaId);
		//ret = insert(tblName, 2, fields, values);
		ret = sqlite3_bind_int(addStmt, 1, userId);
		ret = sqlite3_bind_text(addStmt, 2, nameBuff, -1, SQLITE_TRANSIENT);
		ret = sqlite3_bind_int(addStmt, 3, areaId);
		ret = step(addStmt);
		assert(ret == SQLITE_DONE);
		ret = reset(addStmt);
	}

	sqlite3_finalize(addStmt);

	return ret;
}
int main()
{
	srand(20190224);
	
	//open db
	loadDbModule();
	openDb();

	//begin trans
	exec("BEGIN");

	//crt tbl
	crtTbls();

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