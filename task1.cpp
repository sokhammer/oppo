#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>

using namespace std;

struct TemperatureMeasurement {
  string date;      // гггг.мм.дд
  string location;  // строка в кавычках
  double value;     // дробное значение
};

TemperatureMeasurement ParseTemperatureMeasurement(const string& input) {
  TemperatureMeasurement measurement;
  istringstream iss(input);
  
  iss >> measurement.date;
  iss >> ws;
  
  char first_char = iss.peek();
  if (first_char == '"') {
    iss.get();
    getline(iss, measurement.location, '"');
  } else {
    iss >> measurement.location;
  }
  
  iss >> measurement.value;
  return measurement;
}

void PrintTemperatureMeasurement(const TemperatureMeasurement& tm) {
  cout << "Дата: " << tm.date << endl;
  cout << "Место: " << tm.location << endl;
  cout << "Температура: " << tm.value << "°C" << endl;
  cout << "---" << endl;
}

vector<TemperatureMeasurement> ReadMeasurementsFromFile(const string& filename) {
  vector<TemperatureMeasurement> measurements;
  ifstream input_file(filename.c_str()); // В C++98 нужно .c_str()

  if (!input_file.is_open()) {
    cerr << "Ошибка: не удалось открыть файл " << filename << endl;
    return measurements;
  }

  string line;
  while (getline(input_file, line)) {
    if (line.empty()) {
      continue;
    }

    try {
      TemperatureMeasurement tm = ParseTemperatureMeasurement(line);
      measurements.push_back(tm);
    } catch (...) {
      cerr << "Ошибка при разборе строки: " << line << endl;
    }
  }

  input_file.close();
  return measurements;
}

int main() {
  cout << "Введите имя файла с данными: ";
  string filename;
  getline(cin, filename);

  vector<TemperatureMeasurement> measurements = ReadMeasurementsFromFile(filename);

  cout << "\nЗагруженные измерения:" << endl;
  cout << "=====================" << endl;

  for (size_t i = 0; i < measurements.size(); ++i) {
    PrintTemperatureMeasurement(measurements[i]);
  }
  
  cout << "Всего загружено измерений: " << measurements.size() << endl;
  return 0;
}
