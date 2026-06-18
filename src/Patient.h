#pragma once

#include "include.h"

class Patient
{
public:
    /** \brief Make the StateUpdter class a friend class so it can modify the private data */
    friend class StateUpdater;

    /* Constructor and Destructor */
    Patient(SimContext *simContext, int patientNum, int _ageMonths = 0, int _gender = 0, int startingMonth = 0);
    ~Patient(void);

    /** GeneralState class holds information about the patients initial characteristics
		(and eventually total costs) and survival numbers */
    class GeneralState {
    public:
        /** Unique patient identifier */
        int patientNum;
        /** True if this patient is traced in the output trace file*/
        bool tracingEnabled;
        /** Current simulation month for patient */
        int monthNum;
        /** The patient's current age in months*/
        int ageMonths;
        /** The patient's curent age in years */
        int age;
        int gender;
        /** Whether patient has renal involvement in disease */
        bool renalInv;
        /** The patient's ANCA type */
        bool ancaType;
        /** The length of the patient's time from active to inactive state (in months) */
        int lengthActiveToInactive;
        /** The patient's current mutually exclusive state */
        int state;
        /** The patient's previous month's mutually exclusive state */
        int prevState;
        /** The patient's state two months ago */
        int twoMonthsAgoState;
        /** WHether patient is currently in Inactive state (in addition to being in disease state) */
        bool inactiveDisease;
        /** The patient's initial state */
        int initialState;
        /** Whether patient has ever exited initial state */
        bool exitedInitialState;
        
    };

    class CovidState {
    public:
        /** Whether patient has prior immunity to Covid*/
        bool priorImmunity;
        /** Whether patient has covid vaccine */
        bool covidVacc;
        /** The number of months the vaccine is up to date for the patient */
        int numMonthsVaccUpToDate;
        /** Whether vaccine is up to date */
        bool vaccUpToDate;
        /** Covid PrEP that the patient is put on */
        int covidPrEPType;
        /** Covid treatment that the patient is put on if they get Covid-19 infection */
        int covidTreatment;
        /** Month of Covid Vaccination */
        int monthCovidVaccine;
    };
    
    class Treatment {
    public:
        /** The steroid regimen that patient is put on. */
        int steroidRegimen;
        /** Month of last RTX injection*/
        int monthLastRTX;
    };

    class DiseaseState {
    public:
        /** True if patient is alive */
        bool isAlive;
        /** True if patient has had active disease before */
        bool prevActiveDisease;
        /** True if patient has ESRD */
        bool hasESRD;
        /** True if patient has diabetes */
        bool hasDiabetes;
    };

    class MonitoringState {
    public:
        /** True if patient has had minor relapse */
        bool hadMinorRelapse;
        /** True if patient has had major relapse */
        bool hadMajorRelapse;
        /** True if patient has had minor relapse from Remission */
        bool hadMinorRelapseFromRemission;
        /** True if patient has had major relapse from Remission */
        bool hadMajorRelapseFromRemission;
        /** True if patient has had minor relapse from Inactive */
        bool hadMinorRelapseFromInactiveDisease;
        /** True if patient has had major relapse from Inactive */
        bool hadMajorRelapseFromInactiveDisease;
        /** True if patient has had severe infection */
        bool hadSevereInfection;
        /** True if patient has had covid */
        bool hadCovid;
        /** True if patient had death from covid */
        bool hadCovidDeath;
        /** True if patient has had hospitalization due to covid */
        bool hadCovidHospitalization;
        /** True if patient has had anca rise */
        bool hadANCARise;
        /** True if patient has had B cell rise */
        bool hadBCellRise;
        /** Month of first minor relapse */
        int monthOfFirstMinorRelapse;
        /** Month of first major relapse */
        int monthOfFirstMajorRelapse;
        /** Month of last ANCA Rise */
        int monthLastANCARise;
        /** Month of last B Cell Rise */
        int monthLastBCellRise;
        /** Month of last major relapse */
        int monthLastMajorRelapse;
        /** Month of last minor relapse */
        int monthLastMinorRelapse;
        /** major relapse from Inactive this month */
        bool majorRelapseFromInactiveDiseaseThisMonth;
        /** Number of months spent in Active state (not inactive) */
        int numMonthsSpentActive;
        /** Number of months spent in Inactive */
        int numMonthsSpentInactive;
    };

    /* Accessor functions return const pointers to the Patient state subclass objects */
    const GeneralState *getGeneralState();
    const CovidState *getCovidState();
    const Treatment *getTreatment();
    const DiseaseState *getDiseaseState();
    const MonitoringState* getMonitoringState();

    /* simulateMonth runs a single month of simulation for this patient, and updates
        its state and runStats statistics */
    void simulateMonth();
    /* Changes the inputs that the patient uses to determine disease progression -- to be used primarily by the transmission model*/
    void setSimContext(SimContext* newSimContext);
    /* isAlive returns true if the patient is alive, false if dead */
    bool isAlive();


    /** pointer to the simulation context */
    SimContext *simContext;
    /** Patient state subclass object */
    GeneralState generalState;
    /** Patient covid state subclass object */
    CovidState covidState;
    /** Patient treatment subclass object */
    Treatment treatment;
    /** Patient disease state subclass object */
    DiseaseState diseaseState;
    /** Patient state subclass object */
    MonitoringState monitoringState;
    /** Patient initialize patient subclass object */
    InitializePatient initializePatient;
};

/** getGeneralState returns a const pointer to the GeneralState object */
inline const Patient::GeneralState *Patient::getGeneralState() {
    return &generalState;
}

inline const Patient::CovidState *Patient::getCovidState() {
    return &covidState;
}

/** getTreatment returns a const pointer to the Treatment object */
inline const Patient::Treatment *Patient::getTreatment() {
    return &treatment;
}

/** getDiseaseState returns a const pointer to the DiseaseState object */
inline const Patient::DiseaseState *Patient::getDiseaseState() {
    return &diseaseState;
}

/** getMonitoringState returns a const pointer to the MonitoringState object */
inline const Patient::MonitoringState *Patient::getMonitoringState() {
	return &monitoringState;
}

/** isAlive returns true if the patient is alive, false if dead */
inline bool Patient::isAlive() {
    if (diseaseState.isAlive)
        return true;
    return false;
} /* end isAlive */



int covidInfectionProbColumn(Patient *patient);
int covidHospitalizationAndMortalityProbColumn(Patient *patient);