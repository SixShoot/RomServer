#ifndef SM_DEBUG_SETTING_H
#define SM_DEBUG_SETTING_H

#include <windows.h>    // OutputDebugString(), timeGetTime()
#include <tchar.h>      // _T()

//////////////////////////////////////////////////////////////////////////

// Assert for Debug
#if _DEBUG
    #include <sstream>  // std::ostringstream
    #include <assert.h> // assert()

    #ifndef SM_ASSERT
    #define SM_ASSERT(chk, msg)\
    {\
        if (chk)\
        {\
            std::ostringstream oss;\
            oss << "Assert! [ " << #chk << " ]\n";\
            oss << " because " << msg << "\n";\
            oss << " on line " << __LINE__ << "\n";\
            oss << " in file " << __FILE__ << "\n";\
            SM_PRINT_0(oss.str().c_str());\
            SM_DEBUG_0(oss.str().c_str());\
        }\
        assert(!chk);\
    }
    #endif
#else
    #ifndef SM_ASSERT
    #define SM_ASSERT(chk, str)
    #endif
#endif

// Add for Release Object
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)          if (p) { delete p; p = NULL; }
#endif
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)         if (p) { p->Release(); delete p; p = NULL; }
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p)    if (p) { delete[] p; p = NULL; }
#endif

// Buffer Size
#define LOG_BUF_MAX 8192
#define SQL_BUF_MAX 4096
#define HUGE_BUF_MAX 2048
#define DEBUG_BUF_MAX 1024
#define LARGE_BUF_MAX 512
#define PATH_BUF_MAX 254
#define SMALL_BUF_MAX 128
#define NAME_BUF_MAX 64
#define TINY_BUF_MAX 32
#define IP_BUF_MAX 15 // 255.255.255.255

// return values
#ifndef OPEN_FILE_S_RETURN_NO_ERROR
#define OPEN_FILE_S_RETURN_NO_ERROR 0
#endif
#ifndef GM_TIME_S_RETURN_NO_ERROR
#define GM_TIME_S_RETURN_NO_ERROR 0
#endif

#ifndef SM_PRINT_USE_MEMSET
#define SM_PRINT_USE_MEMSET 0
#endif
#if SM_PRINT_USE_MEMSET
#define SM_MEMSET(bf) memset(bf, 0, sizeof(bf));
#else
#define SM_MEMSET(bf)
#endif

// Print for Converted Byte Size (KB, MB, GB)
#ifndef SM_PRINT_CONVERT_BYTE_SIZE
#define SM_PRINT_CONVERT_BYTE_SIZE(m, s, v)\
{\
    double tempNumber = v * 1.0;\
    if (tempNumber < 1024.0) {\
        _sntprintf(m, s, _T("%.0f Byte(s)"), tempNumber);\
    }\
    else if (tempNumber < 1048576.0) {\
        _sntprintf(m, s, _T("%.2f KiB"), tempNumber / 1024.0);\
    }\
    else if (tempNumber < 1073741824.0) {\
        _sntprintf(m, s, _T("%.2f MiB"), tempNumber / 1048576.0);\
    }\
    else {\
        _sntprintf(m, s, _T("%.2f GiB"), tempNumber / 1073741824.0);\
    }\
}
#endif
#ifndef SM_PRINT_CONVERT_BYTE_SIZE_PER_SECEND
#define SM_PRINT_CONVERT_BYTE_SIZE_PER_SECEND(m, s, v)\
{\
    double tempNumber = v * 1.0;\
    if (tempNumber < 1024.0) {\
        _sntprintf(m, s, _T("%.0f Byte(s)/s"), tempNumber);\
    }\
    else if (tempNumber < 1048576.0) {\
        _sntprintf(m, s, _T("%.2f KiB/s"), tempNumber / 1024.0);\
    }\
    else if (tempNumber < 1073741824.0) {\
        _sntprintf(m, s, _T("%.2f MiB/s"), tempNumber / 1048576.0);\
    }\
    else {\
        _sntprintf(m, s, _T("%.2f GiB/s"), tempNumber / 1073741824.0);\
    }\
}
#endif

// Add for Unicode String
#ifdef _UNICODE
    #ifndef tstring
    #define tstring std::wstring
    #endif
