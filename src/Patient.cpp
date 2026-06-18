#include "include.h"

// Disable warning for unsafe use of this pointer in initialization,
//	state updater constructor only copies the pointer and does not access any of its fields so it is safe
#pragma warning(disable:4355)

/** Constructor takes in the patient number, simulation context, run stats object, and tracing object
	Initializes all subclass state values */

Patient::Patient(SimContext *simContext, int patientNum, int _ageMonths, int _gender, int startingMonth) :
        simContext(simContext),
        initializePatient(this)
{
    initializePatient.performInitialization(patientNum);

}

Patient::~Patient(void) {
    
}
    
int covidInfectionProbColumn(Patient *patient){
    if (patient->getCovidState()->covidPrEPType < 3) {
        if (patient->getCovidState()->covidPrEPType == PrEP_A) {
            return PrEP_Option_A;
        }
        else if (patient->getCovidState()->covidPrEPType == PrEP_B) {
            return PrEP_Option_B;
        }
        else {
            return PrEP_Option_C;
        }
    }
    else if (patient->getCovidState()->vaccUpToDate) {
        return Vaccine_Up_To_Date;
    }
    else if (patient->getCovidState()->covidVacc && patient->getMonitoringState()->hadCovid) {
        return Vaccinated_Past_infection;
    }
    else if (patient->getCovidState()->covidVacc) {
        return Vaccinated;
    }
    else if (patient->getMonitoringState()->hadCovid) {
        return Past_Infection;
    }
    else {
        return Non_Immune;
    }
}

int covidHospitalizationAndMortalityProbColumn(Patient *patient) {
    if (patient->getCovidState()->covidTreatment < 3) {
        if (patient->getCovidState()->covidTreatment == Treatment_A) {
            return Treatment_Option_A;
        }
        else if (patient->getCovidState()->covidTreatment == Treatment_B) {
            return Treatment_Option_B;
        }
        else {
            return Treatment_Option_C;
        }
    }
    else if (patient->getCovidState()->covidPrEPType < 3) {
        if (patient->getCovidState()->covidPrEPType == PrEP_A) {
            return PrEP_Option_A;
        }
        else if (patient->getCovidState()->covidPrEPType == PrEP_B) {
            return PrEP_Option_B;
        }
        else {
            return PrEP_Option_C;
        }
    }
    else if (patient->getCovidState()->vaccUpToDate) {
        return Vaccine_Up_To_Date;
    }
    else if (patient->getCovidState()->covidVacc && patient->getMonitoringState()->hadCovid) { //CHANGE TO priorImmunity. makes more sense
        return Vaccinated_Past_infection;
    }
    else if (patient->getCovidState()->covidVacc) {
        return Vaccinated;
    }
    else if (patient->getMonitoringState()->hadCovid) {//CHANGE TO priorImmunity. makes more sense
        return Past_Infection;
    }
    else {
        return Non_Immune;
    }
}