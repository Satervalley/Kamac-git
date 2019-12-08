#pragma once
#include <afxwin.h>


class CKamacCmdLineInfo : public CCommandLineInfo
{
public:
    virtual void ParseParam(
        const TCHAR* pszParam,
        BOOL bFlag,
        BOOL bLast);
    BOOL bSilent{ FALSE };
};

