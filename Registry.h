#pragma once


class CRegistry
{
public:
	enum cregRestoreEnum
	{
		regVolatile = REG_WHOLE_HIVE_VOLATILE,
		regRefresh = REG_REFRESH_HIVE,
		regNoLazy = REG_NO_LAZY_FLUSH
	};

	enum Keys
	{
		classesRoot = (int)HKEY_CLASSES_ROOT,
		currentUser = (int)HKEY_CURRENT_USER,
		localMachine = (int)HKEY_LOCAL_MACHINE,
		currentConfig = (int)HKEY_CURRENT_CONFIG,
		users = (int)HKEY_USERS,
		performanceData = (int)HKEY_PERFORMANCE_DATA,	//Windows NT/2000
		dynData = (int)HKEY_DYN_DATA						//Windows 95/98
	};

	CRegistry()
		{
			m_hKey = NULL;
		};

	~CRegistry()
		{
			CloseKey();
			m_hKey = NULL;
		};

	BOOL OpenKey(enum Keys hKey, LPCTSTR szKey);
	BOOL CreateKey(enum Keys hKey, LPCTSTR szKey);
	BOOL DeleteKey(enum Keys hKey, LPCTSTR szKey);
	BOOL DeleteValue(LPCTSTR lpValueName);
	BOOL GetValue(LPCTSTR lpValueName, CString& strValue);
	BOOL GetValue(LPCTSTR lpValueName, DWORD& dwValue);
	BOOL SetValue(LPCTSTR lpValueName, LPCTSTR lpData);
	BOOL SetValue(LPCTSTR lpValueName, DWORD dwValue);
	BOOL SaveKey(LPCTSTR lpszFileName);
	BOOL RestoreKey(LPCTSTR lpszFileName, DWORD dwFlag);
	BOOL LoadKey(enum Keys hKey, LPCTSTR lpszSubKey, LPCTSTR lpszFileName);
	void CloseKey();	

protected:
	HKEY m_hKey;
};