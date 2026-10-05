#include <iostream>
#include <vector>
#include <stdexcept>
#include <windows.h>


typedef double (*CalculateAverageFunc)(const int*, int);


struct ArrayData {
    std::vector<int> arr;
    int min_val = 0;
    int max_val = 0;
    double average_val = 0.0;
};


DWORD WINAPI MinMaxThread(LPVOID lpParam) {
    ArrayData* data = static_cast<ArrayData*>(lpParam);
    if (data->arr.empty()) return 0;

    int min_val = data->arr[0];
    int max_val = data->arr[0];

    for (size_t i = 0; i < data->arr.size(); ++i) {
       
        if (data->arr[i] < min_val) {
            min_val = data->arr[i];
        }
        Sleep(7); 

       
        if (data->arr[i] > max_val) {
            max_val = data->arr[i];
        }
        Sleep(7); 
    }

    data->min_val = min_val;
    data->max_val = max_val;

    std::cout << "\n[min_max] Минимальный элемент: " << min_val << std::endl;
    std::cout << "[min_max] Максимальный элемент: " << max_val << std::endl;

    return 0;
}

DWORD WINAPI AverageThread(LPVOID lpParam) {
    ArrayData* data = static_cast<ArrayData*>(lpParam);
    if (data->arr.empty()) return 0;

    
    for (size_t i = 0; i < data->arr.size(); ++i) {
        Sleep(12);
    }

    HMODULE hLib = LoadLibraryA("test_OS_lab_2.dll");
    if (hLib == NULL) {
        std::cerr << "\n[Ошибка DLL] Не удалось найти test.dll! Убедитесь, что файл лежит рядом с .exe" << std::endl;
        return 1;
    }

    CalculateAverageFunc calcAvg = (CalculateAverageFunc)GetProcAddress(hLib, "CalculateAverage");
    if (calcAvg == NULL) {
        std::cerr << "\n[Ошибка DLL] Функция CalculateAverage не найдена внутри библиотеки!" << std::endl;
        FreeLibrary(hLib);
        return 1;
    }

    
    data->average_val = calcAvg(data->arr.data(), static_cast<int>(data->arr.size()));

    
    FreeLibrary(hLib);
    

    std::cout << "\n[average] Среднее арифметическое: " << data->average_val << std::endl;

    return 0;
}


int main() {
    setlocale(LC_ALL, ".UTF-8"); 

    try {
        int n;
        std::cout << "Введите размерность массива: ";
        
        if (!(std::cin >> n) || n <= 0) {
            throw std::invalid_argument("Некорректный размер! Ожидалось положительное целое число.");
        }

        ArrayData data;
        data.arr.resize(n);

        std::cout << "Введите " << n << " элементов массива через пробел: ";
        for (int i = 0; i < n; ++i) {
            if (!(std::cin >> data.arr[i])) {
                throw std::invalid_argument("Ошибка ввода! Ожидалось целое число, а не буква или символ.");
            }
        }

        
        HANDLE hMinMax = CreateThread(NULL, 0, MinMaxThread, &data, 0, NULL);
        HANDLE hAverage = CreateThread(NULL, 0, AverageThread, &data, 0, NULL);

        if (hMinMax == NULL || hAverage == NULL) {
            throw std::runtime_error("Ошибка системного вызова: не удалось создать потоки.");
        }

       
        WaitForSingleObject(hMinMax, INFINITE);
        WaitForSingleObject(hAverage, INFINITE);

        
        CloseHandle(hMinMax);
        CloseHandle(hAverage);

        
        int avg_int = static_cast<int>(data.average_val);
        for (size_t i = 0; i < data.arr.size(); ++i) {
            if (data.arr[i] == data.min_val || data.arr[i] == data.max_val) {
                data.arr[i] = avg_int;
            }
        }

        
        std::cout << "\n[main] Модифицированный массив (min и max заменены на " << avg_int << "):" << std::endl;
        for (int val : data.arr) {
            std::cout << val << " ";
        }
        std::cout << std::endl;

    }
    catch (const std::exception& e) {
        
        std::cerr << "\nКРИТИЧЕСКАЯ ОШИБКА: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}