#include "MtFind.h"
#include "ThreadPool.h"

#include <fstream>
#include <iostream>


MtFind::MtFind(const std::string& iFileName, const std::string& iMask) noexcept:
_fileName(iFileName),
_mask(iMask)
{
}

void MtFind::FindMatch(const std::string& str, uint64_t row)
{
    for (uint64_t col = 0, COL = str.length() - _mask.length() - 1; col < COL; ++col)
    {
        bool match = true;
        for (size_t m = 0, M = _mask.length(); m < M; ++m)
        {
            if (_mask[m] == '?')
            {
                continue;
            }
            else if (_mask[m] != str[m + col])
            {
                match = false;
                break;
            }
        }
        
        if (match)
        {
            {
                std::unique_lock lock(_mutex);
                _results.push_back({row + 1u, col + 1u, str.substr(col, _mask.length())});
            }
            col += _mask.length() - 1u;
        }
    }
}

void MtFind::Read()
{
    std::ifstream file(_fileName);
    if (file.is_open())
    {
        if (file.peek() == EOF)
            throw std::runtime_error("Пустой файл >> " + _fileName);
        
        uint64_t row = 0;
        ThreadPool pool;
        
        std::string line;
        while (getline(file, line))
            pool.AddTask(&MtFind::FindMatch, this, line, row++);
    }
    else
    {
        throw std::runtime_error("Невозможно открыть файл >> " + _fileName);
    }
    
    file.close();
}

void MtFind::Print()
{
    std::cout << _results.size() << std::endl;
    _results.sort([](const Result& lhs, const Result& rhs)
    {
        return lhs.row < rhs.row || (lhs.row == rhs.row && lhs.column < rhs.column);
    });

    for (const auto& result : _results)
    {
        std::cout << result.row << " " << result.column << " " << result.match << std::endl;
    }
}
