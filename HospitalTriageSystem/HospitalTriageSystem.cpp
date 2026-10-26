#include <iostream>
#include <vector>
#include <limits>
#include <string>

#include "Patient.h"
#include "Ward.h"
#include "Logger.h"

void showReceptionistMenu(std::vector<Patient>& patients, std::vector<Ward>& wards);
void showDoctorMenu(std::vector<Patient>& patients, const std::vector<Ward>& wards);

// Returns the index of the first patient whose status differs between
// "before" and "after", or -1 if no status change occurred.
// Safe to compare by index because assignBed / dischargePatient never reorder
// the vector — they only mutate in place.
static int findFirstStatusChange(const std::vector<Patient>& before,
    const std::vector<Patient>& after) {
    for (std::size_t i = 0; i < after.size() && i < before.size(); ++i) {
        if (before[i].status != after[i].status) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

int main() {
    std::vector<Patient> patients;
    std::vector<Ward> wards;

    loadPatients(patients);
    sortPatientsByPriority(patients);   

    loadWards(wards);
    if (wards.empty()) {
        initializeWards(wards);
    }

    logEvent("System initialized: Wards and patient records loaded.");

    int mainChoice = 0;
    do {
        std::cout << "\n=========================================\n";
        std::cout << "   HOSPITAL TRIAGE & WARD MANAGEMENT     \n";
        std::cout << "=========================================\n";
        std::cout << "1. Receptionist Portal\n";
        std::cout << "2. Doctor / Clinical Portal\n";
        std::cout << "3. Exit System\n";
        std::cout << "Select Role (1-3): ";

        if (!(std::cin >> mainChoice)) {
            std::cout << "[Error] Invalid input. Please enter a number.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        switch (mainChoice) {
        case 1:
            showReceptionistMenu(patients, wards);
            break;
        case 2:
            showDoctorMenu(patients, wards);
            break;
        case 3:
            savePatients(patients);
            saveWards(wards);
            std::cout << "\n[System] Data saved. Shutting down. Audit log saved to 'hospital_audit.txt'.\n";
            logEvent("System shut down. Data persisted successfully.");
            break;
        default:
            std::cout << "[Error] Invalid choice. Please select 1, 2, or 3.\n";
        }
    } while (mainChoice != 3);

    return 0;
}

void showReceptionistMenu(std::vector<Patient>& patients, std::vector<Ward>& wards) {
    int choice = 0;
    do {
        std::cout << "\n--- RECEPTIONIST PORTAL ---\n";
        std::cout << "1. Register New Patient & Triage\n";
        std::cout << "2. View All Patients\n";
        std::cout << "3. Assign Bed to Patient\n";
        std::cout << "4. Discharge Patient\n";
        std::cout << "5. View Ward Occupancy\n";
        std::cout << "6. Back to Main Menu\n";
        std::cout << "Select Option (1-6): ";

        if (!(std::cin >> choice)) {
            std::cout << "[Error] Invalid input.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
        case 1: {
            const std::size_t previousSize = patients.size();
            registerPatient(patients);

            if (patients.size() > previousSize) {
                const int registeredId = patients.back().id;
                sortPatientsByPriority(patients);
                savePatients(patients);
                logEvent("Registered new patient ID: " + std::to_string(registeredId));
            }
            break;
        }
        case 2:
            displayPatients(patients);
            break;
        case 3: {
            // Snapshot state to detect a successful admission.
            // Safe to compare by index — assignBed never reorders the vector.
            const auto beforePatients = patients;

            if (assignBed(patients, wards)) {
                savePatients(patients);
                saveWards(wards);

                int changed = findFirstStatusChange(beforePatients, patients);
                if (changed >= 0 && patients[changed].status == PatientStatus::Admitted) {
                    logEvent("Patient ID " + std::to_string(patients[changed].id) +
                        " admitted to " + patients[changed].assignedWard +
                        " (Bed #" + std::to_string(patients[changed].bedNumber) + ").");
                }
            }
            break;
        }
        case 4: {
            // Snapshot state to detect a successful discharge.
            const auto beforePatients = patients;

            if (dischargePatient(patients, wards)) {
                savePatients(patients);
                saveWards(wards);

                int changed = findFirstStatusChange(beforePatients, patients);
                if (changed >= 0 && patients[changed].status == PatientStatus::Discharged) {
                    logEvent("Patient ID " + std::to_string(patients[changed].id) +
                        " discharged from " + beforePatients[changed].assignedWard + ".");
                }
            }
            break;
        }
        case 5:
            displayWardOccupancy(wards);
            break;
        case 6:
            std::cout << "Returning to main menu...\n";
            break;
        default:
            std::cout << "[Error] Invalid option. Choose between 1 and 6.\n";
        }
    } while (choice != 6);
}

void showDoctorMenu(std::vector<Patient>& patients, const std::vector<Ward>& wards) {
    int choice = 0;
    do {
        std::cout << "\n--- DOCTOR / CLINICAL PORTAL ---\n";
        std::cout << "1. View Active Triage Queue (Sorted by Severity)\n";
        std::cout << "2. View Ward Occupancy & Status\n";
        std::cout << "3. Back to Main Menu\n";
        std::cout << "Select Option (1-3): ";

        if (!(std::cin >> choice)) {
            std::cout << "[Error] Invalid input.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
        case 1:
            std::cout << "\n[Clinical View] Active Patient Queue (Highest Priority First):\n";
            displayPatients(patients);
            logEvent("Doctor viewed active patient triage queue.");
            break;
        case 2:
            displayWardOccupancy(wards);
            logEvent("Doctor viewed ward occupancy reports.");
            break;
        case 3:
            std::cout << "Returning to main menu...\n";
            break;
        default:
            std::cout << "[Error] Invalid option. Choose between 1 and 3.\n";
        }
    } while (choice != 3);
}