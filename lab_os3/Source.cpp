#include <iostream>
#include <windows.h>
#include <string>
#include <vector>
#include <ctime>

std::wstring filenames[4] = {
        L"C:\\Users\\dstep\\source\\repos\\lab_os3\\sum\\x64\\Debug\\sum.exe",
        L"C:\\Users\\dstep\\source\\repos\\lab_os3\\sub\\x64\\Debug\\sub.exe",
        L"C:\\Users\\dstep\\source\\repos\\lab_os3\\mul\\x64\\Debug\\mul.exe",
        L"C:\\Users\\dstep\\source\\repos\\lab_os3\\div\\x64\\Debug\\div.exe"
};

std::wstring pipenames[4] = {
    L"\\\\.\\pipe\\for_sum",
    L"\\\\.\\pipe\\for_sub",
    L"\\\\.\\pipe\\for_mul",
    L"\\\\.\\pipe\\for_div" 
};

HANDLE mutex;

// Функция для создания дочернего процесса
float Sum(float a, float b, HANDLE hNamedPipe) {

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Создаем новый процесс
    if (!CreateProcess((LPWSTR)filenames[0].c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        std::cerr << "CreateProcess failed: " << GetLastError() << std::endl;
    }

    if (!ConnectNamedPipe(hNamedPipe, NULL)) {
        std::cerr << "ConnectNamedPipe failed: " << GetLastError() << std::endl;
        CloseHandle(hNamedPipe);
    }

    DWORD written;  // Переменная для хранения количества байт, записанных в канал
    WaitForSingleObject(mutex, INFINITE);
    // Попытка записи первого числа в канал
    BOOL result1 = WriteFile(hNamedPipe, &a, sizeof(a), &written, NULL);
    if (!result1) {
        std::cerr << "Failed to write num1 to pipe. Error: " << GetLastError() << std::endl;
    }
    else {
        std::cout << "Number 1 (" << a << ") written to pipe, bytes written: " << written << std::endl;
    }

    // Попытка записи второго числа в канал
    BOOL result2 = WriteFile(hNamedPipe, &b, sizeof(b), &written, NULL);
    if (!result2) {
        std::cerr << "Failed to write num2 to pipe. Error: " << GetLastError() << std::endl;
    }
    else {
        std::cout << "Number 2 (" << b << ") written to pipe, bytes written: " << written << std::endl;
    }


    DWORD bytesRead;
    float result;
    // Попытка чтения первого числа из канала
    BOOL result3 = ReadFile(hNamedPipe, &result, sizeof(result), &bytesRead, NULL);
    if (!result3 || bytesRead != sizeof(result)) {
        std::cerr << "Failed to read num1 from pipe. Error: " << GetLastError() << std::endl;
    }
    else {
        std::cout << "result read from pipe: " << result << std::endl;
    }

    TerminateProcess(hNamedPipe, 1);

    return result;
}

int main() {
    // Создаем каналы для обмена данными с дочерними процессами
    float array[3] = { 5.0, 7.0, 9.0 };
    HANDLE hNamedPipe[4];
    // Создаем 4 именованных канала
    for (int i = 0; i < 4; ++i) {
        hNamedPipe[i] = CreateNamedPipe(
            (LPWSTR)pipenames[i].c_str(),
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            1,          // Количество каналов экземпляров
            4096,       // Размер выходного буфера
            4096,       // Размер входного буфера
            NMPWAIT_USE_DEFAULT_WAIT, // Время ожидания подключения клиента
            NULL        // Защита по умолчанию
        );

        if (hNamedPipe[i] == INVALID_HANDLE_VALUE) {
            std::cerr << "CreateNamedPipe failed: " << GetLastError() << std::endl;
            return 1;
        }
    }

    std::cout << "Named pipes created successfully." << std::endl;

    float result = 0.0;
    for (int i = 0; i < 3; i++) {
        result = Sum(result, array[i], hNamedPipe[0]);
    }

    return 0;
}
