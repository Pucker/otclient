/*
 * Copyright (c) 2010-2013 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef WIN32

#include "platform.h"
#include <fstream>
#include <unistd.h>
#include <string.h>
#include <framework/stdext/stdext.h>

#include <sys/stat.h>

void Platform::processArgs(std::vector<std::string>& args)
{
    //nothing todo, linux args are already utf8 encoded
}

bool Platform::spawnProcess(const std::string& process, const std::vector<std::string>& args)
{
    struct stat sts;
    if(stat(process.c_str(), &sts) == -1 && errno == ENOENT)
        return false;

    pid_t pid = fork();
    if(pid == -1)
        return false;

    if(pid == 0) {
        char* cargs[args.size()+2];
        cargs[0] = (char*)process.c_str();
        for(uint i=1;i<=args.size();++i)
            cargs[i] = (char*)args[i-1].c_str();
        cargs[args.size()+1] = 0;

        if(execv(process.c_str(), cargs) == -1)
            _exit(EXIT_FAILURE);
    }

    return true;
}

int Platform::getProcessId()
{
    return getpid();
}

std::string Platform::getTempPath()
{
    return "/tmp/";
}

bool Platform::copyFile(std::string from, std::string to)
{
    return system(stdext::format("/bin/cp '%s' '%s'", from, to).c_str()) == 0;
}

void Platform::openUrl(std::string url)
{
    if(url.find("http://") == std::string::npos)
        url.insert(0, "http://");
    system(stdext::format("xdg-open %s", url).c_str());
}

std::string Platform::getCPUName()
{
    std::string line;
    std::ifstream in("/proc/cpuinfo");
    while(getline(in, line)) {
        auto strs = stdext::split(line, ":");
        std::string first = strs[0];
        std::string second = strs[1];
        stdext::trim(first);
        stdext::trim(second);
        if(strs.size() == 2 && first == "model name")
            return second;
    }
    return std::string();
}

double Platform::getTotalSystemMemory()
{
    std::string line;
    std::ifstream in("/proc/meminfo");
    while(getline(in, line)) {
        auto strs = stdext::split(line, ":");
        std::string first = strs[0];
        std::string second = strs[1];
        stdext::trim(first);
        stdext::trim(second);
        if(strs.size() == 2 && first == "MemTotal")
            return stdext::unsafe_cast<double>(second.substr(0, second.length() - 3)) * 1000.0;
    }
    return 0;
}

std::string Platform::getOSName()
{
    std::string line;
    std::ifstream in("/etc/issue");
    if(getline(in, line)) {
        std::size_t end = line.find('\\');
        std::string res = line.substr(0, end);
        stdext::trim(res);
        return res;
    }
    return std::string();
}


#endif
