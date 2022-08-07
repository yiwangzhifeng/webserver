#pragma once
#include<string>
#include<memory>
#include"noncopyable.h"
#include"FileUtil.h"
#include"MutexLock.h"
class LogFile:noncopyable{
    public:
        LogFile(const std::string &basename,int flushEveryN=1024);
        ~LogFile();
        void append(const char *logline,int len);
        void flush();
        bool rollFile();
    private:
        void append_unlocked(const char *logline,int len);
        const std::string basename_;
        const int flushEveryN_;
        int count_;
        std::unique_ptr<MutexLock> mutex_;
        std::unique_ptr<AppendFile> file_;
};