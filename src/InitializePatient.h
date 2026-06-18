#pragma once

#include "include.h"

class InitializePatient : public StateUpdater {
public:
    InitializePatient(Patient *patient);
    ~InitializePatient(void);

    /* performInitialization perform all of the state and statistics updates upon patient creation */
    void performInitialization(int patientNum);
};