#include "Ward.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <limits>

void initializeWards(std::vector<Ward>& wards) {
    wards.clear();
    wards.push_back({ "ICU", 10, 0 });
    wards.push_back({ "General", 30, 0 });
    wards.push_back({ "Emergency", 8, 0 });
}

void displayWardOccupancy(const std::vector<Ward>& wards) {
    std::cout << "\n========== WARD OCCUPANCY ==========\n";
    std::cout << std::left
        << std::setw(15) << "Ward"
        << std::setw(15) << "Total Beds"
        << std::setw(15) << "Occupied"
        << std::setw(15) << "Available"
        << '\n';
    std::cout << std::string(60, '-') << '\n';

    for (const Ward& ward : wards) {
        int availableBeds = ward.totalBeds - ward.occupiedBeds;
        std::cout << std::left
            << std::setw(15) << ward.name
            << std::setw(15) << ward.totalBeds
            << std::setw(15) << ward.occupiedBeds
            << std::setw(15) << availableBeds
            << '\n';
    }
    std::cout << std::string(60, '=') << '\n';
}

bool assignBed(std::vector<Patient>& patients, std::vector<Ward>& wards) {
    int patientId;
    std::cout << "\n--- Assign Bed to Patient ---\n";
    std::cout << "Enter Patient ID: ";
    if (!(std::cin >> patientId)) {
        std::cout << "[Error] Invalid input.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return false;
    }

    int patientIndex = findPatientById(patients, patientId);
    if (patientIndex == -1) {
        std::cout << "[Error] Patient with ID " << patientId << " not found.\n";
        return false;
    }

    Patient& p = patients[patientIndex];

    if (p.status == PatientStatus::Admitted) {
        std::cout << "[Error] Patient " << p.name << " is already admitted to "
            << p.assignedWard << " (Bed #" << p.bedNumber << ").\n";
        return false;
    }

    if (p.status == PatientStatus::Discharged) {
        std::cout << "[Error] Patient " << p.name
            << " has already been discharged and cannot be re-admitted directly.\n";
        return false;
    }

    std::cout << "\nAvailable Wards:\n";
    for (size_t i = 0; i < wards.size(); ++i) {
        int available = wards[i].totalBeds - wards[i].occupiedBeds;
        std::cout << i + 1 << ". " << wards[i].name
            << " (Available Beds: " << available << "/" << wards[i].totalBeds << ")\n";
    }

    int wardChoice;
    std::cout << "Select Ward (1-" << wards.size() << "): ";
    if (!(std::cin >> wardChoice) || wardChoice < 1 || wardChoice > static_cast<int>(wards.size())) {
        std::cout << "[Error] Invalid ward selection.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return false;
    }

    Ward& selectedWard = wards[wardChoice - 1];

    if (selectedWard.occupiedBeds >= selectedWard.totalBeds) {
        std::cout << "[Error] The " << selectedWard.name
            << " ward is currently at full capacity!\n";
        return false;
    }

    selectedWard.occupiedBeds++;
    p.status = PatientStatus::Admitted;
    p.assignedWard = selectedWard.name;
    p.bedNumber = selectedWard.occupiedBeds;

    std::cout << "\n[Success] Patient " << p.name << " admitted to "
        << p.assignedWard << " (Bed #" << p.bedNumber << ").\n";
    return true;
}

bool dischargePatient(std::vector<Patient>& patients, std::vector<Ward>& wards) {
    int patientId;
    std::cout << "\n--- Discharge Patient ---\n";
    std::cout << "Enter Patient ID: ";
    if (!(std::cin >> patientId)) {
        std::cout << "[Error] Invalid input.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return false;
    }

    int patientIndex = findPatientById(patients, patientId);
    if (patientIndex == -1) {
        std::cout << "[Error] Patient with ID " << patientId << " not found.\n";
        return false;
    }

    Patient& p = patients[patientIndex];

    if (p.status != PatientStatus::Admitted) {
        std::cout << "[Error] Patient " << p.name
            << " is not currently admitted to any ward.\n";
        return false;
    }

    for (Ward& w : wards) {
        if (w.name == p.assignedWard) {
            if (w.occupiedBeds > 0) {
                w.occupiedBeds--;
            }
            break;
        }
    }

    p.status = PatientStatus::Discharged;
    std::cout << "\n[Success] Patient " << p.name
        << " has been discharged from " << p.assignedWard << ". Bed released.\n";
    return true;
}

void saveWards(const std::vector<Ward>& wards) {
    std::ofstream file("wards.txt");

    if (!file.is_open()) {
        std::cerr << "[Error] Could not open wards.txt for writing.\n";
        return;
    }

    for (const Ward& ward : wards) {
        file << ward.name << '\n';
        file << ward.totalBeds << '\n';
        file << ward.occupiedBeds << '\n';

        if (!file) {
            std::cerr << "[Error] Failed while writing ward records.\n";
            return;
        }
    }

    file.close();

    if (file.fail()) {
        std::cerr << "[Error] Failed to close wards.txt properly.\n";
        return;
    }

    std::cout << "[System] Ward records saved successfully.\n";
}

void loadWards(std::vector<Ward>& wards) {
    std::ifstream file("wards.txt");

    if (!file.is_open()) {
        initializeWards(wards);
        return;
    }

    std::vector<Ward> loadedWards;
    Ward ward;

    while (std::getline(file, ward.name)) {
        if (!(file >> ward.totalBeds >> ward.occupiedBeds)) {
            std::cerr << "[Warning] Incomplete ward record found.\n";
            break;
        }

        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (ward.name.empty() ||
            ward.totalBeds < 0 ||
            ward.occupiedBeds < 0 ||
            ward.occupiedBeds > ward.totalBeds) {

            std::cerr << "[Warning] Invalid ward record skipped.\n";
            continue;
        }

        loadedWards.push_back(ward);
    }

    if (!file.eof() && file.fail()) {
        std::cerr << "[Warning] Error occurred while reading wards.txt.\n";
    }

    wards = std::move(loadedWards);
    std::cout << "[System] Loaded " << wards.size() << " ward record(s).\n";
}