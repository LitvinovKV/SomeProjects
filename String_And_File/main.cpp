#include <experimental/filesystem>
#include <iostream>
#include <cstring>
#include <string>
#include <algorithm>
#include <fstream>

using namespace std;
namespace fs = std::experimental::filesystem;

static string DIR = "";
static string MASK = "";
static string OutputFileName = "";
static string StringValueFile = "";
static string FindStr = "";
static char SymbolArray[] = "~@#$%^-_(){}'`:*.";


// Функция для проверки файла и подстроки в строках выбранного файла (FileName)
// Возвращает true, если такая подстрока есть в файле
// Возращает false, если нет такой подстори в файле
void checkFileAndPutInOutfile(string PathName) {
    ifstream ReadFile;
    ReadFile.open(PathName);
    ofstream OutputFile;
    OutputFile.open(OutputFileName, ios::app | ios::binary);
    string s = "";
    while (!ReadFile.eof()) {
        ReadFile >> s;
        size_t pose = s.find(FindStr, 0);
        //Если такая подстрока есть в строке, то записать файл в список и досрочно выйти из цикла
        if (pose != string::npos) {
            OutputFile << PathName.c_str() << endl;
            break;
        }
    }
    ReadFile.close();
    OutputFile.close();
}

//Функция для сравнения Маски и Строки
//Возвращает true, если все учтено
//Возвращает false, если строка не подходит под маску
bool searchMask(string Mask, string GlobalStr) {
    //Если нет ни * ни ?, то просто сравнить строки
    vector<string> vectorMasks;
    string str = "";
    //Разбить маску на спецификаторы
    for (size_t i = 0; i < Mask.length(); i++) {
        //Если спецификатор - ?
        if (Mask[i] == '?') {
            //Если была накоплена строка, то занести ее в массив для дальнейшего сравнения
            if (str.length() > 0) vectorMasks.push_back(str);
            //Занести в массив спецификатор - ?
            vectorMasks.push_back("?");
            str = "";
        }
        //Иначе спецификатор - *
        else if (Mask[i] == '*') {
            if (str.length() > 0) vectorMasks.push_back(str);
            vectorMasks.push_back("*");
            str = "";
        }
        else str += Mask[i];
    }
    if (str.length() > 0) vectorMasks.push_back(str);
    reverse(begin(vectorMasks), end(vectorMasks));
    
    bool flagStar = false;
    //Цикл, что бы учесть каждый спецификатор 
    while (vectorMasks.size() != 0) {
        str = vectorMasks.back();
        vectorMasks.pop_back();
        
        //Убрать первый символ из строки
        if (str == "?") {
            //Если спецификатор - ?, а символов нет, то такая строка не подходит
            if (GlobalStr.length() == 0) return false;
            GlobalStr = GlobalStr.substr(1, GlobalStr.length() - 1);
        }
        //Если спецификатор - *, то поставить флаг
        else if (str == "*") {
            //Если спецификатор - *, а символов нет, то такая строка не подходит
            if (GlobalStr.length() == 0) return false;
            flagStar = true;
        }
        //Сравнить слово
        else {
            size_t npos = GlobalStr.find(str, 0);
            //Если такой подстроки нет, то такая строка не подходит
            if (npos == string::npos) return false;
            //Если стоит звездочка и слово начинается сразу в строке, то такая строка не подходит
            if ((flagStar == true) && (npos == 0)) return false;
            GlobalStr = GlobalStr.substr(npos + str.length(), GlobalStr.length() - npos + str.length());
            flagStar = false;
        }
    }
    //Если символы в строке остались и в конце не стоит спецификатор - *, то такая строка не подходит
    if ((flagStar == false) && (GlobalStr.length() > 0)) return false;
    return true;
}

