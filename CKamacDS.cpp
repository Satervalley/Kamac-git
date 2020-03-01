#include "pch.h"
#include "CKamacDS.h"

//----------------------------------------------------------------------------------------------------------------------
int Date_Key_Comp(Date_Key dk1, Date_Key dk2)
{
	return dk1 < dk2 ? -1 : (dk1 > dk2 ? 1 : 0);
}


//----------------------------------------------------------------------------------------------------------------------
bool Date_Key_Between(Date_Key dk, Date_Key dk1, Date_Key dk2, bool bInc)
{
	if(bInc)
		return (dk >= DateKeyMin(dk1, dk2) && dk <= DateKeyMax(dk1, dk2));
	else
		return (dk > DateKeyMin(dk1, dk2) && dk < DateKeyMax(dk1, dk2));
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
int DateKeyDiff(Date_Key dk1, Date_Key dk2)
{
	CTime tm1(DateKeyYear(dk1), DateKeyMonth(dk1), DateKeyDay(dk1), 1, 0, 0);
	CTime tm2(DateKeyYear(dk2), DateKeyMonth(dk2), DateKeyDay(dk2), 1, 0, 0);
	CTimeSpan ts = tm1 - tm2;
	return int(ts.GetDays());
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

