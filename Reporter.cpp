#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <clocale>
#include <cmath>
#include "employee.h"
using namespace std;

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "Russian");
    if (argc != 4)
    {
        cout << "Использование: Reporter.exe <бинарный файл> <отчет> <оплата>\n";
        return 1;
    }
    if (string(argv[1]) == argv[2])
    {
        cout << "Бинарный файл и отчет должны быть разными файлами.\n";
        return 1;
    }

    double payment{};
    istringstream paymentInput(argv[3]);
    if (!(paymentInput >> payment) || !(paymentInput >> ws).eof()
        || !isfinite(payment) || payment < 0)
    {
        cout << "Некорректная оплата за час.\n";
        return 1;
    }

    ifstream binaryFile(argv[1], ios::binary);
    if (!binaryFile)
    {
        cout << "Не удалось открыть бинарный файл.\n";
        return 1;
    }

    vector<employee> employees;
    employee emp{};
    while (binaryFile.read(reinterpret_cast<char*>(&emp), sizeof(emp)))
        employees.push_back(emp);
    if (!binaryFile.eof() || binaryFile.gcount() != 0)
    {
        cout << "Ошибка чтения или неполная запись в бинарном файле.\n";
        return 1;
    }
    binaryFile.close();

    for (size_t i = 0; i < employees.size(); ++i)
    {
        for (size_t j = 1; j < employees.size() - i; ++j)
        {
            if (employees[j - 1].num > employees[j].num)
            {
                employee temp = employees[j - 1];
                employees[j - 1] = employees[j];
                employees[j] = temp;
            }
        }
    }

    ofstream reportFile(argv[2]);
    if (!reportFile)
    {
        cout << "Не удалось создать отчет.\n";
        return 1;
    }

    reportFile << "Отчет по файлу «" << argv[1] << "»\n";
    reportFile << "Номер сотрудника\tИмя сотрудника\tЧасы\tЗарплата\n";

    reportFile << fixed << setprecision(2);
    for (size_t i = 0; i < employees.size(); ++i)
    {
        double salary = employees[i].hours * payment;
        if (!isfinite(salary))
        {
            cout << "Слишком большая зарплата: переполнение.\n";
            return 1;
        }
        reportFile << employees[i].num << '\t'
            << employees[i].name << '\t'
            << employees[i].hours << '\t'
            << salary << '\n';
    }

    reportFile.close();
    if (!reportFile)
    {
        cout << "Ошибка записи отчета.\n";
        return 1;
    }
    return 0;
}
