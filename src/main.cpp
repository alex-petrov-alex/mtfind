#include "File.h"
#include "MtFind.h"

#include <iostream>
#include <exception>


int main(int argc, char* argv[]) 
{
    try
    {
        switch (argc)
        {
            case 0:
            case 1: throw std::runtime_error("Отсутствует файл");
            case 2: throw std::runtime_error("Отсутствует файл и маска");
        }
        File file(argv[1]);
        file.Read();
        
        MtFind mtFind(argv[2], file.GetData());
        mtFind.CalculateResults();
        mtFind.PrintResults();
    }
    catch (const std::exception& exception)
    {
        std::cerr << exception.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Неизвестная ошибка!" << std::endl;
    }
}
