#pragma once

#include <string>
#include <list>
#include <mutex>

struct Result
{
    uint64_t row;
    uint64_t column;
    std::string match;
};

class MtFind
{
public:
    MtFind(const std::string& iFileName, const std::string& iMask) noexcept;
    void Read();
    void Print();
    
private:
    void FindMatch(const std::string& str, uint64_t row);
    
    std::mutex _mutex;
    std::string _fileName;
    std::string _mask;
    std::list<Result> _results;
};
