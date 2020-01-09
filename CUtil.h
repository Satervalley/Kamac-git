#pragma once

#include "framework.h"

class CUtil
{
public:
    static LPTSTR itoa(int v, LPTSTR des, bool bClose = true)
    {
        static TCHAR buf[12] = { _T('\0') };
        int d = v;
        int m = v;
        int l = 0;
        do
        {
            m = (d % 10);
            d = (d / 10);
            buf[10 - l] = m + 48;
            ++l;
        } while (d > 0);
        buf[11] = 0;
        LPTSTR stt = &buf[11 - l];
        int pos = 0;
        while (stt[pos])
        {
            des[pos++] = stt[pos];
        }
        if (bClose)
            des[pos++] = 0;
        return des + pos;
    }

    static LPCTSTR itoa(int v)
    {
        static TCHAR buf[12] = { _T('\0') };
        int d = v;
        int m = v;
        int l = 0;
        do
        {
            m = (d % 10);
            d = (d / 10);
            buf[10 - l] = m + 48;
            ++l;
        } while (d > 0);
        buf[11] = 0;
        return &buf[11 - l];
    }

    static LPTSTR uitoa(unsigned int v, LPTSTR des, bool bClose = true)
    {
        static TCHAR buf[12] = { _T('\0') };
        int d = v;
        int m = v;
        int l = 0;
        do
        {
            m = (d % 10);
            d = (d / 10);
            buf[10 - l] = m + 48;
            ++l;
        } while (d > 0);
        buf[11] = 0;
        LPTSTR stt = &buf[11 - l];
        int pos = 0;
        while (stt[pos])
        {
            des[pos++] = stt[pos];
        }
        if (bClose)
            des[pos++] = 0;
        return des + pos;
    }

    static LPCTSTR uitoa(unsigned int v)
    {
        static TCHAR buf[12] = { _T('\0') };
        unsigned int d = v;
        int m = v;
        int l = 0;
        do
        {
            m = (d % 10);
            d = (d / 10);
            buf[10 - l] = m + 48;
            ++l;
        } while (d > 0);
        buf[11] = 0;
        return &buf[11 - l];
    }

    static LPTSTR itoa_02d(int v, LPTSTR des, bool bClose = true)
    {
        des[1] = v % 10 + 48;
        v /= 10;
        des[0] = v + 48;
        if (bClose)
        {
            des[2] = 0;
            return des + 3;
        }
        else
            return des + 2;
    }

    static LPCTSTR itoa_02d(int v)
    {
        TCHAR des[3];
        des[1] = v % 10 + 48;
        v /= 10;
        des[0] = v + 48;
        des[2] = 0;
        return des;
    }

    static LPTSTR ulltoa(ULONG64 v, LPTSTR des, bool bClose = true)
    {
        static TCHAR buf[24] = { _T('\0') };
        ULONG64 d = v;
        int m;
        int l = 0;
        do
        {
            m = (d % 10);
            d = (d / 10);
            buf[22 - l] = m + 48;
            ++l;
        } while (d > 0);
        buf[23] = 0;
        LPTSTR stt = &buf[23 - l];
        int pos = 0;
        while (stt[pos])
        {
            des[pos++] = stt[pos];
        }
        if (bClose)
            des[pos++] = 0;
        return des + pos;
    }

    static LPCTSTR ulltoa(ULONG64 v)
    {
        static TCHAR buf[24] = { _T('\0') };
        ULONG64 d = v;
        int m;
        int l = 0;
        do
        {
            m = (d % 10);
            d = (d / 10);
            buf[22 - l] = m + 48;
            ++l;
        } while (d > 0);
        buf[23] = 0;
        return &buf[23 - l];
    }

    static LPTSTR itoax(unsigned char uc, LPTSTR des, bool bClose = true)
    {
        static TCHAR dig[] = { _T('0'), _T('1'), _T('2'), _T('3'), _T('4'), _T('5'), _T('6'), _T('7'),
            _T('8'), _T('9'), _T('A'), _T('B'), _T('C'), _T('D'), _T('E'), _T('F') };
        des[0] = dig[(uc >> 4) & 0x0f];
        des[1] = dig[uc & 0x0f];
        if (bClose)
        {
            des[2] = 0;
            return des + 3;
        }
        else
            return des + 2;
    }

    static LPCTSTR GetDistanceString(ULONG64 ull)
    {
        ull /= 100ull;
        static TCHAR dig[24];
        LPTSTR p;
        if (ull < 100ull)
        {
            p = CUtil::ulltoa(ull, dig, false);
            p[0] = _T('c');
            p[1] = _T('m');
            p[2] = 0;
        }
        else
        {
            p = CUtil::ulltoa(ull / 100ull, dig, false);
            p[0] = _T('.');
            p = CUtil::itoa_02d(int(ull % 100ull), p + 1, false);
            p[0] = _T('m');
            p[1] = 0;
        }
        return dig;
    }

    static void GetDistanceString(ULONG64 ull, LPTSTR dest)
    {
        ull /= 100ull;
        static TCHAR dig[24];
        LPTSTR p;
        if (ull < 100ull)
        {
            p = CUtil::ulltoa(ull, dig, false);
            p[0] = _T('c');
            p[1] = _T('m');
            p[2] = 0;
        }
        else
        {
            p = CUtil::ulltoa(ull / 100ull, dig, false);
            p[0] = _T('.');
            p = CUtil::itoa_02d(int(ull % 100ull), p + 1, false);
            p[0] = _T('m');
            p[1] = 0;
        }
#pragma warning(push)
#pragma warning(disable:4996)
        ::_tcscpy(dest, dig);
#pragma warning(pop)
    }
};
