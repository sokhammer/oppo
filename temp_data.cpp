#include "temp_data.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <cstring> 

using namespace std;

// Конструктор
TempData::TempData(const string& date_str, const string& place, float value)
    : place_(place), value_(value) {
    
    // Удаляем кавычки, если они есть
    if (place_.size() >= 2 && place_[0] == '"' && place_[place_.size() - 1] == '"') {
        place_ = place_.substr(1, place_.size() - 2);
    }

    // Проверка что место не пустое
    if (place_.empty()) {
        throw invalid_argument("Место измерения не может быть пустым");
    }

    // Проверка на допустимые символы в названии места
    for (size_t i = 0; i < place_.size(); ++i) {
        char c = place_[i];
        if (!isalnum((unsigned char)c) && c != '_' && c != '-' && c != ' ') {
            throw invalid_argument("Недопустимые символы в названии места: " + place_);
        }
    }

    // Преобразование строки в tm
    istringstream iss(date_str);
    char dot1, dot2;
    int day, month, year;
    iss >> day >> dot1 >> month >> dot2 >> year;

    // Проверка корректности формата даты
    if (iss.fail() || dot1 != '.' || dot2 != '.') {
        throw invalid_argument("Неверный формат даты: " + date_str);
    }

    // Обнуляем структуру tm
    memset(&date_, 0, sizeof(date_));
    
    date_.tm_mday = day;
    date_.tm_mon = month - 1;      // tm_mon от 0 до 11
    date_.tm_year = year - 1900;   // tm_year с 1900 года
    date_.tm_isdst = -1;           // не учитывать летнее время

    // Базовые проверки
    if (date_.tm_mday < 1 || date_.tm_mday > 31) {
        throw invalid_argument("Неверный день в дате: " + date_str);
    }
    if (date_.tm_mon < 0 || date_.tm_mon > 11) {
        throw invalid_argument("Неверный месяц в дате: " + date_str);
    }
    if (date_.tm_year < 0) {
        throw invalid_argument("Неверный год в дате: " + date_str);
    }

    // Проверка корректности дней в месяцах
    if (!IsValidDate(year, month, day)) {
        throw invalid_argument("Некорректная дата: " + date_str);
    }

    // Нормализуем дату
    time_t time_val = mktime(&date_);
    if (time_val == -1) {
        throw invalid_argument("Некорректная дата: " + date_str);
    }

    // Проверка что дата не в будущем
    time_t current_time = time(NULL);
    if (time_val > current_time) {
        throw invalid_argument("Дата " + date_str + " находится в будущем");
    }

    // Проверка температуры
    if (value_ < -100.0f || value_ > 60.0f) {
        throw invalid_argument("Температура выходит за допустимые пределы (-100...+60)");
    }
}

// Вспомогательная функция для проверки корректности даты
bool TempData::IsValidDate(int year, int month, int day) {
    // Проверка високосного года
    bool is_leap_year = false;
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        is_leap_year = true;
    }

    // Проверка дней в месяцах
    if (month < 1 || month > 12) return false;

    int days_in_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    // Корректировка для февраля в високосный год
    if (month == 2 && is_leap_year) {
        days_in_month[1] = 29;
    }

    return day >= 1 && day <= days_in_month[month - 1];
}

// Геттеры
string TempData::get_date() const {
    ostringstream oss;
    oss << setw(2) << setfill('0') << date_.tm_mday << "."
        << setw(2) << setfill('0') << (date_.tm_mon + 1) << "."
        << (date_.tm_year + 1900);
    return oss.str();
}

tm TempData::get_date_tm() const { 
    tm result;
    memcpy(&result, &date_, sizeof(tm));
    return result; 
}

string TempData::get_place() const { return place_; }

float TempData::get_value() const { return value_; }

// Статический метод для чтения из файла
vector<TempData> TempData::ReadAllFromFile(const string& filename) {
    vector<TempData> result;
    ifstream fin(filename.c_str());

    if (!fin.is_open()) {
        cout << "Ошибка открытия файла: " << filename << endl;
        return result;
    }

    string line;
    int line_number = 0;

    // Читаем все строки из файла
    while (getline(fin, line)) {
        line_number++;

        // Пропускаем пустые строки
        if (line.empty()) {
            continue;
        }

        istringstream iss(line);
        string date, place;
        float value;

        // Пытаемся прочитать три аргумента
        if (!(iss >> date >> place >> value)) {
            cout << "Ошибка в строке " << line_number
                 << ": неверное количество аргументов или формат данных" << endl;
            continue;
        }

        // Проверяем, есть ли лишние аргументы
        string extra;
        if (iss >> extra) {
            cout << "Ошибка в строке " << line_number << ": слишком много аргументов" << endl;
            continue;
        }

        try {
            result.push_back(TempData(date, place, value));
        }
        catch (const exception& e) {
            cout << "Ошибка в строке " << line_number << ": " << e.what() << endl;
        }
    }

    fin.close();
    return result;
}

// Перегрузка оператора вывода
ostream& operator<<(ostream& os, const TempData& data) {
    os << "Дата измерений: " << data.get_date()
       << " Место проведения измерений: " << data.get_place()
       << " результат измерений: " << data.get_value();
    return os;
}

// Реализации внешних функций

// Функция для фильтрации по месту
vector<TempData> FilterByPlace(const vector<TempData>& data,
                               const string& target_place) {
    vector<TempData> result;
    for (vector<TempData>::const_iterator it = data.begin(); it != data.end(); ++it) {
        if (it->get_place() == target_place) {
            result.push_back(*it);
        }
    }
    return result;
}

// Функция для фильтрации по диапазону температур
vector<TempData> FilterByTemperature(const vector<TempData>& data,
                                     float min_temp, float max_temp) {
    vector<TempData> result;
    for (vector<TempData>::const_iterator it = data.begin(); it != data.end(); ++it) {
        if (it->get_value() >= min_temp && it->get_value() <= max_temp) {
            result.push_back(*it);
        }
    }
    return result;
}

// Функция для сравнения двух tm структур
bool CompareTm(const tm& tm1, const tm& tm2) {
    // Создаем копии для безопасного использования с mktime
    tm tm1_copy;
    tm tm2_copy;
    memcpy(&tm1_copy, &tm1, sizeof(tm));
    memcpy(&tm2_copy, &tm2, sizeof(tm));
    
    // Преобразуем в time_t для сравнения
    time_t time1 = mktime(&tm1_copy);
    time_t time2 = mktime(&tm2_copy);

    return difftime(time1, time2) < 0;
}

// Функция для сортировки по дате
void SortByDate(vector<TempData>& data) {
    // Используем итераторы вместо лямбда-выражений для C++98
    for (size_t i = 0; i < data.size(); ++i) {
        for (size_t j = i + 1; j < data.size(); ++j) {
            if (CompareTm(data[j].get_date_tm(), data[i].get_date_tm())) {
                TempData temp = data[i];
                data[i] = data[j];
                data[j] = temp;
            }
        }
    }
}