#else
    #ifndef tstring
    #define tstring std::string
    #endif
#endif

// Add for Debug Message
#ifdef _CONSOLE
    #ifdef _DEBUG
        #if _CRT_SECURE_NO_WARNINGS
            #define SM_PRINT(m, ...) _tprintf(m, __VA_ARGS__);
            #define SM_PRINT_0(m)                               _tprintf(m);
            #define SM_PRINT_1(m, a)                            _tprintf(m, a);
            #define SM_PRINT_2(m, a, b)                         _tprintf(m, a, b);
            #define SM_PRINT_3(m, a, b, c)                      _tprintf(m, a, b, c);
            #define SM_PRINT_4(m, a, b, c, d)                   _tprintf(m, a, b, c, d);
            #define SM_PRINT_5(m, a, b, c, d, e)                _tprintf(m, a, b, c, d, e);
            #define SM_PRINT_6(m, a, b, c, d, e, f)             _tprintf(m, a, b, c, d, e, f);
            #define SM_PRINT_7(m, a, b, c, d, e, f, g)          _tprintf(m, a, b, c, d, e, f, g);
            #define SM_PRINT_8(m, a, b, c, d, e, f, g, h)       _tprintf(m, a, b, c, d, e, f, g, h);
            #define SM_PRINT_9(m, a, b, c, d, e, f, g, h, i)    _tprintf(m, a, b, c, d, e, f, g, h, i);
        #else
            #define SM_PRINT(m, ...) _tprintf_s(m, __VA_ARGS__);
            #define SM_PRINT_0(m)                               _tprintf_s(m);
            #define SM_PRINT_1(m, a)                            _tprintf_s(m, a);
            #define SM_PRINT_2(m, a, b)                         _tprintf_s(m, a, b);
            #define SM_PRINT_3(m, a, b, c)                      _tprintf_s(m, a, b, c);
            #define SM_PRINT_4(m, a, b, c, d)                   _tprintf_s(m, a, b, c, d);
            #define SM_PRINT_5(m, a, b, c, d, e)                _tprintf_s(m, a, b, c, d, e);
            #define SM_PRINT_6(m, a, b, c, d, e, f)             _tprintf_s(m, a, b, c, d, e, f);
            #define SM_PRINT_7(m, a, b, c, d, e, f, g)          _tprintf_s(m, a, b, c, d, e, f, g);
            #define SM_PRINT_8(m, a, b, c, d, e, f, g, h)       _tprintf_s(m, a, b, c, d, e, f, g, h);
            #define SM_PRINT_9(m, a, b, c, d, e, f, g, h, i)    _tprintf_s(m, a, b, c, d, e, f, g, h, i);
        #endif

    #else
        #define SM_PRINT(m, ...)
        #define SM_PRINT_0(m)
        #define SM_PRINT_1(m, a)
        #define SM_PRINT_2(m, a, b)
        #define SM_PRINT_3(m, a, b, c)
        #define SM_PRINT_4(m, a, b, c, d)
        #define SM_PRINT_5(m, a, b, c, d, e)
        #define SM_PRINT_6(m, a, b, c, d, e, f)
        #define SM_PRINT_7(m, a, b, c, d, e, f, g)
        #define SM_PRINT_8(m, a, b, c, d, e, f, g, h)
        #define SM_PRINT_9(m, a, b, c, d, e, f, g, h, i)
    #endif // _DEBUG
#else
    #define SM_PRINT(m, ...)
    #define SM_PRINT_0(m)
    #define SM_PRINT_1(m, a)
    #define SM_PRINT_2(m, a, b)
    #define SM_PRINT_3(m, a, b, c)
    #define SM_PRINT_4(m, a, b, c, d)
    #define SM_PRINT_5(m, a, b, c, d, e)
    #define SM_PRINT_6(m, a, b, c, d, e, f)
    #define SM_PRINT_7(m, a, b, c, d, e, f, g)
    #define SM_PRINT_8(m, a, b, c, d, e, f, g, h)
    #define SM_PRINT_9(m, a, b, c, d, e, f, g, h, i)
#endif // _CONSOLE

