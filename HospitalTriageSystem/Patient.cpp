#include "Patient.h"
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <cctype>
#include <fstream>

void savePatients(const std::vector<Patient>& patients)
{
    std::ofstream file("patients.txt");

    if (!file.is_open()) {
        std::cerr << "[Error] Could not open patients.txt for writing.\n";
        return;
    }

    for (const Patient& patient : patients) {
        file << patient.id << '\n';
        file << patient.name << '\n';
        file << patient.age << '\n';
        file << patient.symptomCode << '\n';
        file << static_cast<int>(patient.priority) << '\n';
        file << static_cast<int>(patient.status) << '\n';
        file << patient.assignedWard << '\n';
        file << patient.bedNumber << '\n';

        if (!file) {
            std::cerr << "[Error] Failed while writing patient records.\n";
            return;
        }
    }

    file.close();

    if (file.fail()) {
        std::cerr << "[Error] Failed to close patients.txt properly.\n";
        return;
    }

    std::cout << "[System] Patient records saved successfully.\n";
}

// Load all patient records from patients.txt with strict validation
void loadPatients(std::vector<Patient>& patients)
{
    std::ifstream file("patients.txt");

    if (!file.is_open()) {
        return; // No saved file yet; start with an empty patient list.
    }

    std::vector<Patient> loadedPatients;
    Patient p;
    int priorityInt;
    int statusInt;

    while (file >> p.id) {
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (!std::getline(file, p.name)) {
            std::cerr << "[Warning] Incomplete patient record found.\n";
            break;
        }

        if (!(file >> p.age)) {
            std::cerr << "[Warning] Invalid patient age in file.\n";
            break;
        }

        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (!std::getline(file, p.symptomCode)) {
            std::cerr << "[Warning] Missing symptom code.\n";
            break;
        }

        if (!(file >> priorityInt >> statusInt)) {
            std::cerr << "[Warning] Invalid priority or status.\n";
            break;
        }

        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (!std::getline(file, p.assignedWard)) {
            std::cerr << "[Warning] Missing assigned ward.\n";
            break;
        }

        if (!(file >> p.bedNumber)) {
            std::cerr << "[Warning] Invalid bed number.\n";
            break;
        }

        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // Validate the record before adding it
        if (p.id <= 0 ||
            p.age < 0 || p.age > 130 ||
            priorityInt < 1 || priorityInt > 3 ||
            statusInt < 0 || statusInt > 2) {

            std::cerr << "[Warning] Invalid patient record skipped.\n";
            continue;
        }

        p.priority = static_cast<TriagePriority>(priorityInt);
        p.status = static_cast<PatientStatus>(statusInt);

        loadedPatients.push_back(p);
    }

    // Replace existing data only after successful validation load
    patients = std::move(loadedPatients);

    std::cout << "[System] Loaded " << patients.size() << " patient record(s).\n";
}

// Register a new patient with full input validation, type safety, and non-empty checks
void registerPatient(std::vector<Patient>& patients) {
    Patient newPatient;
    std::cout << "\n--- Register New Patient ---\n";

    while (true) {
        std::cout << "Enter Patient ID: ";
        if (!(std::cin >> newPatient.id)) {
            std::cout << "[Error] Invalid input. Please enter a valid integer for the ID.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if (findPatientById(patients, newPatient.id) != -1) {
            std::cout << "[Error] A patient with ID " << newPatient.id << " already exists. Please use a unique ID.\n";
        }
        else {
            break;
        }
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    while (true) {
        std::cout << "Enter Full Name: ";
        std::getline(std::cin, newPatient.name);

        bool hasChar = false;
        for (char c : newPatient.name) {
            if (!std::isspace(static_cast<unsigned char>(c))) {
                hasChar = true;
                break;
            }
        }

        if (hasChar) {
            break;
        }
        std::cout << "[Error] Name cannot be empty or contain only whitespace.\n";
    }

    while (true) {
        std::cout << "Enter Age (0-130): ";
        if (!(std::cin >> newPatient.age)) {
            std::cout << "[Error] Invalid input. Please enter a valid integer for the age.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if (newPatient.age >= 0 && newPatient.age <= 130) {
            break;
        }
        std::cout << "[Error] Invalid age. Please enter a realistic age between 0 and 130.\n";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    while (true) {
        std::cout << "Enter Symptom Code (e.g., CHEST_PAIN, FRACTURE): ";
        std::getline(std::cin, newPatient.symptomCode);

        bool hasChar = false;
        for (char c : newPatient.symptomCode) {
            if (!std::isspace(static_cast<unsigned char>(c))) {
                hasChar = true;
                break;
            }
        }

        if (hasChar) {
            break;
        }
        std::cout << "[Error] Symptom code cannot be empty or contain only whitespace.\n";
    }

    int priorityInput;
    while (true) {
        std::cout << "Select Priority Level (1 = Critical/Red, 2 = Urgent/Yellow, 3 = Stable/Green): ";
        if (!(std::cin >> priorityInput)) {
            std::cout << "[Error] Invalid input. Please enter 1, 2, or 3.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if (priorityInput == 1) {
            newPatient.priority = TriagePriority::Critical;
            break;
        }
        else if (priorityInput == 2) {
            newPatient.priority = TriagePriority::Urgent;
            break;
        }
        else if (priorityInput == 3) {
            newPatient.priority = TriagePriority::Stable;
            break;
        }
        else {
            std::cout << "[Error] Invalid choice. Please enter 1, 2, or 3.\n";
        }
    }

    newPatient.status = PatientStatus::Waiting;
    patients.push_back(newPatient);

    std::cout << "\n[Success] Patient registered successfully and placed in the queue!\n";
}

void displayPatients(const std::vector<Patient>& patients) {
    if (patients.empty()) {
        std::cout << "\n[Info] No patients currently registered.\n";
        return;
    }

    std::cout << "\n=================================================================\n";
    std::cout << "                        PATIENT QUEUE                            \n";
    std::cout << "=================================================================\n";
    std::cout << std::left
        << std::setw(8) << "ID"
        << std::setw(20) << "Name"
        << std::setw(6) << "Age"
        << std::setw(15) << "Symptom"
        << std::setw(12) << "Priority"
        << std::setw(12) << "Status"
        << '\n';
    std::cout << std::string(65, '-') << '\n';

    for (const Patient& p : patients) {
        std::string priorityStr;
        switch (p.priority) {
        case TriagePriority::Critical: priorityStr = "Critical"; break;
        case TriagePriority::Urgent:   priorityStr = "Urgent"; break;
        case TriagePriority::Stable:   priorityStr = "Stable"; break;
        }

        std::string statusStr;
        switch (p.status) {
        case PatientStatus::Waiting:    statusStr = "Waiting"; break;
        case PatientStatus::Admitted:   statusStr = "Admitted"; break;
        case PatientStatus::Discharged: statusStr = "Discharged"; break;
        }

        std::cout << std::left
            << std::setw(8) << p.id
            << std::setw(20) << p.name
            << std::setw(6) << p.age
            << std::setw(15) << p.symptomCode
            << std::setw(12) << priorityStr
            << std::setw(12) << statusStr
            << '\n';
    }
    std::cout << std::string(65, '=') << '\n';
}

int findPatientById(const std::vector<Patient>& patients, int id) {
    for (size_t i = 0; i < patients.size(); ++i) {
        if (patients[i].id == id) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void sortPatientsByPriority(std::vector<Patient>& patients) {
    std::sort(patients.begin(), patients.end(), [](const Patient& a, const Patient& b) {
        return static_cast<int>(a.priority) < static_cast<int>(b.priority);
        });
}