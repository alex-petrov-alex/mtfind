#pragma once

#include <list>
#include <string>
#include <memory>


class MtFind
{
    using Data = std::list<std::pair<std::string, uint64_t>>;
    
public:
    MtFind(const std::string& iMask, const Data& iData);
    ~MtFind();
    void CalculateResults();
    void PrintResults();
    
private:
    void SortResults();
    
    class KMP;
    std::unique_ptr<KMP> _kmp;
    class Result;
    std::list<Result> _results;
};