#ifdef _DEBUG
    //#define SM_DEBUG_BUFFER_SIZE 4096
    #define SM_DEBUG_BUFFER_SIZE 512
    #if _CRT_SECURE_NO_WARNINGS
        #define SM_TRACE(m, ...) { TCHAR s[SM_DEBUG_BUFFER_SIZE]; SM_MEMSET(s) _sntprintf(s, SM_DEBUG_BUFFER_SIZE-1, m, __VA_ARGS__);                 OutputDebugString(s); }
        #define SM_DEBUG_0(m)                               OutputDebugString(m);
        #define SM_DEBUG_1(m, a)                            { TCHAR s[SM_DEBUG_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf(s, SM_DEBUG_BUFFER_SIZE, m, a);                           OutputDebugString(s); }
        #define SM_DEBUG_2(m, a, b)                         { TCHAR s[SM_DEBUG_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf(s, SM_DEBUG_BUFFER_SIZE, m, a, b);                        OutputDebugString(s); }
        #define SM_DEBUG_3(m, a, b, c)                      { TCHAR s[SM_DEBUG_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf(s, SM_DEBUG_BUFFER_SIZE, m, a, b, c);                     OutputDebugString(s); }
        #define SM_DEBUG_4(m, a, b, c, d)                   { TCHAR s[SM_DEBUG_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf(s, SM_DEBUG_BUFFER_SIZE, m, a, b, c, d);                  OutputDebugString(s); }
        #define SM_DEBUG_5(m, a, b, c, d, e)                { TCHAR s[SM_DEBUG_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf(s, SM_DEBUG_BUFFER_SIZE, m, a, b, c, d, e);               OutputDebugString(s); }
        #define SM_DEBUG_6(m, a, b, c, d, e, f)             { TCHAR s[SM_DEBUG_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf(s, SM_DEBUG_BUFFER_SIZE, m, a, b, c, d, e, f);            OutputDebugString(s); }
        #define SM_DEBUG_7(m, a, b, c, d, e, f, g)          { TCHAR s[SM_DEBUG_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf(s, SM_DEBUG_BUFFER_SIZE, m, a, b, c, d, e, f, g);         OutputDebugString(s); }
        #define SM_DEBUG_8(m, a, b, c, d, e, f, g, h)       { TCHAR s[SM_DEBUG_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf(s, SM_DEBUG_BUFFER_SIZE, m, a, b, c, d, e, f, g, h);      OutputDebugString(s); }
        #define SM_DEBUG_9(m, a, b, c, d, e, f, g, h, i)    { TCHAR s[SM_DEBUG_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf(s, SM_DEBUG_BUFFER_SIZE, m, a, b, c, d, e, f, g, h, i);   OutputDebugString(s); }
    #else
        #define SM_TRACE(m, ...) { TCHAR s[SM_DEBUG_BUFFER_SIZE]; SM_MEMSET(s) _sntprintf_s(s, SM_DEBUG_BUFFER_SIZE-1, m, __VA_ARGS__);                 OutputDebugString(s); }
        #define SM_DEBUG_0(m)                               OutputDebugString(m);
        #define SM_DEBUG_1(m, a)                            { TCHAR s[SM_DEBUG_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf_s(s, SM_DEBUG_BUFFER_SIZE, m, a);                            OutputDebugString(s); }
        #define SM_DEBUG_2(m, a, b)                         { TCHAR s[SM_DEBUG_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf_s(s, SM_DEBUG_BUFFER_SIZE, m, a, b);                         OutputDebugString(s); }
        #define SM_DEBUG_3(m, a, b, c)                      { TCHAR s[SM_DEBUG_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf_s(s, SM_DEBUG_BUFFER_SIZE, m, a, b, c);                      OutputDebugString(s); }
        #define SM_DEBUG_4(m, a, b, c, d)                   { TCHAR s[SM_DEBUG_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf_s(s, SM_DEBUG_BUFFER_SIZE, m, a, b, c, d);                   OutputDebugString(s); }
        #define SM_DEBUG_5(m, a, b, c, d, e)                { TCHAR s[SM_DEBUG_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf_s(s, SM_DEBUG_BUFFER_SIZE, m, a, b, c, d, e);                OutputDebugString(s); }
        #define SM_DEBUG_6(m, a, b, c, d, e, f)             { TCHAR s[SM_DEBUG_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf_s(s, SM_DEBUG_BUFFER_SIZE, m, a, b, c, d, e, f);             OutputDebugString(s); }
        #define SM_DEBUG_7(m, a, b, c, d, e, f, g)          { TCHAR s[SM_DEBUG_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf_s(s, SM_DEBUG_BUFFER_SIZE, m, a, b, c, d, e, f, g);          OutputDebugString(s); }
        #define SM_DEBUG_8(m, a, b, c, d, e, f, g, h)       { TCHAR s[SM_DEBUG_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf_s(s, SM_DEBUG_BUFFER_SIZE, m, a, b, c, d, e, f, g, h);       OutputDebugString(s); }
        #define SM_DEBUG_9(m, a, b, c, d, e, f, g, h, i)    { TCHAR s[SM_DEBUG_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf_s(s, SM_DEBUG_BUFFER_SIZE, m, a, b, c, d, e, f, g, h, i);    OutputDebugString(s); }
    #endif // _CRT_SECURE_NO_WARNINGS
