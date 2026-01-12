#include "temp_data.h"

#include <algorithm>
#include <cctype>
#include <ctime>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;


struct MenuItem {
    string description;
    bool (*function)(vector<TempData>&);
};

// Функция для сохранения данных в файл
bool SaveToFile(const vector<TempData>& measurements, const string& filename) {
    ofstream file(filename.c_str());

    if (!file.is_open()) {
        cout << "Ошибка: не удалось открыть файл для записи!" << endl;
        return false;
    }

    for (vector<TempData>::const_iterator it = measurements.begin(); 
         it != measurements.end(); ++it) {
        file << it->get_date() << " " << it->get_place() << " " 
             << fixed << setprecision(1) << it->get_value() << endl;
    }

    file.close();
    return true;
}

// Функции меню

bool ShowAll(vector<TempData>& measurements) {
    cout << "\n--- Все данные ---" << endl;
    if (measurements.empty()) {
        cout << "Нет данных для отображения." << endl;
    }
    else {
        for (vector<TempData>::iterator it = measurements.begin(); 
             it != measurements.end(); ++it) {
            cout << *it << endl;
        }
    }
    return true;
}

bool SortByDateMenu(vector<TempData>& measurements) {
    if (measurements.empty()) {
        cout << "Нет данных для сортировки." << endl;
        return true;
    }

    SortByDate(measurements);
    cout << "\n--- Данные отсортированы по дате ---" << endl;
    for (vector<TempData>::iterator it = measurements.begin(); 
         it != measurements.end(); ++it) {
        cout << *it << endl;
    }
    return true;
}

bool FilterByPlaceMenu(vector<TempData>& measurements) {
    if (measurements.empty()) {
        cout << "Нет данных для фильтрации." << endl;
        return true;
    }

    string place_filter;
    cout << "Введите место для фильтрации: ";
    cin.ignore();  // Очищаем буфер
    getline(cin, place_filter);

    vector<TempData> filtered = FilterByPlace(measurements, place_filter);

    cout << "\n--- Данные для места '" << place_filter << "' ---" << endl;
    cout << "Найдено записей: " << filtered.size() << endl;

    if (filtered.empty()) {
        cout << "Записей для указанного места не найдено." << endl;
    }
    else {
        for (vector<TempData>::iterator it = filtered.begin(); 
             it != filtered.end(); ++it) {
            cout << *it << endl;
        }
    }
    return true;
}

bool FilterByTemperatureMenu(vector<TempData>& measurements) {
    if (measurements.empty()) {
        cout << "Нет данных для фильтрации." << endl;
        return true;
    }

    float min_temp, max_temp;
    cout << "Введите минимальную температуру: ";
    cin >> min_temp;
    cout << "Введите максимальную температуру: ";
    cin >> max_temp;

    // Проверка корректности диапазона температур
    if (min_temp > max_temp) {
        cout << "Ошибка: минимальная температура не может быть больше максимальной!" << endl;
        return true;
    }

    vector<TempData> filtered = FilterByTemperature(measurements, min_temp, max_temp);

    cout << "\n--- Данные в диапазоне " << min_temp << " - " << max_temp << " ---" << endl;
    cout << "Найдено записей: " << filtered.size() << endl;

    if (filtered.empty()) {
        cout << "Записей в указанном диапазоне температур не найдено." << endl;
    }
    else {
        for (vector<TempData>::iterator it = filtered.begin(); 
             it != filtered.end(); ++it) {
            cout << *it << endl;
        }
    }
    return true;
}

// Функция для чтения температуры с валидацией
float ReadTemperature(const string& prompt) {
    float value;
    while (true) {
        cout << prompt;
        cin >> value;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Ошибка: введите корректное числовое значение!" << endl;
        }
        else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
    }
}

