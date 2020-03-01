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


	bool CalcTops(void)
	{
		bool bRes = false;
		bRes = (bRes || CalcCountTop());
		bool b = CalcDistanceTop();
		bRes = (bRes || b);
		return bRes;
	}


	int GetCountTop(bool bCurScope = false) const 
	{
		if (bCurScope)
		{
			int n = 0;
			for (auto it = dgplData.begin(); it != dgplData.end(); it++)
			{
				if (n < (*it)->nData[0])
					n = (*it)->nData[0];
				if (n < (*it)->nData[1])
					n = (*it)->nData[1];
			}
			n = NormalCountTop(n);
			return n;
		}
		else
			return nCountTop; 
	}
	
	int GetDistanceTop(bool bCurScope = false) const 
	{ 
		if (bCurScope)
		{
			int n = 0;
			for (auto it = dgplData.begin(); it != dgplData.end(); it++)
			{
				if (n < (*it)->nData[2])
					n = (*it)->nData[2];
			}
			n = NormalDistanceTop(n);
			return n;
		}
		else
			return nDistanceTop; 
	}
	
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
				CDataGroup_3_Pointer pdg = FindAndMakeLeft(dk, rc.right - CDataGroup_3::nTotalWidth * i);
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
	bool UpdateDrag(int dis, const CRect& rc, bool bErase = true)
	{
		bool bRes = false;
		if (dis < 0)
			bRes = UpdateDragRightToLeft(dis, rc, bErase);
		else
			bRes = UpdateDragLeftToRight(dis, rc, bErase);
		return bRes;
	}


	// return true: need redraw
	bool UpdateDragRightToLeft(int dis, const CRect& rc, bool bErase = true)
	{
		ASSERT(dis < 0);

		bool bRes = false;
		if (!dgplData.empty())
		{
			int diff = 0, pos;
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
					CDataGroup_3_Pointer pdg = FindAndMakeRight(dk, pos);
					pos = pdg->nBorderXRight;
					dgplData.push_front(pdg);
					diff += CDataGroup_3::nTotalWidth;
					if (diff >= dis)
						break;
				}
				diff = diff > dis ? dis : diff;
				diff = -diff;
			}
			bRes = (diff != 0);
			Translate(diff);
			if (bRes && bErase)
				EraseInvisibleLeft(rc);
		}
		return bRes;
	}

	// return true: need redraw
	bool UpdateDragLeftToRight(int dis, const CRect& rc, bool bErase = true)
	{
		ASSERT(dis >= 0);

		bool bRes = false;
		if (!dgplData.empty())
		{
			int diff = 0, pos;
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
					CDataGroup_3_Pointer pdg = FindAndMakeLeft(dk, pos);
					pos = pdg->nBorderXLeft;
					dgplData.push_back(pdg);
					diff += CDataGroup_3::nTotalWidth;
					if (diff >= dis)
						break;
				}
				diff = diff > dis ? dis : diff;
			}
			bRes = (diff != 0);
			Translate(diff);
			if (bRes && bErase)
				EraseInvisibleRight(rc);
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


	int GetPageDistance(const CRect& rc, bool bNext = false)
	{
		int nDis = 0;
		CDataGroup_3_Pointer pdg;
		if (bNext)	// next page, to the lastest, return nDis <= 0
		{
			// find the most right
			pdg = RightVisible(rc);
			if (pdg)
				nDis = rc.left - pdg->nBorderXLeft;
		}
		else  // prev page, to the oldest, return nDis >= 0
		{
			// find the most left
			pdg = LeftVisible(rc);
			if (pdg)
				nDis = rc.right - pdg->nBorderXRight;
		}
		return nDis;
	}


	static int MaxVisible(int nWidth)
	{
		int n = 1, nt;
		nWidth -= CDataGroup_3::nMargin;
		nt = nWidth / CDataGroup_3::nTotalWidth;
		n += nt;
		nWidth -= nt * CDataGroup_3::nTotalWidth;
		n += (nWidth > CDataGroup_3::nMargin ? 1 : 0);
		return n;
	}


	int PrepareForNaviToDate(Date_Key dkDate, int nWidth, int nLeftPos)
	{
		int nRes = 0;
		int nMV = MaxVisible(nWidth);
		int nDisLimit = nMV * 2;
		CDataGroup_3_Pointer pdgLeft = Left(), pdgRight = Right();
		if (DataGroupCount() > 0)
		{
			if (dkDate >= pdgLeft->dkDate && dkDate <= pdgRight->dkDate)
			{
				CDataGroup_3_Pointer pdg = FindInCurrentScope(dkDate);
				if (pdg)
					nRes = nLeftPos - pdg->nBorderXLeft;
			}
			if (dkDate < pdgLeft->dkDate)
			{
				int nDisDay = DateKeyDiff(pdgLeft->dkDate, dkDate);
				int nPos = pdgLeft->nBorderXLeft;
				Date_Key dk = DateKeyPrevDay(pdgLeft->dkDate);
				if (nDisDay > nDisLimit) // fold some days
				{
					for (int i = 0; i < nMV; i++)
					{
						if (dk < dkDate || dk < dkFirst)
							break;
						CDataGroup_3_Pointer pdg = FindAndMakeLeft(dk, nPos);
						dgplData.push_back(pdg);
						nPos -= CDataGroup_3::nTotalWidth;
						dk = DateKeyPrevDay(dk);
					}
					dk = DateKeyAddDay(dkDate, nMV - 1);
				}
				for (; dk >= dkDate; dk = DateKeyPrevDay(dk))
				{
					CDataGroup_3_Pointer pdg = FindAndMakeLeft(dk, nPos);
					dgplData.push_back(pdg);
					nPos -= CDataGroup_3::nTotalWidth;
				}
				nRes = nLeftPos - nPos;
			}
			if (dkDate > pdgRight->dkDate)
			{
				int nDisDay = DateKeyDiff(dkDate, pdgRight->dkDate);
				int nPos = pdgRight->nBorderXRight;
				Date_Key dk = DateKeyNextDay(pdgRight->dkDate);
				if (nDisDay > nDisLimit) // fold some days
				{
					for (int i = 0; i < nMV; i++)
					{
						if (dk > dkDate || dk > dkLast)
							break;
						CDataGroup_3_Pointer pdg = FindAndMakeRight(dk, nPos);
						dgplData.push_front(pdg);
						nPos += CDataGroup_3::nTotalWidth;
						dk = DateKeyNextDay(dk);
					}
					dk = DateKeyAddDay(dkDate, -(nMV - 1));
				}
				for (; dk <= dkDate; dk = DateKeyNextDay(dk))
				{
					CDataGroup_3_Pointer pdg = FindAndMakeRight(dk, nPos);
					dgplData.push_front(pdg);
					nPos += CDataGroup_3::nTotalWidth;
				}
				nRes = nLeftPos + CDataGroup_3::nTotalWidth - nPos;
			}
		}
		return nRes;
	}


	// return the move distance
	int PrepareForNaviToFirst(int nWidth, int nLeftPos)
	{
		return PrepareForNaviToDate(dkFirst, nWidth, nLeftPos);
	}

	// return the move distance
	int PrepareForNaviToLast(int nWidth, int nLeftPos)
	{
		return PrepareForNaviToDate(dkLast, nWidth, nLeftPos);
	}


