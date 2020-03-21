::run by development command
cl sha1.c
cl sha224-256.c
cl sha384-512.c
link /DLL /out:sha.dll *.obj /def:sha.def