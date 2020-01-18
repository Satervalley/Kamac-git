// KamacHisViewer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "..\CKamacDS.h"

std::ostream& OutputDate(Date_Key dk)
{
    return (std::cout << (dk >> 16) << "." << ((dk >> 8) & 0x000000ff) << "." << (dk & 0x000000ff));
}


std::ostream& OutputKMData(const CKMData& kmd)
{
    return (std::cout << "Key: " << kmd.ulKeyStrokes << "    Left: " << kmd.ulLeftClick << "    Right: " << kmd.ulRightClick <<
        "    Middle: " << kmd.ulMiddleClick << "    Distance: " << kmd.ullDistance);
}


void OutputRecord(const CDS_Record& rec)
{
    std::cout << "Date: ";
    OutputDate(rec.dkDate) << std::endl;
    std::cout << "Day: ";
    OutputKMData(rec.kmdDay) << std::endl;
    std::cout << "Total: ";
    OutputKMData(rec.kmdTotal) << std::endl << std::endl;
}


int _tmain(int argc, TCHAR ** argv)
{
    if (argc != 2)
    {
        std::cout << "Usage: KamacHisViewer HisFileName" << std::endl;
        return 1;
    }
    //std::cout << argv[0] << std::endl;
    //std::cout << argv[1] << std::endl;

    CKamacDS_Man dsman;
    if (!dsman.Open(argv[1]))
    {
        std::cout << "Open file \"" << argv[1] << "\" failed!" << std::endl;
        return 2;
    }
    if (!dsman.LoadAll())
    {
        std::cout << "Load records failed!" << std::endl;
        return 3;
    }
    
    std::cout << "History file version: " << dsman.usVersion << std::endl;
    std::cout << "First Day: ";
    OutputDate(dsman.dkFirstDay) << std::endl << std::endl;
    std::cout << "Key most: " << std::endl;
    OutputRecord(dsman.recKeyMost);
    std::cout << "Mouse most: " << std::endl;
    OutputRecord(dsman.recMouseMost);
    std::cout << "Activity most: " << std::endl;
    OutputRecord(dsman.recActivityMost);
    std::cout << "Move most: " << std::endl;
    OutputRecord(dsman.recMoveMost);
    std::cout << "Record count: " << dsman.ulRecCount << std::endl << std::endl;
    int i = 1;
    for (CDS_Record* pRec = dsman.First(); pRec != nullptr; pRec = dsman.Next())
    {
        std::cout << "Record " << i++ << ":" << std::endl;
        OutputRecord(*pRec);
        std::cout << std::endl;
    }
    return 0;
}

