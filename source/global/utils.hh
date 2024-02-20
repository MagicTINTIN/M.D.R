#pragma once

#include <iostream>
#include <string>
#include <stdio.h>
#include <time.h>

std::string getCurrentHour();
std::string getCurrentMinute();
std::string getCurrentSecond();

std::string replaceNonAsciiWithAscii(const std::string &input);
std::string convertToPrintable(const std::string &input, const int &maxLength, const int &alignment = -1);