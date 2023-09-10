#pragma once

#include <fstream>
#include <cstring>
#include <vector>

namespace sylar {
class FSUtil {
public:
    static void ListAllFile(std::vector<std::string>& files, const std::string& path,
        const std::string& subfix);
    static bool Mkdir(const std::string& dirname);
    static bool IsRunningPidfile(const std::string& pidfile);
    static bool Rm(const std::string& path);
    static bool Mv(const std::string& from, const std::string& to);
    static bool RealPath(const std::string path, std::string rpath);
    static bool Symlink(const std::string& from, const std::string& to);
    static bool Unlink(const std::string& filename, bool exist = false);
    static std::string Dirname(const std::string& filename);
    static std::string Basename(const std::string& filename);
    static bool OpenForRead(std::ifstream& ifs, const std::string& filename,
        std::ios_base::openmode mode);
    static bool OpenForWrite(std::ofstream& ofs, const std::string& filename,
        std::ios_base::openmode mode);
};
}