#include "MtFind.h"
#include "Queue.h"
#include "ThreadPool.h"

#include <iostream>
#include <execution>


class MtFind::Result
{
public:
    Result(uint64_t row, uint64_t column, std::string&& match) :
    _row(row),
    _column(column),
    _match(std::move(match))
    {
        
    }
    
    bool operator<(const Result& other) const noexcept
    {
        return _row < other._row || (_row == other._row && _column < other._column);
    }
    
    friend std::ostream& operator<<(std::ostream &os, const Result& other)
    {
        return std::cout << other._row << " " << other._column << " " << other._match;
    }
    
private:
    uint64_t _row;
    uint64_t _column;
    std::string _match;
};

/*
 Алгоритм Кнута — Морриса — Пратта (KMP)
 Идея алгоритма KMP: всякий раз, когда обнаруживается несовпадение символов (после нескольких совпадений), то уже известны некоторые символы в тексте следующего окна. Воспользовавшись этой информации, можно избежать лишних сопоставлений символов, которые в любом случае совпадут.
 
 Алгоритм KMP предварительно обрабатывает mask и создает вспомогательный Longest prefix-suffix (lps) такого же размера как и маска, который используется для пропуска символов при сопоставлении.
 Имя lps указывает на самый длинный правильный префикс, который также является суффиксом. Правильный префикс — это префикс с целой строкой, которая не допускается. Например, префиксами «ABC» являются «», «A», «AB» и «ABC». Правильными префиксами являются «», «A» и «AB». Суффиксами строки являются «», «C», «BC» и «ABC».
 */
class MtFind::KMP
{
    using Data = std::list<std::pair<std::string, uint64_t>>;
    using ThreadSafeQueue = Queue<std::pair<std::exception_ptr, std::thread::id>>;
    
public:
    KMP(const std::string& mask, const Data& data):
    _mask(mask),
    _data(data)
    {
        
    }
    
    // Time: O(n+m), где n — длина строки, а m — длина подстроки, Memory: O(m)
    void GetResults(std::list<Result>& results)
    {
        CalculateLPS();
        
        ThreadSafeQueue exception_queue;
        ThreadPool pool;
        
        std::for_each(std::execution::par, _data.begin(), _data.end(), [this, &pool, &exception_queue, &results](auto&& line)
        {
            pool.AddTask(&KMP::Search, this, std::cref(line.first), line.second, std::ref(results), std::ref(exception_queue));
        });
        
        /// Обработка всех исключений, пойманных в потоках
        while (!exception_queue.Empty())
        {
            try
            {
                auto [exception, thread_id] = exception_queue.Front();
                exception_queue.Pop();
                std::cout << "Поток: " << thread_id << " ";
                std::rethrow_exception(exception);
            }
            catch (const std::exception& exception)
            {
                std::cout << "Исключение: " << exception.what() << std::endl;
            }
        }
    }
    
private:
    
    /* Вычисление самого длинного значения префикса-суффикса (lps).
     Для подстроки «AAAA» lps = [0, 1, 2, 3]
     Для подстроки «ABCDE» lps = [0, 0, 0, 0, 0]
     Для подстркои «AABAACAABAA» lps = [0, 1, 0, 1, 2, 0, 1, 2, 3, 4, 5]
     Для подстроки «AAACAAAAAC» lps = [0, 1, 2, 0, 1, 2, 3, 3, 3, 4]
     Для подстроки «AAABAAA» lps = [0, 1, 2, 0, 1, 2, 3]
     */
    void CalculateLPS()
    {
        const uint64_t mask_size = _mask.size();
        _lps.resize(mask_size);
     
        uint64_t i = 1;
        uint64_t len = 0;
        
        while (i < mask_size)
        {
            if (_mask[i] != '?' && _mask[i] == _mask[len])
            {
                ++len;
                _lps[i] = len;
                ++i;
            }
            else
            {
                if (len != 0)
                {
                    len = _lps[len - 1u];
                }
                else
                {
                    _lps[i] = 0;
                    ++i;
                }
            }
        }
    }
    
    void Search(const std::string& str, uint64_t row, std::list<Result>& results, ThreadSafeQueue& exception_queue)
    {
        try
        {
            uint64_t s = 0; // индекс для str
            uint64_t m = 0; // индекс для mask
            
            uint64_t S = str.size();
            uint64_t M = _mask.size();
            
            while ((S - s) >= (M - m))
            {
                if (_mask[m] == '?' || _mask[m] == str[s])
                {
                    ++m;
                    ++s;
                }
         
                if (m == M)
                {
                    /// Deadlock не возможен, т.к. после обработки исключения запись результатов невозможна
                    {
                        std::unique_lock lock(_mutex);
                        results.emplace_back(row + 1u, s - m + 1u, str.substr(s - m, _mask.length()));
                    }
                    
                    m = _lps[m - 1];
                }
                else if ((s < S) && (_mask[m] != str[s]) && (_mask[m] != '?'))
                {
                    if (m != 0)
                        m = _lps[m - 1];
                    else
                        ++s;
                }
            }
        }
        catch (...)
        {
            /// Deadlock не возможен, т.к. после обработки исключения запись результатов невозможна
            exception_queue.Emplace(std::current_exception(), std::this_thread::get_id());
        }
    }
    
private:
    const std::string _mask;
    const Data _data;
    std::mutex _mutex;
    std::vector<uint64_t> _lps; // Longest prefix-suffix
};


MtFind::MtFind(const std::string& mask, const Data& data):
_kmp(std::make_unique<KMP>(mask, data))
{
    
}

MtFind::~MtFind()
{
    
}

void MtFind::GetResults()
{
    _kmp->GetResults(_results);
}

void MtFind::SortResults()
{
    _results.sort();
}

void MtFind::PrintResults()
{
    SortResults();
    std::cout << _results.size() << std::endl;
    for (const auto& result : _results)
        std::cout << result << std::endl;
}