protected:
	CKamacDS_Man* pdsMan{ nullptr };
	Date_Key dkFirst{ Date_Key_NULL }, dkLast{ Date_Key_NULL };
	int nCountTop{ 1000 }, nDistanceTop{ 1000 };
	CDataGroup_3_PointerList dgplData;


	// return true: count top updated, otherwise not updated
	bool CalcCountTop(void)
	{
		int n = 1000;
		if (pdsMan)
		{
			n = (int)pdsMan->recKeyMost.kmdDay.ulKeyStrokes;
			n = n < (int)pdsMan->recMouseMost.MouseTotal() ? (int)pdsMan->recMouseMost.MouseTotal() : n;
			n = NormalCountTop(n);
		}
		bool bRes = (nCountTop != n);
		nCountTop = n;
		return bRes;
	}


	int NormalCountTop(int n) const
	{
		n = n * 12 / 10;
		n = n / 1000 * 1000 + 1000;
		return n;
	}


	// return true: distance top updated, otherwise not updated
	bool CalcDistanceTop(void)
	{
		int n = 200;
		if (pdsMan)
		{
			n = (int)pdsMan->recMoveMost.DistanceAsMeter();
			n = NormalDistanceTop(n);
		}
		bool bRes = (nDistanceTop != n);
		nDistanceTop = n;
		return bRes;
	}


	int NormalDistanceTop(int n) const
	{
		n = n * 12 / 10;
		n = n / 200 * 200 + 200;
		return n;
	}

	CDataGroup_3_Pointer FindInCurrentScope(Date_Key dk)
	{
		CDataGroup_3_Pointer pdg;
		for (auto it = dgplData.begin(); it != dgplData.end(); it++)
		{
			if ((*it)->dkDate == dk)
			{
				pdg = *it;
				break;
			}
		}
		return pdg;
	}


	CDataGroup_3_Pointer FindAndMakeLeft(Date_Key dk, int nPosRight)
	{
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
		pdg->nBorderXRight = nPosRight;
		pdg->nBorderXLeft = pdg->nBorderXRight - CDataGroup_3::nTotalWidth;
		pdg->nDrawXRight = pdg->nBorderXRight - CDataGroup_3::nMargin;
		pdg->nDrawXLeft = pdg->nBorderXLeft + CDataGroup_3::nMargin;
		return pdg;
	}


	CDataGroup_3_Pointer FindAndMakeRight(Date_Key dk, int nPosLeft)
	{
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
		pdg->nBorderXLeft = nPosLeft;
		pdg->nBorderXRight = nPosLeft + CDataGroup_3::nTotalWidth;
		pdg->nDrawXRight = pdg->nBorderXRight - CDataGroup_3::nMargin;
		pdg->nDrawXLeft = pdg->nBorderXLeft + CDataGroup_3::nMargin;
		return pdg;
	}

	void Translate(int nDiff)
	{
		for (auto it = dgplData.begin(); it != dgplData.end(); it++)
		{
			CDataGroup_3_Pointer pdg = *it;
			if (pdg)
			{
				pdg->nDrawXLeft += nDiff;
				pdg->nDrawXRight += nDiff;
				pdg->nBorderXLeft += nDiff;
				pdg->nBorderXRight += nDiff;
			}
		}
	}

	size_t EraseInvisibleRight(const CRect& rect)
	{
		size_t sz = 0;
		for (auto it = dgplData.begin(); it != dgplData.end();)
		{
			CDataGroup_3_Pointer pdg = *it;
			if (pdg)
			{
				if (pdg->nDrawXLeft > rect.right)
				{
					it = dgplData.erase(it);
					sz++;
				}
				else
					break;
			}
			else
			{
				it = dgplData.erase(it);
				sz++;
			}
		}
		return sz;
	}

	size_t EraseInvisibleLeft(const CRect& rect)
	{
		size_t sz = 0;
		dgplData.reverse();
		for (auto it = dgplData.begin(); it != dgplData.end();)
		{
			CDataGroup_3_Pointer pdg = *it;
			if (pdg)
			{
				if (pdg->nDrawXRight < rect.left)
				{
					it = dgplData.erase(it);
					sz++;
				}
				else
					break;
			}
			else
			{
				it = dgplData.erase(it);
				sz++;
			}
		}
		dgplData.reverse();
		return sz;
	}

	size_t EraseInvisible(const CRect& rect)
	{
		return EraseInvisibleRight(rect) + EraseInvisibleLeft(rect);
	}

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

	bool DataGroupVisible(const CDataGroup_3_Pointer& pdg, const CRect& rect)
	{
		return !(pdg->nDrawXLeft > rect.right || pdg->nDrawXRight < rect.left);
	}


	CDataGroup_3_Pointer LeftVisible(const CRect& rc)
	{
		CDataGroup_3_Pointer pdg;
		for (auto it = dgplData.rbegin(); it != dgplData.rend(); it++)
		{
			if (DataGroupVisible(*it, rc))
			{
				pdg = *it;
				break;
			}
		}
		return pdg;
	}


	CDataGroup_3_Pointer RightVisible(const CRect& rc)
	{
		CDataGroup_3_Pointer pdg;
		for (auto it = dgplData.begin(); it != dgplData.end(); it++)
		{
			if (DataGroupVisible(*it, rc))
			{
				pdg = *it;
				break;
			}
		}
		return pdg;
	}

	CDataGroup_3_Pointer Left(void)
	{
		CDataGroup_3_Pointer pdg;
		if (!dgplData.empty())
			pdg = *(dgplData.rbegin());
		return pdg;
	}


	CDataGroup_3_Pointer Right(void)
	{
		CDataGroup_3_Pointer pdg;
		if (!dgplData.empty())
			pdg = *(dgplData.begin());
		return pdg;
	}
};

