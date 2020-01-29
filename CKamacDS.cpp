#include "pch.h"
#include "CKamacDS.h"

//----------------------------------------------------------------------------------------------------------------------
int Date_Key_Comp(Date_Key dk1, Date_Key dk2)
{
	return dk1 < dk2 ? -1 : (dk1 > dk2 ? 1 : 0);
}


//----------------------------------------------------------------------------------------------------------------------
Date_Key DateKeyToday(void)
{
	SYSTEMTIME st;
	::GetLocalTime(&st);
	return MakeDateKey(st.wYear, st.wMonth, st.wDay);
}


//----------------------------------------------------------------------------------------------------------------------
Date_Key DateKeyYestoday(void)
{
	return DateKeyPrevDay(DateKeyToday());
}


//----------------------------------------------------------------------------------------------------------------------
Date_Key DateKeyAddDay(Date_Key dk, int diff)
{
	CTimeSpan ts(diff < 0 ? -diff : diff, 0, 0, 0);
	CTime tm(DateKeyYear(dk), DateKeyMonth(dk), DateKeyDay(dk), 1, 0, 0);
	CTime tmd = diff < 0 ? tm - ts : tm + ts;
	return MakeDateKey(tmd.GetYear(), tmd.GetMonth(), tmd.GetDay());
}


//----------------------------------------------------------------------------------------------------------------------
Date_Key DateKeyNextDay(Date_Key dk)
{
	return DateKeyAddDay(dk, 1);
}


//----------------------------------------------------------------------------------------------------------------------
Date_Key DateKeyPrevDay(Date_Key dk)
{
	return DateKeyAddDay(dk, -1);
}


//----------------------------------------------------------------------------------------------------------------------
CString MakeDateString(Date_Key dk, bool bWithYear)
{
	CString str;
	if(bWithYear)
		str.Format(_T("%d.%02d.%02d"), DateKeyYear(dk), DateKeyMonth(dk), DateKeyDay(dk));
	else
		str.Format(_T("%02d.%02d"), DateKeyMonth(dk), DateKeyDay(dk));
	return str;
}


const char* const CKamacDS_File::DS_HEAD = "KamacDS!";
const USHORT CKamacDS_File::usVersion = 0x0001;

