#pragma once

#include <map>
//#include <vector>
#include "CKMData.h"


typedef ULONG32 Date_Key;

constexpr Date_Key Date_Key_NULL = 0 ;

#define MakeDateKey(y, m, d) (((y) << 16) + ((m) << 8) + (d))

constexpr Date_Key Date_Key_Min = MakeDateKey(2020, 1, 1);


//----------------------------------------------------------------------------------------------------------------------
int Date_Key_Comp(Date_Key dk1, Date_Key dk2)
{
	return dk1 < dk2 ? -1 : (dk1 > dk2 ? 1 : 0);
}


//----------------------------------------------------------------------------------------------------------------------
Date_Key Today(void)
{
	SYSTEMTIME st;
	::GetLocalTime(&st);
	return MakeDateKey(st.wYear, st.wMonth, st.wDay);
}


//----------------------------------------------------------------------------------------------------------------------
#pragma pack(push, rec, 4)
class CDS_Record
{
public:
	Date_Key dkDate{ Date_Key_NULL };
	CKMData kmdDay;
	CKMData kmdTotal;


	//----------------------------------------------------------------------------------------------------------------------
	void Reset(void)
	{
		Reset(Date_Key_NULL);
	}


	//----------------------------------------------------------------------------------------------------------------------
	void Reset(Date_Key dk)
	{
		dkDate = dk;
		kmdDay.Reset();
		kmdTotal.Reset();
	}


	//----------------------------------------------------------------------------------------------------------------------
	ULONG32 MouseTotal(void) const
	{
		return kmdDay.ulLeftClick + kmdDay.ulRightClick + kmdDay.ulMiddleClick;
	}


	//----------------------------------------------------------------------------------------------------------------------
	ULONG32 ActivityTotal(void) const
	{
		return MouseTotal() + kmdDay.ulKeyStrokes;
	}


	//----------------------------------------------------------------------------------------------------------------------
	CDS_Record& operator =(const CDS_Record& rec)
	{
		dkDate = rec.dkDate;
		kmdDay = rec.kmdDay;
		kmdTotal = rec.kmdTotal;
		return *this;
	}
};


//----------------------------------------------------------------------------------------------------------------------
constexpr size_t Record_Size = sizeof(CDS_Record);

#pragma pack(pop, rec)


typedef std::map<Date_Key, CDS_Record*> DS_Key_Record_Map;
//typedef std::vector<CDS_Record*> DS_Record_Vector;


/*
	file format: 
	magic code: "KamacDS!" 8 bytes
	version: 2 bytes;
	first date: Date_Key, 4  bytes;
	most key: Record_Size bytes
	most mouse: Record_Size bytes
	most activities: Record_Size bytes
	record count: 4 bytes
	records: record count * Record_Size bytes
*/
class CKamacDS_File
{
public:
	//----------------------------------------------------------------------------------------------------------------------
	static bool FileExist(LPCTSTR filename)
	{
		DWORD dw = ::GetFileAttributes(filename);
		return dw != INVALID_FILE_ATTRIBUTES && dw != FILE_ATTRIBUTE_DIRECTORY;
	}


	//----------------------------------------------------------------------------------------------------------------------
	// open record file, if file not exist, return false, no create
	bool Open(LPCTSTR filename)
	{
		ASSERT(filename);
		if (hFile != INVALID_HANDLE_VALUE)	// already opened
			return false;
		bool bRes = false;
		//Close();
		hFile = ::CreateFile(filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 
			FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			bRes = LoadBasic();
			if (!bRes)
				Close();
		}
		else
			bRes = false;
		return bRes;
	}


	//----------------------------------------------------------------------------------------------------------------------
	void Close(void)
	{
		if (hFile != INVALID_HANDLE_VALUE)
		{
			::CloseHandle(hFile);
			hFile = INVALID_HANDLE_VALUE;
		}
	}


	//----------------------------------------------------------------------------------------------------------------------
	// Create record file, if file exist, return false
	bool Create(LPCTSTR filename)
	{
		if (hFile != INVALID_HANDLE_VALUE)	// already opened
			return false;
		//Close();
		bool bRes = false;
		hFile = ::CreateFile(filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_NEW,
			FILE_ATTRIBUTE_NORMAL, nullptr);
		bRes = SaveBasic();
		return bRes;
	}

protected:
	static const char* const DS_HEAD;
	static const USHORT usVersion;
	Date_Key dkFirstDay{ Date_Key_NULL };
	CDS_Record recKeyMost;
	CDS_Record recMouseMost;
	CDS_Record recActivityMost;
	ULONG32 ulRecCount{ 0 };

