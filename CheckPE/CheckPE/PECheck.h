//////////////////////////////////////////////////////////////////////////
// Writer: KhaiTN
// PEinject Heuristic
//////////////////////////////////////////////////////////////////////////

#ifndef ___PECHECK_H___
#define ___PECHECK_H___
class PECheckFile
{
private
	char filename[MAX_PATH];
	IMAGE_NT_HEADERS NtHeader;
	IMAGE_DOS_HEADER DosHeader;
	IMAGE_OPTIONAL_HEADER OptionnalHeader;
	IMAGE_SECTION_HEADER SectionHeader;
	
public:
	int isPEinject();
};

#endif