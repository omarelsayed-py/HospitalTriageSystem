#ifndef WARD_H
#define WARD_H

#include <string>
#include <vector>
#include "Patient.h"

struct Ward {
    std::string name;
    int totalBeds;
    int occupiedBeds;
};

void initializeWards(std::vector<Ward>& wards);
void displayWardOccupancy(const std::vector<Ward>& wards);

// Return true on success, false if the operation was rejected (invalid ID,
// wrong status, full ward, etc.).
bool assignBed(std::vector<Patient>& patients, std::vector<Ward>& wards);
bool dischargePatient(std::vector<Patient>& patients, std::vector<Ward>& wards);

void saveWards(const std::vector<Ward>& wards);
void loadWards(std::vector<Ward>& wards);

#endif