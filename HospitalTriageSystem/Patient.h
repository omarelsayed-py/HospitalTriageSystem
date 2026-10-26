#ifndef PATIENT_H
#define PATIENT_H

#include <string>
#include <vector>

// The urgency assigned to a patient.
enum class TriagePriority {
    Critical = 1,
    Urgent = 2,
    Stable = 3
};

// The patient's current state in the hospital.
enum class PatientStatus {
    Waiting,
    Admitted,
    Discharged
};

// Represents one patient record.
struct Patient {
    int id;
    std::string name;
    int age;
    std::string symptomCode;

    TriagePriority priority;
    PatientStatus status;

    // Tracking fields for ward management
    std::string assignedWard = "None";
    int bedNumber = -1;
};

// Patient management function declarations.
void registerPatient(std::vector<Patient>& patients);
void displayPatients(const std::vector<Patient>& patients);
int findPatientById(const std::vector<Patient>& patients, int id);
void sortPatientsByPriority(std::vector<Patient>& patients);
void savePatients(const std::vector<Patient>& patients);
void loadPatients(std::vector<Patient>& patients);

#endif