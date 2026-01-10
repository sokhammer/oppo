#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>

using namespace std;

struct TemperatureMeasurement {
    string date;
    string location;
    double value;
};

string ReadDateFromStream(istringstream& iss) {
    string date;
    iss >> date;
    return date;
}

string ReadQuotedLocation(istringstream& iss) {
    iss.get();  // Убираем '"'
    string location;
    getline(iss, location, '"');
    return location;
}

string ReadUnquotedLocation(istringstream& iss) {
    string location;
    iss >> location;
    return location;
}

string ReadLocationFromStream(istringstream& iss) {
    iss >> ws;
    char first_char = iss.peek();
    if (first_char == '"') {
        return ReadQuotedLocation(iss);
    }
    return ReadUnquotedLocation(iss);
}

double ReadTemperatureValueFromStream(istringstream& iss) {
    double value;
    iss >> value;
    return value;
}

TemperatureMeasurement ParseTemperatureMeasurement(const string& input_line) {
    istringstream input_stream(input_line);
    TemperatureMeasurement measurement;
    measurement.date = ReadDateFromStream(input_stream);
    measurement.location = ReadLocationFromStream(input_stream);
    measurement.value = ReadTemperatureValueFromStream(input_stream);
    return measurement;
}

bool IsEmptyLine(const string& line) {
    return line.empty();
}

bool OpenFile(ifstream& input_file, const string& filename) {
    input_file.open(filename);
    if (!input_file.is_open()) {
        cerr << "Ошибка: не удалось открыть файл " << filename << endl;
        return false;
    }
    return true;
}

void ProcessLine(const string& line, vector<TemperatureMeasurement>& measurements) {
    if (IsEmptyLine(line)) return;
    
    try {
        TemperatureMeasurement measurement = ParseTemperatureMeasurement(line);
        measurements.push_back(measurement);
    } catch (...) {
        cerr << "Ошибка при разборе строки: " << line << endl;
    }
}

vector<TemperatureMeasurement> ReadAllMeasurementsFromFile(const string& filename) {
    vector<TemperatureMeasurement> measurements;
    ifstream input_file;
    
    if (!OpenFile(input_file, filename)) {
        return measurements;
    }
    
    string line;
    while (getline(input_file, line)) {
        ProcessLine(line, measurements);
    }
    
    input_file.close();
    return measurements;
}

void DisplaySingleMeasurement(const TemperatureMeasurement& measurement) {
    cout << "Дата: " << measurement.date << endl;
    cout << "Место: " << measurement.location << endl;
    cout << "Температура: " << measurement.value << "°C" << endl;
    cout << "---" << endl;
}

void DisplayAllMeasurements(const vector<TemperatureMeasurement>& measurements) {
    cout << "\nЗагруженные измерения:" << endl;
    cout << "=====================" << endl;
    
    for (const auto& measurement : measurements) {
        DisplaySingleMeasurement(measurement);
    }
}

void DisplayMeasurementsCount(const vector<TemperatureMeasurement>& measurements) {
    cout << "Всего загружено измерений: " << measurements.size() << endl;
}

string GetInputFilenameFromUser() {
    cout << "Введите имя файла с данными: ";
    string filename;
    getline(cin, filename);
    return filename;
}

int main() {
    string filename = GetInputFilenameFromUser();
    vector<TemperatureMeasurement> measurements = ReadAllMeasurementsFromFile(filename);
    DisplayAllMeasurements(measurements);
    DisplayMeasurementsCount(measurements);
    return 0;
}