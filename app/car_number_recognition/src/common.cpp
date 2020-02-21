#include "src/common.h"

namespace Encode {


std::string UTF8ToGBK(const char* str)
{
    std::string strUTF8 = std::string(str);
    if (strUTF8.empty()){
        return FALSE;
    }

    int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
    //unsigned short * wszGBK = new unsigned short[len + 1];
    wchar_t * wszGBK = new wchar_t[len + 1];
    memset(wszGBK, 0, len * 2 + 2);
    MultiByteToWideChar(CP_UTF8, 0, (LPCCH)strUTF8.c_str(), -1, wszGBK, len);

    len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
    char *szGBK = new char[len + 1];
    memset(szGBK, 0, len + 1);
    WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
    std::string strTemp(szGBK);
    delete[]szGBK;
    delete[]wszGBK;

    return strTemp;
}

std::string GBKToUTF8(const char* strGBK)
{
    int len = MultiByteToWideChar(CP_ACP, 0, strGBK, -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len+1];
    memset(wstr, 0, len+1);
    MultiByteToWideChar(CP_ACP, 0, strGBK, -1, wstr, len);
    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* str = new char[len+1];
    memset(str, 0, len+1);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
    std::string strTemp = str;
    if(wstr) delete[] wstr;
    if(str) delete[] str;
    return strTemp;
}


}

static fn_outInfo_t fn_outInfo = 0;


void Common::setFnOutInfo(fn_outInfo_t fn)
{
    fn_outInfo = fn;
}

void Common::outInfo(const std::string &sInfo)
{
    if (fn_outInfo) fn_outInfo(sInfo.c_str());
}

void Common::outInfo(const char *sInfo)
{
    if (fn_outInfo) fn_outInfo(sInfo);
}
