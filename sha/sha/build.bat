::run by development command
cl -c sha224-256.c  /D "_UNICODE" /D "UNICODE" 
cl -c sha384-512.c  /D "_UNICODE" /D "UNICODE" 
cl -c md5.c  /D "_UNICODE" /D "UNICODE" 
cl -c main.c  /D "_UNICODE" /D "UNICODE" 
link /DLL /out:hash.dll *.obj /def:hash.def