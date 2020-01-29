#pragma once

#include "CKamacDS.h"
#include <memory>
#include <list>


template<size_t szGroup = 3> class CDataGroup
{
public:
	static const int nTotalWidth{ 64 }, nColumnWidth{ 12 }, nMargin{ 12 }, 
		nGap{ (nTotalWidth - nColumnWidth * szGroup - nMargin * 2) / (szGroup - 1) };
	static int nPos[szGroup];// {nMargin, nMargin + nColumnWidth + nGap, nMargin + (nColumnWidth + nGap) * 2};
//	static DWORD clrColors[szGroup];// { 0x00ff5252, 0x0069f0ae, 0x448aff };
	int nData[szGroup]{ 0, 0, 0 };
	Date_Key dkDate{Date_Key_NULL};
	int nDrawXLeft{ 0 }, nDrawXRight{ 0 }, nBorderXLeft{ 0 }, nBorderXRight{ 0 };
	CString strDate;
};

typedef CDataGroup<3> CDataGroup_3;
typedef std::shared_ptr<CDataGroup_3> CDataGroup_3_Pointer;
typedef std::list<CDataGroup_3_Pointer> CDataGroup_3_PointerList;



class CGraphMan
{
public:
	typedef CDataGroup_3_PointerList::iterator iterator;

	CGraphMan()
	{}

	~CGraphMan()
	{}

	void SetDS(CKamacDS_Man* pds) 
	{ 
		pdsMan = pds;
		CalcTops();
	}


	int CalcCountTop(void)
	{
		int n = 1000;
		if (pdsMan)
		{
			n = (int)pdsMan->recKeyMost.kmdDay.ulKeyStrokes;
			n = n < (int)pdsMan->recMouseMost.MouseTotal() ? (int)pdsMan->recMouseMost.MouseTotal() : n;
			n = n / 1000 * 1000 + 1000;
		}
		nCountTop = n;
		return n;
	}

	
	int CalcDistanceTop(void)
	{
		int n = 1000;
		if (pdsMan)
		{
			n = (int)pdsMan->recMoveMost.DistanceAsMeter();
			n = n / 1000 * 1000 + 1000;
		}
		nDistanceTop = n;
		return n;
	}

	void CalcTops(void)
	{
		CalcCountTop();
		CalcDistanceTop();
	}


	int GetCountTop(void) const { return nCountTop; }
	int GetDistanceTop(void) const { return nDistanceTop; }
	size_t DataGroupCount(void) const { return dgplData.size(); }

	size_t Init(const CRect& rc)
	{
		CalcTops();
		dgplData.clear();
		if (pdsMan)
		{
			dkFirst = pdsMan->dkFirstDay; 
			dkLast = DateKeyYestoday();
			Date_Key dk = dkLast;
			for (int i = 0; ; i++)
			{
				if (dk < dkFirst)
					break;
				CDS_Record * prec = pdsMan->Find(dk);
				CDataGroup_3_Pointer pdg = std::make_shared<CDataGroup_3>();
				if (prec)
				{
					pdg->nData[0] = (int)prec->kmdDay.ulKeyStrokes;
					pdg->nData[1] = (int)prec->MouseTotal();
					pdg->nData[2] = (int)prec->DistanceAsMeter();
				}
				pdg->dkDate = dk;
				pdg->strDate = MakeDateString(dk, false);
				pdg->nBorderXRight = rc.right - CDataGroup_3::nTotalWidth * i;
				pdg->nBorderXLeft = pdg->nBorderXRight - CDataGroup_3::nTotalWidth;
				pdg->nDrawXRight = pdg->nBorderXRight - CDataGroup_3::nMargin;
				pdg->nDrawXLeft = pdg->nBorderXLeft + CDataGroup_3::nMargin;
				if (pdg->nDrawXRight > rc.left)
					dgplData.push_back(pdg);
				else
					break;
				dk = DateKeyPrevDay(dk);
			}
		}
		return dgplData.size();
	}


