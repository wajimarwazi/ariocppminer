#pragma once

#include <string>

bool parseArgs(const char* prefix, int argc, char** argv);
void printUsage();
bool isValidBase58(const std::string& address);