	HANDLE hFile{ INVALID_HANDLE_VALUE };


	//----------------------------------------------------------------------------------------------------------------------
	CKamacDS_File()
	{

	}


	//----------------------------------------------------------------------------------------------------------------------
	virtual ~CKamacDS_File()
	{
		Close();
	}


	//----------------------------------------------------------------------------------------------------------------------
	bool LoadBasic(void)
	{
		bool bRes = false;
		if (hFile != INVALID_HANDLE_VALUE)	// opened
		{ 
			::SetFilePointer(hFile, 10l, nullptr, FILE_BEGIN);
			bRes = ReadFromFile(dkFirstDay);
			bRes = bRes && ReadRecord(recKeyMost);
			bRes = bRes && ReadRecord(recMouseMost);
			bRes = bRes && ReadRecord(recActivityMost);
			bRes = bRes && ReadFromFile(ulRecCount);
		}
		return bRes;
	}


	//----------------------------------------------------------------------------------------------------------------------
	bool SaveBasic(void)
	{
		bool bRes = false;
		if (hFile != INVALID_HANDLE_VALUE)	// create ok
		{
			::SetFilePointer(hFile, 0, nullptr, FILE_BEGIN);
			DWORD dwwb = 0;
			bRes = ::WriteFile(hFile, DS_HEAD, 8, &dwwb, nullptr);
			bRes = bRes && WriteToFile(usVersion);
			dkFirstDay = Today();
			bRes = bRes && WriteToFile(dkFirstDay);
			recKeyMost.Reset(dkFirstDay);
			bRes = bRes && WriteRecord(recKeyMost);
			recMouseMost.Reset(dkFirstDay);
			bRes = bRes && WriteRecord(recMouseMost);
			recActivityMost.Reset(dkFirstDay);
			bRes = bRes && WriteRecord(recActivityMost);
			ulRecCount = 0;
			bRes = bRes && WriteToFile(ulRecCount);
		}
		return bRes;
	}


	//----------------------------------------------------------------------------------------------------------------------
	bool IsKeyMost(const CDS_Record& rec)
	{
		return rec.kmdDay.ulKeyStrokes > recKeyMost.kmdDay.ulKeyStrokes;
	}


	//----------------------------------------------------------------------------------------------------------------------
	bool IsMouseMost(const CDS_Record& rec)
	{
		return rec.MouseTotal() > recMouseMost.MouseTotal();
	}


	//----------------------------------------------------------------------------------------------------------------------
	bool IsActivityMost(const CDS_Record& rec)
	{
		return rec.ActivityTotal() > recActivityMost.ActivityTotal();
	}


	//----------------------------------------------------------------------------------------------------------------------
	bool UpdateKeyMost(const CDS_Record& rec)
	{
		bool bRes = IsKeyMost(rec);
		if (bRes)
			recKeyMost = rec;
		return bRes;
	}


	//----------------------------------------------------------------------------------------------------------------------
	bool UpdateMouseMost(const CDS_Record& rec)
	{
		bool bRes = IsMouseMost(rec);
		if (bRes)
			recMouseMost = rec;
		return bRes;
	}


	//----------------------------------------------------------------------------------------------------------------------
	bool UpdateActivityMost(const CDS_Record& rec)
	{
		bool bRes = IsActivityMost(rec);
		if (bRes)
			recActivityMost = rec;
		return bRes;
	}


	//----------------------------------------------------------------------------------------------------------------------
	bool WriteRecord(const CDS_Record& rec)
	{
		bool bRes = false;
		if (hFile != INVALID_HANDLE_VALUE)
		{
			DWORD dwwb = 0;
			bRes = ::WriteFile(hFile, &rec, Record_Size, &dwwb, nullptr);
			if (bRes)
				bRes = (dwwb == Record_Size);
		}
		return bRes;
	}


