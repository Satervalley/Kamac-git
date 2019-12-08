#pragma once

#include "framework.h"
#include "Registry.h"

static LPCTSTR STR_REG_KEY_AUTO_START =
_T("Software\\microsoft\\Windows\\CurrentVersion\\Run");

class CKamacOptions
{
public:
	BOOL bStartWithOS{ FALSE };
	BOOL bMonitorSizeConfirmed{ FALSE };
	ULONG32 ulMonitorSize{3556};	// of 1/10 mm

	CKamacOptions& operator =(const CKamacOptions& ko)
	{
		bStartWithOS = ko.bStartWithOS;
		bMonitorSizeConfirmed = ko.bMonitorSizeConfirmed;
		ulMonitorSize = ko.ulMonitorSize;
		return *this;
	}


	void UpdateStartWithOS(LPCTSTR strExeFileName) const
	{
		CRegistry reg;
		reg.OpenKey(CRegistry::currentUser, STR_REG_KEY_AUTO_START);
		if (bStartWithOS)
		{
			CString str;
			str.Format(_T("\"%s\" /s"), strExeFileName);
			reg.SetValue(_T("Kamac"), str);
		}
		else
		{
			reg.DeleteValue(_T("Kamac"));
		}
		reg.CloseKey();
	}
};

