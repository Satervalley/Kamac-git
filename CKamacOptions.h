#pragma once

#include "framework.h"

class CKamacOptions
{
public:
	BOOL bStartWithOS{ FALSE };
	BOOL bMonitorSizeConfirmed{ FALSE };
	ULONG32 ulMonitorSize{35560};	// of 1% mm
};