	//----------------------------------------------------------------------------------------------------------------------
	bool ReadRecord(CDS_Record& rec)
	{
		bool bRes = false;
		if (hFile != INVALID_HANDLE_VALUE)
		{
			DWORD dwrb = 0;
			bRes = ::ReadFile(hFile, &rec, Record_Size, &dwrb, nullptr);
			if (bRes)
				bRes = (dwrb == Record_Size);
		}
		return bRes;
	}


	//----------------------------------------------------------------------------------------------------------------------
	template<typename T>
	bool WriteToFile(const T& t)
	{
		bool bRes = false;
		if (hFile != INVALID_HANDLE_VALUE)
		{
			DWORD dwwb = 0;
			bRes = ::WriteFile(hFile, &t, sizeof(T), &dwwb, nullptr);
			if (bRes)
				bRes = (dwwb == sizeof(T));
		}
		return bRes;
	}


	//----------------------------------------------------------------------------------------------------------------------
	template<typename T>
	bool ReadFromFile(T& t)
	{
		bool bRes = false;
		if (hFile != INVALID_HANDLE_VALUE)
		{
			DWORD dwrb = 0;
			bRes = ::ReadFile(hFile, &t, sizeof(T), &dwrb, nullptr);
			if (bRes)
				bRes = (dwrb == sizeof(T));
		}
		return bRes;
	}

};


//----------------------------------------------------------------------------------------------------------------------
const char * const CKamacDS_File::DS_HEAD = "KamacDS!";
const USHORT CKamacDS_File::usVersion = 0x0001;


class CKamacDS_Storage : public CKamacDS_File
{
public:
	//----------------------------------------------------------------------------------------------------------------------
	CKamacDS_Storage(): CKamacDS_File()
	{
	
	}


	//----------------------------------------------------------------------------------------------------------------------
	virtual ~CKamacDS_Storage()
	{

	}


	//----------------------------------------------------------------------------------------------------------------------
	bool Append(CDS_Record& rec)
	{
		bool bRes = false;
		if (hFile != INVALID_HANDLE_VALUE)
		{
			CDS_Record recTemp;
			bool bExist = false;
			if (ulRecCount > 0)
			{
				::SetFilePointer(hFile, -((LONG)Record_Size), nullptr, FILE_END);
				ReadRecord(recTemp);
				bExist = (Date_Key_Comp(rec.dkDate, recTemp.dkDate) == 0);
			}
			if (!bExist)
			{
				::SetFilePointer(hFile, 0, nullptr, FILE_END);
			}
			else
			{
				::SetFilePointer(hFile, -((LONG)Record_Size), nullptr, FILE_END);
			}
			bRes = WriteRecord(rec);
			if (bRes)
			{
				if (UpdateKeyMost(rec))
				{
					::SetFilePointer(hFile, 14l, nullptr, FILE_BEGIN);
					WriteRecord(recKeyMost);
				}
				if (UpdateMouseMost(rec))
				{
					::SetFilePointer(hFile, 14l + Record_Size, nullptr, FILE_BEGIN);
					WriteRecord(recMouseMost);
				}
				if (UpdateActivityMost(rec))
				{
					::SetFilePointer(hFile, 14l + Record_Size * 2, nullptr, FILE_BEGIN);
					WriteRecord(recActivityMost);
				}
				if (!bExist)
				{
					::SetFilePointer(hFile, 14l + Record_Size * 3, nullptr, FILE_BEGIN);
					ulRecCount++;
					WriteToFile(ulRecCount);
				}
			}
		}
		return bRes;
	}

protected:

};


//----------------------------------------------------------------------------------------------------------------------
class CKamacDS_Man : public CKamacDS_File
{
public:
	//----------------------------------------------------------------------------------------------------------------------
	CKamacDS_Man() : CKamacDS_File()
	{

	}


	//----------------------------------------------------------------------------------------------------------------------
	virtual ~CKamacDS_Man()
	{
		Close(true);
	}


	//----------------------------------------------------------------------------------------------------------------------
	void Close(bool bRelease)
	{
		if (bRelease)
			Release();
		CKamacDS_File::Close();
	}


	//----------------------------------------------------------------------------------------------------------------------
	bool LoadAll(void)
	{
		bool bRes = false;
		if (hFile != INVALID_HANDLE_VALUE)
		{
			bRes = LoadBasic();

		}
		bAllLoaded = bRes;
		return bRes;
	}