#else
    #define SM_DEBUG_SIZE 0
    #define SM_TRACE(m, ...)
    #define SM_DEBUG_0(m)
    #define SM_DEBUG_1(m, a)
    #define SM_DEBUG_2(m, a, b)
    #define SM_DEBUG_3(m, a, b, c)
    #define SM_DEBUG_4(m, a, b, c, d)
    #define SM_DEBUG_5(m, a, b, c, d, e)
    #define SM_DEBUG_6(m, a, b, c, d, e, f)
    #define SM_DEBUG_7(m, a, b, c, d, e, f, g)
    #define SM_DEBUG_8(m, a, b, c, d, e, f, g, h)
    #define SM_DEBUG_9(m, a, b, c, d, e, f, g, h, i)
#endif // _DEBUG

#if 1
    //#define LOG_FILE_BUFFER_SIZE 4096
    #define LOG_FILE_BUFFER_SIZE 512
    #if _CRT_SECURE_NO_WARNINGS
        #define LOG_FILE(p, m, ...) if (p) { TCHAR s[LOG_FILE_BUFFER_SIZE]; SM_MEMSET(s) _sntprintf(s, LOG_FILE_BUFFER_SIZE-1, m, __VA_ARGS__); p->f_LogWithTime(s); }
        #define LOG_FILE_0(m, p)                            if (p) { p->f_LogWithTime(m); }
        #define LOG_FILE_1(m, a, p)                         if (p) { TCHAR s[LOG_FILE_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf(s, LOG_FILE_BUFFER_SIZE, m, a);                         p->f_LogWithTime(s); }
        #define LOG_FILE_2(m, a, b, p)                      if (p) { TCHAR s[LOG_FILE_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf(s, LOG_FILE_BUFFER_SIZE, m, a, b);                      p->f_LogWithTime(s); }
        #define LOG_FILE_3(m, a, b, c, p)                   if (p) { TCHAR s[LOG_FILE_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf(s, LOG_FILE_BUFFER_SIZE, m, a, b, c);                   p->f_LogWithTime(s); }
        #define LOG_FILE_4(m, a, b, c, d, p)                if (p) { TCHAR s[LOG_FILE_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf(s, LOG_FILE_BUFFER_SIZE, m, a, b, c, d);                p->f_LogWithTime(s); }
        #define LOG_FILE_5(m, a, b, c, d, e, p)             if (p) { TCHAR s[LOG_FILE_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf(s, LOG_FILE_BUFFER_SIZE, m, a, b, c, d, e);             p->f_LogWithTime(s); }
        #define LOG_FILE_6(m, a, b, c, d, e, f, p)          if (p) { TCHAR s[LOG_FILE_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf(s, LOG_FILE_BUFFER_SIZE, m, a, b, c, d, e, f);          p->f_LogWithTime(s); }
        #define LOG_FILE_7(m, a, b, c, d, e, f, g, p)       if (p) { TCHAR s[LOG_FILE_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf(s, LOG_FILE_BUFFER_SIZE, m, a, b, c, d, e, f, g);       p->f_LogWithTime(s); }
        #define LOG_FILE_8(m, a, b, c, d, e, f, g, h, p)    if (p) { TCHAR s[LOG_FILE_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf(s, LOG_FILE_BUFFER_SIZE, m, a, b, c, d, e, f, g, h);    p->f_LogWithTime(s); }
        #define LOG_FILE_9(m, a, b, c, d, e, f, g, h, i, p) if (p) { TCHAR s[LOG_FILE_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf(s, LOG_FILE_BUFFER_SIZE, m, a, b, c, d, e, f, g, h, i); p->f_LogWithTime(s); }
    #else
        #define LOG_FILE(p, m, ...) if (p) { TCHAR s[LOG_FILE_BUFFER_SIZE]; SM_MEMSET(s) _sntprintf_s(s, LOG_FILE_BUFFER_SIZE-1, m, __VA_ARGS__); p->f_LogWithTime(s); }
        #define LOG_FILE_0(m, p)                            if (p) { p->f_LogWithTime(m); }
        #define LOG_FILE_1(m, a, p)                         if (p) { TCHAR s[LOG_FILE_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf_s(s, LOG_FILE_BUFFER_SIZE, m, a);                         p->f_LogWithTime(s); }
        #define LOG_FILE_2(m, a, b, p)                      if (p) { TCHAR s[LOG_FILE_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf_s(s, LOG_FILE_BUFFER_SIZE, m, a, b);                      p->f_LogWithTime(s); }
        #define LOG_FILE_3(m, a, b, c, p)                   if (p) { TCHAR s[LOG_FILE_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf_s(s, LOG_FILE_BUFFER_SIZE, m, a, b, c);                   p->f_LogWithTime(s); }
        #define LOG_FILE_4(m, a, b, c, d, p)                if (p) { TCHAR s[LOG_FILE_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf_s(s, LOG_FILE_BUFFER_SIZE, m, a, b, c, d);                p->f_LogWithTime(s); }
        #define LOG_FILE_5(m, a, b, c, d, e, p)             if (p) { TCHAR s[LOG_FILE_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf_s(s, LOG_FILE_BUFFER_SIZE, m, a, b, c, d, e);             p->f_LogWithTime(s); }
        #define LOG_FILE_6(m, a, b, c, d, e, f, p)          if (p) { TCHAR s[LOG_FILE_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf_s(s, LOG_FILE_BUFFER_SIZE, m, a, b, c, d, e, f);          p->f_LogWithTime(s); }
        #define LOG_FILE_7(m, a, b, c, d, e, f, g, p)       if (p) { TCHAR s[LOG_FILE_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf_s(s, LOG_FILE_BUFFER_SIZE, m, a, b, c, d, e, f, g);       p->f_LogWithTime(s); }
        #define LOG_FILE_8(m, a, b, c, d, e, f, g, h, p)    if (p) { TCHAR s[LOG_FILE_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf_s(s, LOG_FILE_BUFFER_SIZE, m, a, b, c, d, e, f, g, h);    p->f_LogWithTime(s); }
        #define LOG_FILE_9(m, a, b, c, d, e, f, g, h, i, p) if (p) { TCHAR s[LOG_FILE_BUFFER_SIZE+1]; SM_MEMSET(s) _sntprintf_s(s, LOG_FILE_BUFFER_SIZE, m, a, b, c, d, e, f, g, h, i); p->f_LogWithTime(s); }
    #endif // _CRT_SECURE_NO_WARNINGS
#else
    #define LOG_FILE_BUFFER_SIZE 0
    #define LOG_FILE(p, m, ...)
    #define LOG_FILE_0(m, p)
    #define LOG_FILE_1(m, a, p)
    #define LOG_FILE_2(m, a, b, p)
    #define LOG_FILE_3(m, a, b, c, p)
    #define LOG_FILE_4(m, a, b, c, d, p)
    #define LOG_FILE_5(m, a, b, c, d, e, p)
    #define LOG_FILE_6(m, a, b, c, d, e, f, p)
    #define LOG_FILE_7(m, a, b, c, d, e, f, g, p)
    #define LOG_FILE_8(m, a, b, c, d, e, f, g, h, p)
    #define LOG_FILE_9(m, a, b, c, d, e, f, g, h, i, p)
#endif

//////////////////////////////////////////////////////////////////////////

#endif // SM_DEBUG_SETTING_H
