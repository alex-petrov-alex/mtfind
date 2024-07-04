# Задача
Написать программу mtfind, производящую поиск подстроки в текстовом файле
по маске с использованием многопоточности. <br>
Маска - это строка, где "?" обозначает любой символ. <br>
Программа принимает в качестве параметров командной строки:
1. Имя текстового файла, в котором должен идти поиск (размер файла - до 1Гб).
2. Маску для поиска, в кавычках. Максимальная длина маски 1000 символов.

Вывод программы должен быть в следующем формате:
- На первой строке - количество найденных вхождений.
- Далее информация о каждом вхождении, каждое на отдельной строке, через пробел: номер строки, позиция в строке, само найденное вхождение.
- Порядок вывода найденных вхождений должен совпадать с их порядком в файле.
- Вся нумерация ведется начиная с 1 (делаем программу для обычных людей).

## Дополнения:
- В текстовом файле кодировка только 7-bit ASCII.
- Поиск с учетом регистра.
- Каждое вхождение может быть только на одной строке. Маска не может содержать символа перевода строки.
- Найденные вхождения не должны пересекаться. Если в файле есть пересекающиеся вхождения то нужно вывести одно из них (любое).
- Пробелы и разделители участвуют в поиске наравне с другими символами.
- Можно использовать STL, Boost, возможности С++1x.
- Многопоточность нужно использовать обязательно. Однопоточные решения засчитываться не будут.
- Серьезным плюсом будет разделение работы между потоками равномерно вне зависимости от количества строк во входном файле.
- Решение представить в виде архива с исходным кодом и проектом CMake или Visual Studio (либо в виде ссылки на онлайн Git-репозиторий).
- Код должен компилироваться в GCC или MSVC.

### ПРИМЕР
Файл `input.txt`:
```txt
I've paid my dues
Time after time.
I've done my sentence
But committed no crime.
And bad mistakes ?
I've made a few.
I've had my share of sand kicked in my face
But I've come through.
```
Запуск программы: `mtfind.exe input.txt "?ad"`
### Ожидаемый результат:
```txt
3
5 5 bad
6 6 mad
7 6 had
```

### Критерии оценки решения:
- Правильность выдаваемых результатов.
- Качество и читабельность кода, легкость дальнейшего развития и поддержки.
- Скорость работы и потребление памяти.

Все критерии одинаково важны.

# Review
- Ошибки компиляции:
```include/ThreadPool.h:55:18: error: ‘bind’ is not a member of ‘std’```

Результаты тестов:
```
Segmentation fault
./case
_
10 [FAILED]
./case
_
06 [OK]
Segmentation fault
./case
_
03 [FAILED]
./case
_
05 [FAILED]
./case
_
02 [OK]
./case
_
11 [FAILED]
./case
_
08 [FAILED]
./case
_
14 [FAILED]
./case
_
01 [OK]
Segmentation fault
./case
_
09 [FAILED]
Segmentation fault
./case
_
07 [FAILED]
./case
_
12 [OK]
./case
_
13 [FAILED]
./case
_
04 [OK]
./speed
_
case
_
liberated
_
mask
[TIMEOUT 5 min]
[FAILED]
./speed
_
case
_
lorem
_
ipsum
wall clock time: 0:08.35
user space time: 23.32s
kernel space time: 4.27s
CPU usage: 330%
Maximum memory usage: 151028 KB
[OK]
./speed
_
case
_
lorem
_
ipsum
wall clock time: 0:02.22
user space time: 10.97s
kernel space time: 1.88s
CPU usage: 578%
Maximum memory usage: 709264 KB
[OK]
./speed
_
case
_
die
_
for
_
square
wall clock time: 2:31.20
user space time: 133.77s
kernel space time: 6.26s
CPU usage: 92%
Maximum memory usage: 3199964 KB
[OK]
./speed
_
case
_
die
_
for
_
square
wall clock time: 2:10.70
user space time: 128.21s
kernel space time: 2.55s
CPU usage: 100% - Низкая степень распараллеливания
Maximum memory usage: 3200032 KB
[OK]
./speed
_
case
_
01
_
1GB
_
lotLinesOfTheSameSize
wall clock time: 2:40.06
user space time: 286.54s
kernel space time: 210.09s
CPU usage: 310%
Maximum memory usage: 1360360 KB - Среднее потребление памяти
[OK]
./speed
_
case
_
01
_
1GB
_
lotLinesOfTheSameSize
wall clock time: 2:39.42
user space time: 288.57s
kernel space time: 213.70s - Высокие накладные расходы на синхронизацию
CPU usage: 315%
Maximum memory usage: 1360280 KB
[OK]
./speed
_
case
_
die
_
for
_
square2
wall clock time: 1:32.62
user space time: 81.94s
kernel space time: 4.33s
CPU usage: 93%
Maximum memory usage: 2100736 KB
[OK]
./speed
_
case
_
die
_
for
_
square2
wall clock time: 1:20.09
user space time: 78.20s
kernel space time: 1.95s
CPU usage: 100%
Maximum memory usage: 2100856 KB
[OK]
./speed
_
case
_
02
_
1GB
_
singleLine
wall clock time: 0:15.27
user space time: 11.06s
kernel space time: 4.20s
CPU usage: 99%
Maximum memory usage: 3675452 KB
[OK]
./speed
_
case
_
02
_
1GB
_
singleLine
wall clock time: 0:14.86
user space time: 10.95s
kernel space time: 3.90s
CPU usage: 99%
Maximum memory usage: 3675228 KB
[OK]
```
- Немного странный код catch ```(std::runtimeerror& exception) { std::cerr << exception.what() << std::endl; } catch (std::exception& exception) { std::cerr << exception.what() << std::endl; }``` Зачем тут сделано два catch-а если обработка одинаковая?
- Признаки GOD-object у класса MtFind. По сути его поля это глобальные перменные.
  1. Неявная сигнатура метода MtFind::Read(). Фактически принимает путь к файлу и маску а возвращает std::list
  2. Неявная сигнатура метода MtFind::Print(). Фактически принимает std::list
- Построчный алгоритм распараллеливания
- выход за пределы массива в случае если длина маски больше чем длина строки в файле
- Квадратичный алгоритм поиска
- Конструктор MtFind помечена как noexcept, хотя таковым не является
- Нет обработки исключений в потоках
+ Интересная реализация ThreadPool, с усложнениями неиспользуемыми в данном приложении. (было скопипащено из
другого решения?)