	//----------------------------------------------------------------------------------------------------------------------
	bool SaveAll(void)
	{
		bool bRes = false;
		bRes = SaveBasic();
		if (bAllLoaded)
		{
			bRes = bRes && SaveRecords();
		}
		return bRes;
	}


	//----------------------------------------------------------------------------------------------------------------------
	CDS_Record* Find(Date_Key dk)
	{
		auto it = mapRecords.find(dk);
		if (it != mapRecords.end())
			return it->second;
		else
			return nullptr;
	}


	//----------------------------------------------------------------------------------------------------------------------
	CDS_Record* First(void)
	{
		//if (vecRecords.size() > 0)
		//	return vecRecords[0];
		//else
		//	return nullptr;
		if (mapRecords.size() > 0)
			return mapRecords.begin()->second;
		else
			return nullptr;
	}


	//----------------------------------------------------------------------------------------------------------------------
	CDS_Record* Last(void)
	{
		//if (vecRecords.size() > 0)
		//	return *(vecRecords.rbegin());
		//else
		//	return nullptr;
		if (mapRecords.size() > 0)
			return mapRecords.rbegin()->second;
		else
			return nullptr;
	}

protected:
	CDS_Record* pRecords{ nullptr };
	DS_Key_Record_Map mapRecords;
	//DS_Record_Vector vecRecords;

	bool bAllLoaded{ false };


	//----------------------------------------------------------------------------------------------------------------------
	void Release(void)
	{
		if (pRecords)
		{
			delete[] reinterpret_cast<BYTE *>(pRecords);
			pRecords = nullptr;
		}
		mapRecords.clear();
		//vecRecords.clear();
		bAllLoaded = false;
	}


	//----------------------------------------------------------------------------------------------------------------------
	bool LoadRecords(void)
	{
		bool bRes = false;
		if (hFile != INVALID_HANDLE_VALUE)
		{
			Release();
			DWORD dwsz = ::GetFileSize(hFile, nullptr), dwrd = 0;
			if (dwsz != INVALID_FILE_SIZE)
			{
				dwsz -= (18 + Record_Size * 3);
				ulRecCount = dwsz / Record_Size;
				dwsz = ulRecCount * Record_Size;
				BYTE* pBuf = new BYTE[dwsz];
				::SetFilePointer(hFile, 18 + Record_Size * 3, nullptr, FILE_BEGIN);
				if (::ReadFile(hFile, pBuf, dwsz, &dwrd, nullptr))
				{
					if (dwsz == dwrd)
					{
						pRecords = reinterpret_cast<CDS_Record*>(pBuf);
						for (ULONG32 i = 0; i < ulRecCount; i++)
							VerifyAndAdd(pRecords[i]);
						ulRecCount = mapRecords.size();
						bRes = true;
					}
				}
				else
					ulRecCount = 0;
			}
			else
			{
				ulRecCount = 0;
			}
		}
		return bRes;
	}


	//----------------------------------------------------------------------------------------------------------------------
	bool SaveRecords(void)
	{
		bool bRes = false;
		if (hFile != INVALID_HANDLE_VALUE)
		{
			::SetFilePointer(hFile, 14 + Record_Size * 3, nullptr, FILE_BEGIN);
			ulRecCount = mapRecords.size();
			bRes = WriteToFile(ulRecCount);
			for (auto it = mapRecords.begin(); it != mapRecords.end(); it++)
			{
				bRes = bRes && WriteRecord(*(it->second));
			}
		}
		return bRes;
	}

	//----------------------------------------------------------------------------------------------------------------------
	// return value: true, added, false, overwritten or ignored
	bool VerifyAndAdd(CDS_Record& rec)
	{
		if (Date_Key_Comp(rec.dkDate, Date_Key_Min) < 0)	// too early, drop it
			return false;
		if (Date_Key_Comp(rec.dkDate, dkFirstDay) < 0)
			dkFirstDay = rec.dkDate;
		UpdateKeyMost(rec);
		UpdateMouseMost(rec);
		UpdateActivityMost(rec);
		return mapRecords.insert_or_assign(rec.dkDate, &rec).second;
	}
};
