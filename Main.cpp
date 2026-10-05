#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>
#include <iomanip>
#include <clocale>
#include <cmath>
#include "employee.h"
using namespace std;

bool runProcess(string command);

int main()
{
    setlocale(LC_ALL, "Russian");

    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);

    cout << "Введите название файла, куда записать данные: " << '\n';
    string filename;
    getline(cin, filename);
    if (filename.empty())
    {
        cout << "Имя файла не должно быть пустым.\n";
        return 1;
    }

    cout << "Введите количество сотрудников: " << '\n';
    int n{};
    if (!(cin >> n) || n <= 0)
    {
        cout << "Нужно положительное целое количество сотрудников.\n";
        return 1;
    }

    string creatorCommand = "Creator.exe \"" + filename + "\" " + to_string(n);
    if (!runProcess(creatorCommand))
        return 1;

    ifstream binaryFile(filename, ios::binary);
    if (!binaryFile)
    {
        cout << "Не удалось открыть бинарный файл.\n";
        return 1;
    }

    cout << "\nСодержимое бинарного файла:\n";
    cout << "Номер\tИмя\tЧасы\n";
    employee emp{};

    while (binaryFile.read(reinterpret_cast<char*>(&emp), sizeof(emp)))
        cout << emp.num << '\t' << emp.name << '\t' << emp.hours << '\n';

    if (!binaryFile.eof() || binaryFile.gcount() != 0)
    {
        cout << "Ошибка чтения или неполная запись в бинарном файле.\n";
        return 1;
    }
    binaryFile.close();

    cout << "\nВведите имя файла отчета: " << '\n';
    string reportFilename;

    getline(cin >> ws, reportFilename);
    if (reportFilename.empty() || reportFilename == filename)
    {
        cout << "Для отчета нужно отдельное непустое имя файла.\n";
        return 1;
    }

    cout << "Введите оплату за час работы (дробную часть через точку): " << '\n';
    double payment{};
    if (!(cin >> payment) || !isfinite(payment) || payment < 0)
    {
        cout << "Оплата должна быть неотрицательным конечным числом.\n";
        return 1;
    }

    string reporterCommand = "Reporter.exe \"" + filename + "\" \""
        + reportFilename + "\" " + to_string(payment);
    if (!runProcess(reporterCommand))
        return 1;

    ifstream reportFile(reportFilename);
    if (!reportFile)
    {
        cout << "Не удалось открыть отчет.\n";
        return 1;
    }

    cout << "\n";
    string line;

    while (getline(reportFile, line))
        cout << line << '\n';
    if (!reportFile.eof())
    {
        cout << "Ошибка чтения отчета.\n";
        return 1;
    }
    return 0;
}

bool runProcess(string command)
{

    STARTUPINFOA startupInfo{};
    startupInfo.cb = sizeof(startupInfo);

    PROCESS_INFORMATION processInfo{};

    bool success = CreateProcessA(
        nullptr,
        &command[0],
        nullptr,
        nullptr,
        false,
        0,
        nullptr,
        nullptr,
        &startupInfo,
        &processInfo
    );

    if (!success)
    {
        cout << "CreateProcess error: " << GetLastError() << '\n';
        return false;
    }

    DWORD waitResult = WaitForSingleObject(processInfo.hProcess, INFINITE);
    if (waitResult != WAIT_OBJECT_0)
    {
        cout << "Ошибка ожидания процесса: " << GetLastError() << '\n';
        CloseHandle(processInfo.hProcess);
        CloseHandle(processInfo.hThread);
        return false;
    }

    DWORD exitCode{};
    bool gotExitCode = GetExitCodeProcess(processInfo.hProcess, &exitCode);
    if (!gotExitCode)
        cout << "Ошибка получения кода завершения: " << GetLastError() << '\n';

    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);

    if (!gotExitCode)
        return false;
    if (exitCode != 0)
    {
        cout << "Дочерняя программа завершилась с ошибкой: " << exitCode << '\n';
        return false;
    }
    return true;
}
