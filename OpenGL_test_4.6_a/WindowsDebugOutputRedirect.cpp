#include "WindowsDebugOutputRedirect.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include <iosfwd>
#include <Windows.h>

using std::stringbuf;

class dbg_stream_for_cout
    : public stringbuf
{
public:
    ~dbg_stream_for_cout() { sync(); }
    int sync()
    {
        ::OutputDebugStringA(str().c_str());
        str(std::string()); // Clear the string buffer
        return 0;
    }
};
dbg_stream_for_cout g_DebugStreamFor_cout;
dbg_stream_for_cout g_DebugStreamFor_cerr;

void WindowsDebugOutputRedirect::redirect()
{
    std::cout.rdbuf(&g_DebugStreamFor_cout);
//    std::cerr.rdbuf(&g_DebugStreamFor_cerr);
}
