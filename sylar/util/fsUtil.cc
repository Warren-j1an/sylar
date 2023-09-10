#include "fsUtil.h"

#include <climits>
#include <cstdio>
#include <cstdlib>
#include <dirent.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace sylar {
void FSUtil::ListAllFile(std::vector<std::string>& files, const std::string& path,
    const std::string& subfix) {
    if (access(path.c_str(), 0) != 0) {
        return;
    }
    DIR* dir = opendir(path.c_str());
    if (dir == nullptr) {
        return;
    }
    struct dirent* dp = nullptr;
    while ((dp = readdir(dir)) != nullptr) {
        if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..")) {
            continue;
        }
        ListAllFile(files, path + "/" + dp->d_name, subfix);
    }
    closedir(dir);
    std::string filename = dp->d_name;
    if (subfix.empty()) {
        files.push_back(path + "/" + filename);
    } else {
        if ((filename.size() > subfix.size()) &&
            (filename.substr(filename.size() - subfix.size()) == subfix)) {
            files.push_back(path + "/" + filename);
        }
    }
}

static int __lstat(const char* file, struct stat* st = nullptr) {
    struct stat lst;
    int ret = lstat(file, &lst);
    if (st != nullptr) {
        *st = lst;
    }
    return ret;
}

static int __mkdir(const char* dirname) {
    if (access(dirname, F_OK) == 0) {
        return 0;
    }
    return mkdir(dirname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}

bool FSUtil::Mkdir(const std::string& dirname) {
    if (__lstat(dirname.c_str()) == 0) {
        return true;
    }
    size_t pos = dirname.find("/", 1);
    while (pos != std::string::npos) {
        std::string path = dirname.substr(0, pos);
        if (__mkdir(path.c_str()) != 0) {
            return false;
        }
        pos = path.find("/", pos + 1);
    }
    if (__mkdir(dirname.c_str()) != 0) {
        return false;
    }
    return true;
}

bool FSUtil::IsRunningPidfile(const std::string& pidfile) {
    if (__lstat(pidfile.c_str()) != 0) {
        return false;
    }
    std::ifstream ifs(pidfile);
    std::string line;
    if (!ifs || !std::getline(ifs, line)) {
        return false;
    }
    if (line.empty()) {
        return false;
    }
    pid_t pid = std::stoi(line);
    if (pid <= 1) {
        return false;
    }
    if (kill(pid, 0) != 0) {
        return false;
    }
    return true;
}

bool FSUtil::Rm(const std::string& path) {
    struct stat st;
    if (__lstat(path.c_str(), &st)) {
        return true;
    }
    if (!(st.st_mode & S_IFDIR)) {
        return Unlink(path);
    }
    DIR* dir = opendir(path.c_str());
    if (dir == nullptr) {
        return false;
    }
    bool ret = true;
    struct dirent* dp = nullptr;
    while ((dp = readdir(dir)) != nullptr) {
        if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, "..")) {
            continue;
        }
        ret = Rm(path + "/" + dp->d_name);
    }
    closedir(dir);
    if (::rmdir(path.c_str())) {
        ret = false;
    }
    return ret;
}

bool FSUtil::Mv(const std::string& from, const std::string& to) {
    if (!Rm(to)) {
        return false;
    }
    return rename(from.c_str(), to.c_str()) == 0;
}

bool FSUtil::RealPath(const std::string path, std::string rpath) {
    if (__lstat(path.c_str())) {
        return false;
    }
    char* ptr = realpath(path.c_str(), nullptr);
    if (ptr == nullptr) {
        return false;
    }
    std::string(ptr).swap(rpath);
    free(ptr);
    return true;
}

bool FSUtil::Symlink(const std::string& from, const std::string& to) {
    if (!Rm(to)) {
        return false;
    }
    return symlink(from.c_str(), to.c_str()) == 0;
}

bool FSUtil::Unlink(const std::string& filename, bool exist) {
    if (!exist && __lstat(filename.c_str())) {
        return true;
    }
    return unlink(filename.c_str()) == 0;
}

std::string FSUtil::Dirname(const std::string& filename) {
    if (filename.empty()) {
        return ".";
    }
    size_t pos = filename.rfind("/");
    if (pos == 0) {
        return "/";
    } else if (pos == std::string::npos) {
        return ".";
    } else {
        return filename.substr(0, pos);
    }
}

std::string FSUtil::Basename(const std::string& filename) {
    if (filename.empty()) {
        return filename;
    }
    size_t pos = filename.rfind("/");
    if (pos == std::string::npos) {
        return filename;
    } else {
        return filename.substr(pos + 1);
    }
}

bool FSUtil::OpenForRead(std::ifstream& ifs, const std::string& filename,
        std::ios_base::openmode mode) {
    ifs.open(filename.c_str(), mode);
    return ifs.is_open();
}

bool FSUtil::OpenForWrite(std::ofstream& ofs, const std::string& filename,
        std::ios_base::openmode mode) {
    ofs.open(filename.c_str(), mode);
    if (!ofs.is_open()) {
        std::string dir = Dirname(filename);
        Mkdir(dir);
        ofs.open(filename.c_str(), mode);
    }
    return ofs.is_open();
}
}