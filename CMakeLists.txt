cmake_minimum_required(VERSION 3.10)
project(WeatherApp)

# Устанавливаем стандарт C++98
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Отключаем расширения компилятора для строгого C++98

# Список исходных файлов
set(SOURCES
        main.cpp
        temp_data.cpp
)

# Создание исполняемого файла
add_executable(weather_app ${SOURCES})

# Для macOS добавляем флаги локализации
if(APPLE)
    target_compile_options(weather_app PRIVATE
            -Wall
            -Wextra
    )

    # Для русского текста в консоли
    target_link_libraries(weather_app iconv)
endif()
