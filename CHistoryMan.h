#pragma once

#include "itzamDatabase.h"
#include "CKMData.h"


struct SKMKey
{
	uint32_t uDate;
	CKMData kmDay;
	CKMData kmTotal;

	void MakeDate(const SYSTEMTIME& st)
	{
		uDate = st.wYear;
		uDate <<= 16;
		uDate += st.wMonth;
		uDate <<= 8;
		uDate += st.wDay;
	}

	void Split(short& ny, short& nm, short& nd)
	{
		nd = short(uDate & 0x000000ff);
		nm = short((uDate & 0x0000ff00) >> 8);
		ny = short(uDate >> 16);
	}

	void From(void* p)
	{
		::memcpy(this, p, sizeof(SKMKey));
	}
};


int SKMKeyComparator(const void* key1, const void* key2);


class CHistoryMan
{
protected:

public:

	CHistoryMan(const char* path)
	{

	}


	bool Open(void)
	{
		return false;
	}
};

