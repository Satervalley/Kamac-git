#pragma once

#include <map>
//#include <vector>
#include "CKMData.h"


typedef ULONG32 Date_Key;

constexpr Date_Key Date_Key_NULL = 0 ;

#define MakeDateKey(y, m, d) (((y) << 16) + ((m) << 8) + (d))
#define DateKeyYear(dk) ((dk) >> 16)
#define DateKeyMonth(dk) (((dk) >> 8) & 0x000000ff)
#define DateKeyDay(dk) ((dk) & 0x000000ff)
#define DateKeyMin(dk1, dk2) (((dk1) < (dk2))? (dk1) : (dk2))
#define DateKeyMax(dk1, dk2) (((dk1) < (dk2))? (dk2) : (dk1))


constexpr Date_Key Date_Key_Min = MakeDateKey(2020, 1, 1);


//----------------------------------------------------------------------------------------------------------------------
int Date_Key_Comp(Date_Key dk1, Date_Key dk2);
bool Date_Key_Between(Date_Key dk, Date_Key dk1, Date_Key dk2, bool bInc = true);
Date_Key DateKeyToday(void);
Date_Key DateKeyYestoday(void);
Date_Key DateKeyAddDay(Date_Key dk, int diff);
Date_Key DateKeyNextDay(Date_Key dk);
Date_Key DateKeyPrevDay(Date_Key dk);
int DateKeyDiff(Date_Key dk1, Date_Key dk2);
CString MakeDateString(Date_Key dk, bool bWithYear = true);


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
	ULONG32 DistanceAsMeter(void) const
	{
		return ULONG32((kmdDay.ullDistance + 5000) / 10000);
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
			strFileName = filename;
		}
		else
			bRes = false;
		return bRes;
	}


	//----------------------------------------------------------------------------------------------------------------------
	bool Reopen(void)
	{
		Close();
		bool bRes = false;
		if (!strFileName.IsEmpty())
		{
			bRes = Open(strFileName);
		}
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

public:
	static const char* const DS_HEAD;
	static const USHORT usVersion;
	Date_Key dkFirstDay{ Date_Key_NULL };
	CDS_Record recKeyMost;
	CDS_Record recMouseMost;
	CDS_Record recActivityMost;
	CDS_Record recMoveMost;
	ULONG32 ulRecCount{ 0 };

	CString strFileName;
	HANDLE hFile{ INVALID_HANDLE_VALUE };

protected:
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
			bRes = bRes && ReadRecord(recMoveMost);
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
			dkFirstDay = DateKeyToday();
			bRes = bRes && WriteToFile(dkFirstDay);
			recKeyMost.Reset(dkFirstDay);
			bRes = bRes && WriteRecord(recKeyMost);
			recMouseMost.Reset(dkFirstDay);
			bRes = bRes && WriteRecord(recMouseMost);
			recActivityMost.Reset(dkFirstDay);
			bRes = bRes && WriteRecord(recActivityMost);
			recMoveMost.Reset(dkFirstDay);
			bRes = bRes && WriteRecord(recMoveMost);
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
	bool IsMoveMost(const CDS_Record& rec)
	{
		return rec.kmdDay.ullDistance > recMoveMost.kmdDay.ullDistance;
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
	bool UpdateMoveMost(const CDS_Record& rec)
	{
		bool bRes = IsMoveMost(rec);
		if (bRes)
			recMoveMost = rec;
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
				if (UpdateMoveMost(rec))
				{
					::SetFilePointer(hFile, 14l + Record_Size * 3, nullptr, FILE_BEGIN);
					WriteRecord(recMoveMost);
				}
				if (!bExist)
				{
					::SetFilePointer(hFile, 14l + Record_Size * 4, nullptr, FILE_BEGIN);
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
			bRes = bRes && LoadRecords();
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
		it = mapRecords.begin();
		if (mapRecords.size() > 0)
			return it->second;
		else
			return nullptr;
	}


	//----------------------------------------------------------------------------------------------------------------------
	CDS_Record* Last(void)
	{
		it = mapRecords.end();
		if (mapRecords.size() > 0)
			return mapRecords.rbegin()->second;
		else
			return nullptr;
	}

	//----------------------------------------------------------------------------------------------------------------------
	CDS_Record* Next(void)
	{
		if (it != mapRecords.end())
		{
			it++;
		}
		if (it == mapRecords.end())
			return nullptr;
		else
			return it->second;
	}
protected:
	CDS_Record* pRecords{ nullptr };
	DS_Key_Record_Map mapRecords;
	DS_Key_Record_Map::iterator it;
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
				dwsz -= (18 + Record_Size * 4);
				ulRecCount = dwsz / Record_Size;
				dwsz = ulRecCount * Record_Size;
				BYTE* pBuf = new BYTE[dwsz];
				::SetFilePointer(hFile, 18 + Record_Size * 4, nullptr, FILE_BEGIN);
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
			::SetFilePointer(hFile, 14 + Record_Size * 4, nullptr, FILE_BEGIN);
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
