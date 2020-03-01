
// Kamac.h: PROJECT_NAME 应用程序的主头文件
//

#pragma once

#include "CKamacSheet.h"


#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含 'pch.h' 以生成 PCH"
#endif

#include "resource.h"		// 主符号


// CKamacApp:
// 有关此类的实现，请参阅 Kamac.cpp
//

class CKamacApp : public CWinApp
{
public:
	CKamacApp();
	~CKamacApp();
// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()

protected:
	ATOM atomInstance{ 0 };
	CKamacSheet* ppsSheet{ nullptr };

	BOOL CheckExist(void);
	virtual BOOL OnIdle(LONG lCount);
};

extern CKamacApp theApp;
