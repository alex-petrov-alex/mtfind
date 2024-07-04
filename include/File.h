#pragma once

#include <list>
#include <string>


class File
{
    using Data = std::list<std::pair<std::string, uint64_t>>;
public:
    File(const std::string& iFileName);
    void Read();
    Data GetData() const noexcept;
    
private:
    std::string _fileName;
    Data _data;
};