	// return true: need redraw
	bool UpdateDrag(int dis, const CRect& rc)
	{
		bool bRes = false;
		if (!dgplData.empty())
		{
			int diff = 0, pos;
			if (dis < 0)	// drag right to left
			{
				auto it = dgplData.begin();
				dis = -dis;
				diff = (*it)->nBorderXRight - rc.right;
				pos = (*it)->nBorderXRight;
				if (dis <= diff)
				{
					diff = -dis;
				}
				else
				{
					Date_Key dk = (*it)->dkDate;
					for (;;)
					{
						dk = DateKeyNextDay(dk);
						if (dk > dkLast)
							break;
						CDS_Record* prec = pdsMan->Find(dk);
						CDataGroup_3_Pointer pdg = std::make_shared<CDataGroup_3>();
						if (prec)
						{
							pdg->nData[0] = (int)prec->kmdDay.ulKeyStrokes;
							pdg->nData[1] = (int)prec->MouseTotal();
							pdg->nData[2] = (int)prec->DistanceAsMeter();
						}
						pdg->dkDate = dk;
						pdg->strDate = MakeDateString(dk, false);
						pdg->nBorderXLeft = pos;
						pdg->nBorderXRight = pos + CDataGroup_3::nTotalWidth;
						pdg->nDrawXRight = pdg->nBorderXRight - CDataGroup_3::nMargin;
						pdg->nDrawXLeft = pdg->nBorderXLeft + CDataGroup_3::nMargin;
						pos = pdg->nBorderXRight;
						dgplData.push_front(pdg);
						diff += CDataGroup_3::nTotalWidth;
						if (diff >= dis)
							break;
					}
					diff = diff > dis ? dis : diff;
					diff = -diff;
				}
			}
			else // drag left to right
			{
				auto it = dgplData.rbegin();
				diff = rc.left - (*it)->nBorderXLeft;
				pos = (*it)->nBorderXLeft;
				if (diff >= dis)
				{
					diff = dis;
				}
				else
				{
					Date_Key dk = (*it)->dkDate;
					for (;;)
					{
						dk = DateKeyPrevDay(dk);
						if (dk < dkFirst)
							break;
						CDS_Record* prec = pdsMan->Find(dk);
						CDataGroup_3_Pointer pdg = std::make_shared<CDataGroup_3>();
						if (prec)
						{
							pdg->nData[0] = (int)prec->kmdDay.ulKeyStrokes;
							pdg->nData[1] = (int)prec->MouseTotal();
							pdg->nData[2] = (int)prec->DistanceAsMeter();
						}
						pdg->dkDate = dk;
						pdg->strDate = MakeDateString(dk, false);
						pdg->nBorderXRight = pos;
						pdg->nBorderXLeft = pos - CDataGroup_3::nTotalWidth;
						pdg->nDrawXRight = pdg->nBorderXRight - CDataGroup_3::nMargin;
						pdg->nDrawXLeft = pdg->nBorderXLeft + CDataGroup_3::nMargin;
						pos = pdg->nBorderXLeft;
						dgplData.push_back(pdg);
						diff += CDataGroup_3::nTotalWidth;
						if (diff >= dis)
							break;
					}
					diff = diff > dis ? dis : diff;
				}
			}
			bRes = (diff != 0);
			if(bRes)
				TranslateAndEraseInvisible(diff, rc);
		}
		return bRes;
	}


	CDataGroup_3_PointerList& GetDataGroups(void) { return dgplData; }

	CDataGroup_3_Pointer OnWhich(const CPoint& pt)
	{
		CDataGroup_3_Pointer pdg(nullptr);

		for (auto it = dgplData.begin(); it != dgplData.end(); it++)
		{
			if (pt.x >= (*it)->nDrawXLeft && pt.x <= (*it)->nDrawXRight)
			{
				pdg = *it;
				break;
			}
		}
		return pdg;
	}
protected:
	CKamacDS_Man* pdsMan{ nullptr };
	Date_Key dkFirst{ Date_Key_NULL }, dkLast{ Date_Key_NULL };
	int nCountTop{ 1000 }, nDistanceTop{ 1000 };
	CDataGroup_3_PointerList dgplData;

	size_t TranslateAndEraseInvisible(int nDiff, const CRect& rect)
	{
		size_t sz = 0;
		for (auto it = dgplData.begin(); it != dgplData.end();)
		{
			CDataGroup_3_Pointer pdg = *it;
			if (pdg)
			{
				pdg->nDrawXLeft += nDiff;
				pdg->nDrawXRight += nDiff;
				if (pdg->nDrawXLeft > rect.right || pdg->nDrawXRight < rect.left)
				{
					it = dgplData.erase(it);
					sz++;
					continue;
				}
				else
				{
					pdg->nBorderXLeft += nDiff;
					pdg->nBorderXRight += nDiff;
					it++;
				}
			}
			else
				it++;
		}
		return sz;
	}
};

