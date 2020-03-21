#define macros
EXECUTABLE_NAME = tcshell.exe
DIR_SRC = .\
DIR_INCLUDE = .\
DIR_BIN = .\bin
#DIR_BIN_X86 = $(DIR_BIN)\x86
DIR_BIN_X86 = $(DIR_BIN)
DIR_INTERMEDIATE = .\bin
#DIR_INTERMEDIATE_X86 = $(DIR_INTERMEDIATE)\x86
DIR_INTERMEDIATE_X86 = $(DIR_INTERMEDIATE)

CFLAGS = /c /EHsc
LFLAGS =

!IF "$(enable_debug)" == "yes"
CFLAGS = $(CFLAGS) /D "DEBUG" /nologo /MDd /GR /Od /W4 /Zi
LFLAGS = $(LFLAGS) /INCREMENTAL:NO /DEBUG
# /OPT:REF /OPT:ICF
!ELSE
CFLAGS = $(CFLAGS) /nologo /MD /GR /O2 /W3
!ENDIF

SRC_FILES= \
  $(DIR_SRC)\sha1.c \
  $(DIR_SRC)\sha224-256.c \
  $(DIR_SRC)\sha384-512.c

#use rmem.dll rmem.lib
!IF "$(use_rmem)" == "yes"
CFLAGS = $(CFLAGS) /D "RINGMEM_DLL"
LFLAGS = $(LFLAGS) /LIBPATH:$(DIR_BIN_X86)
!ENDIF
RMEM_EXPORT = /D "RINGMEM_LIBRARY_EXPORT"

default: build

# create directories and build application
all: clean create_dirs complie $(EXECUTABLE_NAME)

$(EXECUTABLE_NAME) : build_rmem
   @echo Linking $(EXECUTABLE_NAME)...
   link /out:$(DIR_BIN_X86)\$(EXECUTABLE_NAME) $(LFLAGS) $(DIR_INTERMEDIATE_X86)\*.obj

# build application
build: create_dirs complie $(EXECUTABLE_NAME)

rebuild: clean complie $(EXECUTABLE_NAME)

#build rmem module
# DLL_NAME = rmem.dll
# DLL_INCLUDE = /I inc /I .\inlcude \
          # /I "C:\Program Files\Microsoft Visual Studio 9.0\VC\include" \
          # /I "C:\Program Files\Microsoft SDKs\Windows\v6.0A\include"
# DLL_LIBS    =  kernel32.lib \
  # user32.lib gdi32.lib winspool.lib \
  # comdlg32.lib advapi32.lib shell32.lib \
  # ole32.lib oleaut32.lib uuid.lib \
  # odbc32.lib odbccp32.lib
#DLL_EXPORT = /D "RINGMEM_LIBRARY_EXPORT"

build_dll:
# cl $(CFLAGS) /c /EHsc /Fo$(DIR_INTERMEDIATE_X86)\ $(DLL_INCLUDE) $(DLL_EXPORT) $(DIR_SRC)\rmem.cpp
# link $(LFLAGS) $(DLL_LIBS) /DLL /out:$(DIR_BIN_X86)\$(DLL_NAME) $(DIR_INTERMEDIATE_X86)\*.obj 
#  cl $(CFLAGS) /D "RINGMEM_DLL" /D "RINGMEM_LIBRARY_EXPORT" /Fo$(DIR_INTERMEDIATE_X86)\ /I$(DIR_INCLUDE) $(DIR_SRC)\rmem.cpp
  link /DLL $(LFLAGS) /out:$(DIR_BIN_X86)\sha.dll $(DIR_INTERMEDIATE_X86)\*.obj

# description block
complie: $(SRC_FILES)
  @echo Compiling...
  cl $(CFLAGS) /Fo$(DIR_INTERMEDIATE_X86)\ /I$(DIR_INCLUDE) $(SRC_FILES)
#  cl /EHsc /Fe$(DIR_BIN_X86)\$(EXECUTABLE_NAME) /I$(DIR_INCLUDE) $(SRC_FILES)
#  copy *.obj $(DIR_INTERMEDIATE_X86)
#  del *.obj

#{$(DIR_SRC)}.cpp{$(DIR_INTERMEDIATE_X86)}.obj ::
#    @echo Compiling...
#    cl $(CFLAGS) /Fo$(DIR_INTERMEDIATE_X86)\ /I$(DIR_INCLUDE) $<

# create output directories
create_dirs:
  @if not exist $(DIR_BIN_X86) mkdir $(DIR_BIN_X86)
  @if not exist $(DIR_INTERMEDIATE_X86) mkdir $(DIR_INTERMEDIATE_X86)

# delete output directories
clean:
  @if exist $(DIR_BIN) del /S /Q $(DIR_BIN)\*.*
  @if exist $(DIR_INTERMEDIATE) del /S /Q $(DIR_INTERMEDIATE)\*.*
