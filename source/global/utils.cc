#include <iostream>
#include <string>
#include <stdio.h>
#include <iomanip>
#include <ctime>

std::string getCurrentHour() {
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << now->tm_hour;

    return oss.str();
}

std::string getCurrentMinute() {
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << now->tm_min;

    return oss.str();
}

std::string getCurrentSecond() {
    std::time_t t = std::time(nullptr);
    std::tm* now = std::localtime(&t);
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << now->tm_sec;

    return oss.str();
}