//Рекурсивная функция для прохождения всех поддеркторий дерркитории - GlobalDir
void coutFilesInDir(fs::directory_entry GlobalDir) {
    //Цикл чтобы получить каждую поддиректорию директории
    for (fs::directory_entry p1 : fs::directory_iterator(GlobalDir)) {
        //Если поддиректория является директорией, то вызвать рекурсивную функцию
        if (fs::is_directory(p1) == true) {
            coutFilesInDir(p1);
        }
        //Иначе если это не директория, то это файл
        else {
            //Получить путь файла
            fs::path FilePath = p1;
            //Взять из полного пити только название файла
            string FileName  = FilePath.filename();
            //Запомнить позиции разделительной точки в имени файла и формата
            size_t posFileName = FileName.find('.');
            size_t posMask = MASK.find('.');
            // Если Имя файла не подходит под маску файла, то пропустить этот файл
            if (searchMask(MASK.substr(0, posMask), FileName.substr(0, posFileName)) == false) continue;
            // Если формат файла не подходит под маску формата, то пропустить этот файл
            if (searchMask(MASK.substr(posMask + 1, MASK.length() - posMask), 
                            FileName.substr(posFileName + 1, FileName.length() - posFileName)) == false) continue;
            //Вызвать ф-ию, что бы проверить существование строки в файле
            checkFileAndPutInOutfile(FilePath);
        }
    }
}

//Функция, которая корректирует путь к файлам:
// C:Tmp:File -> C:\Tmp:\File
string changeDir(string str) {
    for (size_t i = 0; i < str.length(); i++) {
        if (str[i] == ':')
            str.replace(i, 1, "/");
    }
    return str;
}

//Функция, которая удаляет лишние скобки '”' в названии файла при вводе с консоли
//Возвращает строку без скобок
string deleteBrackets(char* str) {
    string new_str = "";
    for (size_t i = 0; i < strlen(str); i++ ) {
        // Если симвло буква или цифра
        if (((str[i] >= 'A') && (str[i] <= 'Z')) ||
            ((str[i] >= 'a') && (str[i] <= 'z')) ||
            ((str[i] >= '1') && (str[i] <= '9'))) {
            new_str += str[i];
            continue;
        }
        // Если символ входит в массив SymbolArray
        for (size_t j = 0; j < strlen(SymbolArray); j++) {
            if (str[i] == SymbolArray[j]) { 
                new_str += str[i];
                break;
            }
        }
    }
    return new_str;
}

//MyApp.exe -p “/home/icqking/CPlusPlusProjects” -m “*.*” -i “String.txt” -o “Result.txt”
// DIR = "/home/icqking/CPlusPlusProjects";
// MASK = "*.json";
// StringValueFile = "String.txt";
// OutputFileName = "Result.txt";

int main(int argc, char* argv[])
{
    if (argc != 9) {
        cout << "Error! Arguments count must be = 8:" <<
        " -p 'NameDir' -m 'FilesMask' -i 'InputFile' -o 'OutputFile" << endl;
        return 0;
    }
    
    if ((strcmp(argv[1], "-p") != 0) || (strcmp(argv[3], "-m") != 0) ||
        (strcmp(argv[5], "-i") != 0) || (strcmp(argv[7], "-o") != 0)) {
            cout << "Error! Bad arguments. Must be " <<
             "-p 'NameDir' -m '*FilesMask*' -i 'InputFile' -o 'OutputFile" << endl; 
            return 0;
    }

    DIR = deleteBrackets(argv[2]);
    DIR = changeDir(DIR);
    MASK = deleteBrackets(argv[4]);
    StringValueFile = deleteBrackets(argv[6]);
    OutputFileName = deleteBrackets(argv[8]);

    ifstream StringFile;
    StringFile.open(StringValueFile);
    // Проверить существует ли файл со строкой для поиска
    if (!StringFile) {
        cout << "File with string not found!" << endl;
        StringFile.close();
        return 0;
    } 
    else {
        StringFile >> FindStr;
        StringFile.close();
    }

    ofstream OutFile;
    OutFile.open(OutputFileName);
    //Проверить существует ли файл для вывода результатов
    if (!OutFile) {
        cout << "Output file not found!" << endl;
        OutFile.close();
        return 0;
    }
    else OutFile.close();

    cout << DIR << endl;
    cout << MASK << endl;
    cout << StringValueFile << endl;
    cout << OutputFileName << endl;

    //Вывести поддиректории директории - Path
    for (fs::directory_entry p1 : fs::directory_iterator(DIR)) {
        cout << "Global DIR = " << p1 << endl;
        coutFilesInDir(p1);
    }
    return 0;
}