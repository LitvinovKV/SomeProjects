# String and File Project

Реализация предполагает наличие консольного приложения, которому при помощи аргументов командной строки 
подаются на вход путь к обрабатываемой директории с файлами, маска искомых файлов, 
файл с заданной подстрокой и файл с результатами поиска (перечень файлов, в которых встретилась искомая подстрока).

Данное приложение расчитано на ОС Linux, следовательно вводить путь следует следующим образом:  
/home/Folder1/Folder2 => :/home:/Folder1:/Folder2  
Т.к. '/' опускается консолью и если добавлять перед этим ':', то данный символ будет заменяться на '/' и проблем не будет. 

Пример запуска может выглядеть следующим образом:  
```
./MyApp.out -p “:/home:/Temp” -m “*.*” -i “String.txt” -o “Result.txt”,
```

где:  
 - p – путь к обрабатываемой директории с файлами;
 - m – маска имен файлов, в которых осуществляется поиск;
 - i – путь к файлу с заданной подстрокой;
 - o – путь к файлу с результатами поиска.
