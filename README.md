# 🏥 Hospital Patient Triage & Ward Management System

A robust, production-grade C++ terminal application designed to streamline emergency room triage, patient intake, clinical status tracking, and hospital bed management. Built with strict data validation, stream safety, and automated audit logging.

---

## ✨ Key Features

- **Role-Based Portals:**
  - **Receptionist Portal:** Register new patients, perform triage, assign/discharge beds, and monitor ward capacities.
  - **Doctor / Clinical Portal:** View active, severity-sorted patient queues and real-time ward occupancy reports.
- **Smart Triage & Sorting:** Automatically organizes patients by severity (`Critical = 1`, `Urgent = 2`, `Stable = 3`) using modern C++ sorting algorithms.
- **Robust Input Sanitization:** Crash-proof input loops handling type mismatches, whitespace-only names, negative ages, and duplicate IDs.
- **Persistent State Storage:** Automatic data persistence to `patients.txt` and `wards.txt` with defensive boundary checks and transactional vector loading.
- **Automated Audit Trails:** Timestamped clinical logging into `hospital_audit.txt` for compliance and system tracking.

---

## 🛠️ Tech Stack & Architecture

- **Language:** C++ (Standard: C++11 / C++14 / C++17)
- **Data Structures:** `std::vector` for contiguous memory storage and random-access performance.
- **Type Safety:** Strongly-typed `enum class` for triage priorities and patient statuses.
- **Modularity:** Clean header-source separation (`Patient.h/.cpp`, `Ward.h/.cpp`, `Logger.h/.cpp`, `main.cpp`).

---

## 🚀 Getting Started & Compilation

### Prerequisites
You need a C++ compiler supporting C++11 or higher (e.g., GCC/G++, Clang, or Microsoft Visual Studio).

### Compilation (using GCC via Terminal)
```bash
g++ main.cpp Patient.cpp Ward.cpp Logger.cpp -o HospitalSystem
```

### Running the Application
```bash
./HospitalSystem
```
*(On Windows via Visual Studio, simply open the solution file and press `Ctrl + F5` to build and run).*

---

## 📋 Testing & Verification Suite

1. **Clean Boot:** Start the application with no data files present. Default wards (ICU, General, Emergency) will initialize automatically.
2. **Registration & Triage:** Register patients across different priority tiers and observe them automatically sort to the top.
3. **Bed Allocation:** Assign patients to available ward beds and verify that occupancy counts update dynamically.
4. **State Persistence:** Exit the application, reopen it, and confirm that all patient queues and ward states successfully reload from disk.
5. **Audit Logs:** Check `hospital_audit.txt` to review timestamped historical activity.
