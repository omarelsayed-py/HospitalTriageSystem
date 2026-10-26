#include "Logger.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>

void logEvent(const std::string& eventDescription) {
    std::ofstream auditFile("hospital_audit.txt", std::ios::app);

    if (!auditFile.is_open()) {
        std::cerr << "[Error] Failed to open hospital_audit.txt for writing.\n";
        return;
    }

    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    std::tm localTm{};
#if defined(_WIN32)
    localtime_s(&localTm, &currentTime);
#else
    localtime_r(&currentTime, &localTm);
#endif

    auditFile << "[" << std::put_time(&localTm, "%Y-%m-%d %H:%M:%S") << "] "
        << eventDescription << "\n";
    auditFile.close();
}