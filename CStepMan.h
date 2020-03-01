#pragma once

#include "framework.h"
#include <assert.h>


class CStepMan
{
public:
	CStepMan(size_t sz) : szCount{ sz }
	{
		pSteps = new int[sz];
	}


	~CStepMan()
	{
		if (pSteps)
			delete[] pSteps;
	}


	/*
	1 2 4 8 16    31  2 ^ 5 - 1
	1 2 4 2 1      10 (2 ^ 3 - 1) * 2 - 2 ^ 2

	1 2 4 8 16 32    63 2 ^ 6 - 1
	*/
	void SetSpan_Inc(int ns)
	{
		BasicSplitSpan(ns, szCount, pSteps);
	}


	/*
	1 2 4 8 16    31  2 ^ 5 - 1
	1 2 4 2 1      10 (2 ^ 3 - 1) * 2 - 2 ^ 2

	1 2 4 8 16 32    63 2 ^ 6 - 1
	1 2 4 4 2 1      14  (2 ^ 3 - 1) * 2

	1 2 4 8 4 2 1   22 (2 ^ 4 - 1) * 2 - 2 ^ 3 = 22

	1 2 4 8 8 4 2 1   30  (2 ^ 4 - 1) * 2 = 30

	*/
	void SetSpan_IncDec(int ns)
	{
		assert(ns != 0);
		assert(szCount > 2);

		size_t szHalf = szCount / 2;
		if (szCount % 2)	// odd
		{
			int nSig = ns < 0 ? -1 : 1;
			ns = ns < 0 ? -ns : ns;
			int nFrac = (((2 << szHalf) - 1) << 1) - (2 << (szHalf - 1));
			int nFracThird = (2 << (szHalf - 1)) - 1;
			float f = float(nFracThird) / float(nFrac);
			int nSpanThird = int(f * float(ns) + .5f);
			if (nSpanThird < 1)
				BasicSplitSpan(ns * nSig, szCount, pSteps);
			else
			{
				size_t szPos = BasicSplitSpan(nSpanThird * nSig, szHalf, pSteps);
				ns -= (nSpanThird << 1);
				if (ns > 0)
				{
					pSteps[szPos] = ns * nSig;
					for (size_t i = 1; i <= szPos; i++)
					{
						pSteps[szPos + i] = pSteps[szPos - i];
					}
					for (size_t i = szPos * 2 + 1; i < szCount; i++)
						pSteps[i] = 0;
				}
				else
				{
					for (size_t i = 0; i < szPos; i++)
					{
						pSteps[szPos + i] = pSteps[szPos - i - 1];
					}
					for (size_t i = szPos * 2; i < szCount; i++)
						pSteps[i] = 0;
				}
			}
		}
		else // even
		{
			int nSpanHalf = ns / 2;
			if (nSpanHalf == 0)
			{
				BasicSplitSpan(ns, szCount, pSteps);
			}
			else
			{
				size_t szPos = BasicSplitSpan(nSpanHalf, szHalf, pSteps);
				for (size_t i = 0; i < szPos; i++)
				{
					pSteps[szPos + i] = pSteps[szPos - 1 - i];
				}
				for (size_t i = szPos * 2; i < szCount; i++)
					pSteps[i] = 0;
				if (szPos >= szHalf)
					pSteps[szPos] += (ns - nSpanHalf * 2);
				else
					pSteps[szPos * 2] = (ns - nSpanHalf * 2);
			}
		}
	}


	int Get(size_t at, bool bReverse = false) const
	{
		assert(at < szCount);
		if (bReverse)
			return pSteps[szCount - at - 1];
		else
			return pSteps[at];
	}

	int operator[](size_t at) const
	{
		return Get(at);
	}

	size_t Count(void) const { return szCount; }
protected:
	size_t szCount;
	int* pSteps;

	static size_t BasicSplitSpan(int ns, size_t sz, int* pRes)
	{
		assert(ns != 0);
		assert(sz > 0);
		assert(pRes);

		int nFraction = (1 << sz) - 1;
		int nMulti = ns > 0 ? 1 : -1;
		ns = ns < 0 ? -ns : ns;
		float fbs = float(ns) / float(nFraction);
		int nTotal = 0;
		size_t szLast = sz - 1, szFirst = 0;
		for (size_t i = 0; i < sz; i++)
		{
			if (nTotal >= ns)
				break;
			int n = int(fbs * float(1 << i) + .5f);
			if (n == 0)
				szFirst++;
			else
			{
				nTotal += n;
				pRes[i - szFirst] = n * nMulti;
				if (nTotal >= ns)
					szLast = i - szFirst;
			}
		}
		pRes[szLast] += (ns - nTotal) * nMulti;
		for (size_t i = szLast + 1; i < sz; i++)
			pRes[i] = 0;
		return szLast + 1;
	}
};