// Функция для чтения даты с валидацией
string ReadDate() {
    string date;
    while (true) {
        cout << "Введите дату (формат: ДД.ММ.ГГГГ): ";
        cin >> date;

        // Базовая проверка формата
        if (date.length() == 10 && date[2] == '.' && date[5] == '.') {
            // Проверяем, что остальные символы - цифры
            bool valid = true;
            for (int i = 0; i < 10; ++i) {
                if (i == 2 || i == 5) continue;
                if (!isdigit((unsigned char)date[i])) {
                    valid = false;
                    break;
                }
            }

            if (valid) {
                return date;
            }
        }

        cout << "Неверный формат даты! Используйте формат ДД.ММ.ГГГГ" << endl;
    }
}

// Функция для чтения места с валидацией
string ReadPlace() {
    string place;
    while (true) {
        cout << "Введите место измерения: ";
        getline(cin, place);

        if (place.empty()) {
            cout << "Место измерения не может быть пустым!" << endl;
            continue;
        }

        bool valid = true;
        for (size_t i = 0; i < place.size(); ++i) {
            char c = place[i];
            if (!isalnum((unsigned char)c) && c != '_' && c != '-' && c != ' ') {
                cout << "Недопустимые символы в названии места! Разрешены только буквы, "
                        "цифры, '_', '-' и пробелы" << endl;
                valid = false;
                break;
            }
        }

        if (valid) {
            return place;
        }
    }
}

bool AddManualData(vector<TempData>& measurements) {
    cout << "\n--- Ручной ввод данных ---" << endl;

    try {
        string date = ReadDate();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');  // Очищаем буфер
        string place = ReadPlace();
        float temperature = ReadTemperature("Введите температуру: ");

        // Создаем новую запись
        TempData new_data(date, place, temperature);
        measurements.push_back(new_data);

        cout << "Данные успешно добавлены!" << endl;
        cout << new_data << endl;
    }
    catch (const exception& e) {
        cout << "Ошибка при добавлении данных: " << e.what() << endl;
    }

    return true;
}

bool ExitProgram(vector<TempData>& measurements) {
    cout << "\nСохранить изменения в файл? (y/n): ";
    char choice;
    cin >> choice;

    if (choice == 'y' || choice == 'Y') {
        if (SaveToFile(measurements, "Data.txt")) {
            cout << "Данные успешно сохранены в файл Data.txt" << endl;
        }
    }

    cout << "Выход из программы." << endl;
    return false;
}

int main() {
    // Настройка локали для русского языка
    setlocale(LC_ALL, "");

    vector<TempData> measurements = TempData::ReadAllFromFile("Data.txt");
    cout << "Успешно загружено записей из файла: " << measurements.size() << endl;

    // Меню в стиле C++98 (без std::function)
    map<int, MenuItem> menu;
    menu[1] = (MenuItem){"Показать все данные", ShowAll};
    menu[2] = (MenuItem){"Отсортировать по дате", SortByDateMenu};
    menu[3] = (MenuItem){"Фильтровать по месту", FilterByPlaceMenu};
    menu[4] = (MenuItem){"Фильтровать по диапазону температур", FilterByTemperatureMenu};
    menu[5] = (MenuItem){"Добавить данные вручную", AddManualData};
    menu[6] = (MenuItem){"Выйти", ExitProgram};

    int choice;
    bool should_continue = true;

    do {
        // Отображаем меню
        cout << "\n=== МЕНЮ ===" << endl;
        for (map<int, MenuItem>::const_iterator it = menu.begin(); 
             it != menu.end(); ++it) {
            cout << it->first << ". " << it->second.description << endl;
        }
        cout << "Выберите действие: ";

        cin >> choice;

        // Валидация ввода меню
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Ошибка: введите число от 1 до " << menu.size() << "!" << endl;
            continue;
        }

        try {
            map<int, MenuItem>::iterator it = menu.find(choice);
            if (it != menu.end()) {
                // Вызываем функцию
                should_continue = it->second.function(measurements);
            }
            else {
                cout << "Ошибка: неверный пункт меню! Выберите от 1 до " 
                     << menu.size() << "." << endl;
            }
        }
        catch (const exception& e) {
            cout << "Неожиданная ошибка: " << e.what() << endl;
        }

    } while (should_continue);

    return 0;
}
