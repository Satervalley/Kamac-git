
#pragma once

#include "pch.h"
#include <intsafe.h>

class CKMData
{
public:
	ULONG32 ulKeyStrokes{ 0 };
	
	ULONG32 ulLeftClick{ 0 };
	ULONG32 ulRightClick{ 0 };
	ULONG32 ulMiddleClick{ 0 };

	ULONG64 ullDistance{ 0 };	// as 1/10 mm

	void Reset(void)
	{
		ulKeyStrokes = 0;
		ulLeftClick = ulRightClick = ulMiddleClick = 0;
		ullDistance = 0;
	}


	ULONG32 IncKeyStrokes(ULONG32 inc = 1)
	{
		ulKeyStrokes += inc;
		return ulKeyStrokes;
	}


	ULONG32 IncLeftClick(ULONG32 inc = 1)
	{
		ulLeftClick += inc;
		return ulLeftClick;
	}


	ULONG32 IncRightClick(ULONG32 inc = 1)
	{
		ulRightClick += inc;
		return ulRightClick;
	}


	ULONG32 IncMiddleClick(ULONG32 inc = 1)
	{
		ulMiddleClick += inc;
		return ulMiddleClick;
	}


	ULONG64 IncDistance(ULONG32 inc)
	{
		ullDistance += inc;
		return ullDistance;
	}
};

