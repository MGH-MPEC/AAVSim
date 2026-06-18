#include "include.h"

/** \brief Constructor takes in the patient object */
InitializePatient::InitializePatient(Patient* patient) : StateUpdater(patient) {

}

/* \brief Destructor is empty, no cleanup required */
InitializePatient::~InitializePatient(void) {

}

/** \brief performInitialization performs all of the state and statistics updates upon patient creation */
void InitializePatient::performInitialization(int patientNum) {
    StateUpdater::performInitialization();

    bool tracingEnabled = false;
    if (patientNum < 100) {
        tracingEnabled = true;
    }
    initializePatient(patientNum, tracingEnabled);
    resetMonthNum();
    
    /** Determine the patient's age */
    double ageMean;
    double ageStdDev;
    ageMean = simContext->getCohortParamsInputs()->ageMean;
    ageStdDev = simContext->getCohortParamsInputs()->ageStdDev;
    int age = (int) (AavUtil::getRandomGaussian(ageMean, ageStdDev, 20020, patient));
    if (age < 0) {
        age = 0; }
    else if (age > 100) {
        age = 100; }
    
    /** Determine the patient's gender*/
    double randNum = AavUtil::getRandomDouble();
    bool gender = FEMALE;
    if (randNum < simContext->getCohortParamsInputs()->probMale) {
        gender = MALE;
    }
    /** Set age and gender */
    setPatientAgeGender(gender, age);

    /** rolling for renal involvement */
    randNum = AavUtil::getRandomDouble();
    bool renalInv = NO_RENAL_INV;
    if (randNum < simContext->getCohortParamsInputs()->probRenalInv) {
        renalInv = WITH_RENAL_INV;
    }
    updateRenalInv(renalInv);

    /** rolling for anca type */
    randNum = AavUtil::getRandomDouble();
    bool ancaType = MPO;
    if (randNum < simContext->getCohortParamsInputs()->probPR3) {
        ancaType = PR3;
    }
    setANCAType(ancaType);

    /** setting months from active to inactive to be 0. Will draw each time major relapse occurs in DecisionTree. */
    patient->generalState.lengthActiveToInactive = 0;


    /** rolling for initial exclusive state */
    // vector containing initial cohort odds (obtained from input sheet in Source.cpp)
    float initRemission = simContext->getCohortParamsInputs()->initRemission;
    // initActive is in one value made up of the sum of both Active Relapsing and Active Newly Diagnosed probabilities. 
    float initActive = simContext->getCohortParamsInputs()->initActive[RELAPSE_ACTIVE] + simContext->getCohortParamsInputs()->initActive[NEW_ACTIVE]; 
    float initSevere = simContext->getCohortParamsInputs()->initSevere;
    float initCovid = simContext->getCohortParamsInputs()->initCovid;
    vector<float> initOdds = {initRemission, initActive, initSevere, initCovid};
    randNum = AavUtil::getRandomDouble();
    int state = selectBasedOnOdds(initOdds, randNum);
    initializeMutuallyExclusiveState(state);
    setExitedInitialState(false);

    /** setting initial previous exclusive state (starts at remission state in month 0)*/
    setPreviousExclusiveState(-1); // there is no previous exclusive state at simulation start.
    patient->generalState.twoMonthsAgoState = -1; // there is no two months ago state at simulation start.

    /** setting initial Inactive state to false, since patient will start in either Remission or Active (Relapse) state */
    setInactive(false);

    /* Setting initial previous active disease to be false. Will roll for this in the next conditional. */
    setPrevActiveDisease(false); 
    
    /** If initially in Active state, determine if it's Active Newly Diagnosed or Active Relapsing, and the num months from active to inactive  */
    if (state == ACTIVE) {
        float probActiveRelapsing = simContext->getCohortParamsInputs()->initActive[RELAPSE_ACTIVE]/initActive;
        randNum = AavUtil::getRandomDouble();
        if (randNum < probActiveRelapsing) {
            // set patient active state as active relapsing (not a new diagnosis), prev active to be true
            setPrevActiveDisease(true);
        }
        // Determine time from active to inactive
        drawlengthActiveToInactive();
    }

    /* SETTING INITIAL COVID STATES */

    /** Setting initial covid vaccine status for patient */
    randNum = AavUtil::getRandomDouble();
    float pastCovidInfection = simContext->getCovidInputs()->pastCovidInfection;
    float probCovidVaccine = simContext->getCovidInputs()->probCovidVaccine;
    float probPastInfectionAndVaccine = simContext->getCovidInputs()->probPastInfectionAndVaccine;
    float probVaccineUpToDate = simContext->getCovidInputs()->probVaccineUpToDate;
    float probVaccineUpToDateAndPastInfection = simContext->getCovidInputs()->probVaccineUpToDateAndPastInfection;
    vector<float> initCovidStatsVector = {1 - pastCovidInfection - probCovidVaccine - probPastInfectionAndVaccine - probVaccineUpToDate - 
        probVaccineUpToDateAndPastInfection, pastCovidInfection, probCovidVaccine, probPastInfectionAndVaccine, probVaccineUpToDate, 
        probVaccineUpToDateAndPastInfection};

    int initCovidStat = selectBasedOnOdds(initCovidStatsVector, randNum);
    if (initCovidStat == NO_PAST_INFECTION_OR_VACCINATION) {
        updateHadCovid(false);
        setPriorCovidImmunity(false);
        setCovidVaccine(false);
        setVaccUpToDate(false);
    }
    else if (initCovidStat == PAST_INFECTION_NO_VACCINE) {
        updateHadCovid(true);
        setPriorCovidImmunity(true);
        setCovidVaccine(false);
        setVaccUpToDate(false);
    }
    else if (initCovidStat == VACCINATED_NO_PAST_INFECTION) {
        updateHadCovid(false);
        setPriorCovidImmunity(false);
        setCovidVaccine(true);
        setVaccUpToDate(false);
    }
    else if (initCovidStat == VACCINATED_AND_PAST_INFECTION) {
        updateHadCovid(true);
        setPriorCovidImmunity(true);
        setCovidVaccine(true);
        setVaccUpToDate(false);
    }
    else if (initCovidStat = VACCINE_UP_TO_DATE) {
        updateHadCovid(false);
        setPriorCovidImmunity(false);
        setCovidVaccine(true);
        setVaccUpToDate(true);
    }
    else if (initCovidStat == VACCINE_UP_TO_DATE_PAST_INFECTION) {
        updateHadCovid(true);
        setPriorCovidImmunity(true);
        setCovidVaccine(true);
        setVaccUpToDate(true);
    }

    /** Setting number of months the vaccine is up to date for the patient */
    int vaccineUpToDateMonthsMean = simContext->getCovidInputs()->vaccineUpToDateMonthsMean;
    int vaccineUpToDateMonthsStdDev = simContext->getCovidInputs()->vaccineUpToDateMonthsStdDev;
    float vaccineUpToDateMonthsDist = AavUtil::getRandomGaussian(vaccineUpToDateMonthsMean, vaccineUpToDateMonthsStdDev, 60005, patient);
    int vaccineUpToDateMonths = abs(round(vaccineUpToDateMonthsDist));
    setNumMonthsVaccUpToDate(vaccineUpToDateMonths);

    /** Setting Covid PrEP Type */
    float probPrEPA = simContext->getCovidInputs()->covidPrEPA;
    float probPrEPB = simContext->getCovidInputs()->covidPrEPB;
    float probPrEPC = simContext->getCovidInputs()->covidPrEPC;
    vector<float> PrEPVect = {probPrEPA, probPrEPB, probPrEPC, 1 - probPrEPA - probPrEPB - probPrEPC};
    randNum = AavUtil::getRandomDouble();
    int PrEPType = selectBasedOnOdds(PrEPVect, randNum);
    setCovidPrEPType(PrEPType);
    /** Setting Covid Treatment Type */
    float probTreatmentA = simContext->getCovidInputs()->covidTreatmentA;
    float probTreatmentB = simContext->getCovidInputs()->covidTreatmentB;
    float probTreatmentC = simContext->getCovidInputs()->covidTreatmentC;
    vector<float> treatmentVect = {probTreatmentA, probTreatmentB, probTreatmentC, 1 - probTreatmentA - probTreatmentB - probTreatmentC};
    randNum = AavUtil::getRandomDouble();
    int treatmentType = selectBasedOnOdds(treatmentVect, randNum);
    setCovidTreatment(treatmentType);
    /** Setting month of covid vaccine: starts at 0 for everyone, turns nonzero if vaccination occurs during model */
    setMonthOfCovidVaccine(0);

    /* SETTING TREATMENT STATES */
    /** Setting steroid regimen */
    string steroidRegimenString = simContext->getDiabetesInputs()->steroidRegimen;
    if (steroidRegimenString == "Reduced"){
        setPatientSteroidRegimen(REDUCED);
    }
    else if (steroidRegimenString == "Standard"){
        setPatientSteroidRegimen(STANDARD);
    }
    else if (steroidRegimenString == "Observational"){
        setPatientSteroidRegimen(OBSERVATIONAL);
    }
    else {
        setPatientSteroidRegimen(NO_STEROID);
    }
    /** Setting monthLastRTX. Initialize to -6 due to a quick of the model: since b cell rise and anca rise don't happen until monthNum - monthLastRTX is greater than or equal to 6. Once rtx is given, this is set to the monthNum it is given.*/
    updateMonthLastRTX(-6);

    /* SETTING DISEASE STATES */
    /** Setting alive status (all patients alive on start )*/
    setAliveStatus(true);
    /** Setting whether patient has ESRD initially */
    float probESRD = simContext->getCohortParamsInputs()->initESRD;
    randNum = AavUtil::getRandomDouble();
    updateESRDStatus(false);
    if (randNum < probESRD) {
        updateESRDStatus(true);
    }
    /** Setting whether patient has Diabetes initially */
    float probDiabetes = simContext->getCohortParamsInputs()->initDiabetes;
    randNum = AavUtil::getRandomDouble();
    updateDiabetesStatus(false);
    if (randNum < probDiabetes) {
        updateDiabetesStatus(true);
    }

    /* SETTING INITIAL MONITORING STATE VARIABLES */
    updateHadMinorRelapse(false);
    updateHadMajorRelapse(false);
    patient->monitoringState.hadMinorRelapseFromRemission = false;
    patient->monitoringState.hadMajorRelapseFromRemission = false;
    patient->monitoringState.hadMinorRelapseFromInactiveDisease = false;
    patient->monitoringState.hadMajorRelapseFromInactiveDisease = false;
    updateHadSevereInfection(false);
    // updateHadCovid(false); RE ADD AFTER FIXING covidInfectionProbColumn functions in patient.cpp
    updateHadCovidDeath(false);
    updateHadCovidHospitalization(false);
    updateHadANCARise(false);
    updateHadBCellRise(false);
    updateMonthOfFirstMinorRelapse(0);
    updateMonthOfFirstMajorRelapse(0);
    updateMonthLastMajorRelapse(-10000);
    if (state == ACTIVE) {
        updateMonthLastMajorRelapse(0);
    }
    updateMonthLastMinorRelapse(-10000);
    updateMajorRelapseFromInactiveDisease(false);
    patient->monitoringState.numMonthsSpentActive = 0;
    patient->monitoringState.numMonthsSpentInactive = 0;
}