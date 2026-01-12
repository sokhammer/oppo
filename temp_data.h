#ifndef TEMP_DATA_H_
#define TEMP_DATA_H_

#include <ctime>
#include <iostream>
#include <string>
#include <vector>

class TempData {
private:
    std::tm date_;
    std::string place_;
    float value_;

    // Вспомогательная функция для проверки корректности даты
    static bool IsValidDate(int year, int month, int day);

public:
    TempData(const std::string& date_str, const std::string& place, float value);

    // Геттеры
    std::string get_date() const;
    std::tm get_date_tm() const;
    std::string get_place() const;
    float get_value() const;

    // Статический метод для чтения из файла
    static std::vector<TempData> ReadAllFromFile(const std::string& filename);
};

// Внешние функции (объявления)
std::vector<TempData> FilterByPlace(const std::vector<TempData>& data,
                                    const std::string& target_place);
std::vector<TempData> FilterByTemperature(const std::vector<TempData>& data,
                                          float min_temp, float max_temp);
void SortByDate(std::vector<TempData>& data);
bool CompareTm(const std::tm& tm1, const std::tm& tm2);

// Перегрузка оператора вывода
std::ostream& operator<<(std::ostream& os, const TempData& data);

#endif  // TEMP_DATA_H_
