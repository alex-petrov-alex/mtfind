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
        
        MtFind mtFind(argv[1], argv[2]);
        mtFind.Read();
        mtFind.Print();
    }
    catch (std::runtime_error& exception)
    {
        std::cerr << exception.what() << std::endl;
    }
    catch (std::exception& exception)
    {
        std::cerr << exception.what() << std::endl;
    }
    catch(...)
    {
        std::cerr << "Неизвестная ошибка!" << std::endl;
    }
}
