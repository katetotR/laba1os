#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <clocale>
#include <cmath>
#include "employee.h"
using namespace std;

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "Russian");
    if (argc != 3)
    {
        cout << "Использование: Creator.exe <файл> <количество>\n";
        return 1;
    }

    int n{};

    istringstream countInput(argv[2]);
    if (!(countInput >> n) || !(countInput >> ws).eof() || n <= 0)
    {
        cout << "Некорректное количество сотрудников.\n";
        return 1;
    }

    ofstream file(argv[1], ios::binary);
    if (!file)
    {
        cout << "Не удалось создать бинарный файл.\n";
        return 1;
    }

    for (int i = 0; i < n; ++i)
    {
        employee emp{};
        string name;
        cout << "\nСотрудник " << i + 1 << '\n';
        cout << "Номер: ";
        if (!(cin >> emp.num))
        {
            cout << "Некорректный номер.\n";
            return 1;
        }

        cout << "Имя (одно слово, до 9 байт): ";
        cin >> name;

        if (!cin || name.empty() || name.size() >= sizeof(emp.name))
        {
            cout << "Имя слишком длинное или не введено.\n";
            return 1;
        }
        for (size_t j = 0; j < name.size(); ++j)
            emp.name[j] = name[j];

        cout << "Отработанные часы (дробную часть через точку): ";
        if (!(cin >> emp.hours) || !isfinite(emp.hours) || emp.hours < 0)
        {
            cout << "Часы должны быть неотрицательным конечным числом.\n";
            return 1;
        }

        file.write(reinterpret_cast<const char*>(&emp), sizeof(emp));
        if (!file)
        {
            cout << "Ошибка записи сотрудника.\n";
            return 1;
        }
    }

    file.close();
    if (!file)
    {
        cout << "Не удалось завершить запись файла.\n";
        return 1;
    }
    return 0;
}
