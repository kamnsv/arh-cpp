##### Содержание  

[RiceEncoding aрхиватор](#riceencoding-aрхиватор)   
[Использование](#использование)  
 	[Linux](#linux)  
		[Зависимости](#зависимости)  
		[Запуск](#запуск)  
		[Тесты скриптами](#тесты-скриптами)  
	[Windows](#windows)  
		[Тестирование](#тестирование) 
        
# RiceEncoding aрхиватор

Реализация архивации данных в виде последовательности целых чисел кодом RiceEncoding. 
Данный метод сжимает числа с помощью выбора такого значения k, чтобы значение b=2^k было наиболее близко к среднему значению последовательности кодируемых чисел. 
Каждое целое n представляется q=[(n–1)/b], сохранённое в унарном коде используя q+1 битов  (лидирующее число единиц = q и нулевой бит, который отделает 2 части кода) и остаток r=(n–1)(mod b), сохранённый в бинарном коде используя k битов. 

Рассмотрим среднее кодируемых чисел mean. 
Округлим mean до ближайшей степени 2 в меньшую сторону, получим b. 
Каждое число n будем представлять, как объединение двух кодовых частей, часть в унарном коде [(n–1)/b] и часть в бинарном коде (n–1)(mod b). 

Пример RiceEncoding:

- Последовательность: 34, 178, 291, 453
- Промежутки: 34, 144, 113, 162
- Среднее: mean = (34 + 144 + 113 + 162) / 4 = 113,33
- Округляем: b = 64 (6 бит)


| Число |      Разложение      | Кодирование |
|-------|----------------------|-------------|
|   34  | 64*0 + (34-1) & 63   | 0 - 100001  | 
|  144  | 64*2 + (144-1) & 63  | 110 - 001111| 
|  113  | 64*1 + (113-1) & 63  | 10 - 110000 |
|  162  | 64*2 + (162-1) & 63  | 110 - 100001|

Имя файла, и тип операции(арх или деарх) – параметры командной строки.
Написать архиватор и деархиватор. Протестировать работу архиватора на случайно сгенерированных бинарных файлах, состоящих из целых чисел. Размер заархивированного файла должен быть всегда меньше файла исходного и однозначно деархивироваться до файла, совпадающего с исходным.


# Использование

```
git clone ...
```


## Linux

### Зависимости

```
sudo apt update
sudo apt install --reinstall build-essential
```

### Запуск

```
cd arh-cpp
sudo g++ src/main.cpp
./a.out
Archiving with Rice encoding.

Use [FILENAME] [ACTION] [OPTIONS].

        FILENAME: The path to the archived file.

        ACTIONS:

        --arh            Archive file, output archive with '.arh' extension.
        --dearh          Dearchive the file, the output is the original file, but with the extension '.dearh'.

        OPTIONS:

        --k              Parameter [1;8] for encoding Rice, default 6.
        --debug          Optional flag to display information about bytes.

```

### Тесты скриптами

```
sudo chmod +x scripts/*
sudo ./scripts/test.sh 
```


## Windows

> Компилировать `win.cpp`, здесь хэш и тесты включены в сборку.

```
> win.exe 
Archiving with Rice encoding.

Use [FILENAME] [ACTION] [OPTIONS].

        FILENAME: The path to the archived file or the name for generating a new one,
                  if the '--n' option is specified with the number of bytes

        ACTIONS:

        --arh            Archive file, output archive with '.arh' extension.
        --dearh          Dearchive the file, the output is the original file, but with the extension '.dearh'.

        OPTIONS:

        --n              Creates a random binary file of the given length [n] wtis name [FILENAME].
        --k              Parameter [1;8] for encoding Rice, default 6.
        --debug          Optional flag to display information about bytes.
```

### Тестирование

```
arh.exe test.exe --n 1000 --arh --dearh
```
