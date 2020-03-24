#include <tchar.h>
typedef enum HashMode {
	Md5,
	Sha512
}HashMode;
int Hash(TCHAR* path, TCHAR* buff, int buffCount, HashMode hashMode);