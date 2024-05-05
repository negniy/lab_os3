#include <iostream>
#include <windows.h>
#include <string>

int main(int argc, char* argv[]) {
    // Открытие именованного канала для чтения
    HANDLE hPipe = CreateFile(TEXT("\\\\.\\pipe\\for_sum"),
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (hPipe == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open named pipe. Error code: " << GetLastError() << std::endl;
        return 1;
    }

    // Чтение операндов из именованного канала
    float operand1, operand2;
    DWORD bytesRead;
    if (!ReadFile(hPipe, &operand1, sizeof(operand1), &bytesRead, NULL)) {
        std::cerr << "Failed to read operand1 from named pipe. Error code: " << GetLastError() << std::endl;
        CloseHandle(hPipe);
        return 1;
    }

    if (!ReadFile(hPipe, &operand2, sizeof(operand2), &bytesRead, NULL)) {
        std::cerr << "Failed to read operand2 from named pipe. Error code: " << GetLastError() << std::endl;
        CloseHandle(hPipe);
        return 1;
    }

    // Закрываем дескриптор именованного канала
    CloseHandle(hPipe);

    // Выполнение операции
    float result = operand1 + operand2;

    // Отправляем результат обратно родительскому процессу через канал res_sum
    HANDLE hPipe1 = CreateFile(TEXT("\\\\.\\pipe\\res_sum"),
        GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (hPipe1 == INVALID_HANDLE_VALUE) {
        std::cerr << "Failed to open named pipe. Error code: " << GetLastError() << std::endl;
        return 1;
    }

    DWORD bytesWritten;
    if (!WriteFile(hPipe1, &result, sizeof(result), &bytesWritten, NULL)) {
        std::cerr << "Failed to write result to named pipe. Error code: " << GetLastError() << std::endl;
        CloseHandle(hPipe1);
        return 1;
    }

    // Закрываем дескриптор именованного канала
    CloseHandle(hPipe1);

    return 0;
}
