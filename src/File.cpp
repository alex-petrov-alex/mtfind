#include "File.h"

#include <fstream>
#include <string>


File::File(const std::string& iFileName):
_fileName(iFileName)
{
}
 

void File::Read()
{
    std::ifstream file(_fileName);
    if (file.is_open())
    {
        if (file.peek() == EOF)
            throw std::runtime_error("Пустой файл >> " + _fileName);
        
        std::string line;
        uint64_t row = 0;
        while (getline(file, line))
            _data.emplace_back(line, row++);
    }
    else
    {
        throw std::runtime_error("Невозможно открыть файл >> " + _fileName);
    }
    
    file.close();
}

File::Data File::GetData() const noexcept
{
    return _data;
}
