#include "pch.h"
#include "CKamacCmdLineInfo.h"

void CKamacCmdLineInfo::ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast)
{
	CCommandLineInfo::ParseParam(pszParam, bFlag, bLast);
	if (bFlag)
	{
		if (pszParam[0] == _T('s') || pszParam[0] == _T('S'))
			bSilent = TRUE;
	}
}
