#pragma once
#include "include.h"

class StateUpdater
{
public:
    /* Constructor and Destructor */
	StateUpdater(Patient *patient);
	virtual ~StateUpdater(void);

    /**
     * performInitialUpdates performs all the state and stats updates upon patient creation
    */
    virtual void performInitialization();
	/**
	 *  performMonthlyUpdates perform all of the state and statistics updates for a simulated month
	 **/
	virtual void performMonthlyUpdates();

    
    /* performInitialization initializes the patients basic state */
    virtual void initializePatient(int patientNum, bool tracingEnabled);
    /* resetMonthNum resets the month number to 0 */
    void resetMonthNum();
    /* setPatientAgeGender set the patients age and gender */
    void setPatientAgeGender(bool gender, int age);
    /* setInitialRenalInv sets the patient's initial renal involvement */
    void updateRenalInv(bool renalInv);
    /* setANCAType sets the patient's ANCA type */
    void setANCAType(bool ancaType);
    /* initializeMutuallyExclusiveState sets the patient's initial mutually exclusive state (Remission, Active, Severe, etc)*/
    void initializeMutuallyExclusiveState(int state);
    /* setPreviousMutuallyExclusiveState sets the patient's initial mutually exclusive state in the PREVIOUS month */
    void setPreviousExclusiveState(int state);
    /* setInactive sets whether the patient is currently experiencing inactive disease (in conjunction with disease state)*/
    void setInactive(bool inactiveDisease);
    /* draw length of time from Active to Inactive state */
    void drawlengthActiveToInactive();
    /* setExitedInitialState sets whether patient has exited the state that they began the simulation in */
    void setExitedInitialState(bool exitedInitialState);
    /* setInitialCovidVaccine sets the patient's initial covid vaccin status */
    void setCovidVaccine(bool covidVacc);
    /* setInitialPriorImmunity sets the patient's initial immunity to COVID from prior infection */
    void setPriorCovidImmunity(bool priorImmunity);
    /* setNumMonthsVaccUpToDate sets number of months the vaccine is up to date for the patient*/
    void setNumMonthsVaccUpToDate(int numMonths);
    /* setVaccUpToDate sets whether the vaccine is up to date */
    void setVaccUpToDate(bool vaccUpToDate);
    /* setCovidPrEPType sets the type of covid PrEP the patient gets put on */
    void setCovidPrEPType(int covidPrEPType);
    /* setCovidTreatment sets the type of covid treatment the patient receives */
    void setCovidTreatment(int covidTreatment);
    /* setMonthCovidVaccine sets the month that the patient was vaccinated for covid */
    void setMonthOfCovidVaccine(int monthCovidVaccine);
    /* setPatientSteroidRegimen sets the patient's steroid regimen (is only called upon if applicable) */
    void setPatientSteroidRegimen(int steroidRegimen);
    /* updateMonthLastRTX updates the month that the patient last had rituximab infusion */
    void updateMonthLastRTX(int monthLastRTX);
    /* updateAliveStatus updates the patient's alive status */
    void setAliveStatus(bool isAlive);
    /* setPrevActive() sets whether the patient has had previous active disease (will be true after the end of the first New Active disease flare)*/
    void setPrevActiveDisease(bool prevActiveDisease);
    /* updateESRDStatus updates the patient's ESRD status*/
    void updateESRDStatus(bool hasESRD);
    /* updateDiabetesStatus updates the patient's Diabetes status */
    void updateDiabetesStatus(bool hasDiabetes);
    /* updateHadMinorRelapse updates whether patient has had minor relapse */
    void updateHadMinorRelapse(bool hadMinorRelapse);
    /* updateHadMajorRelapse updates whether patient has had major relapse */
    void updateHadMajorRelapse(bool hadMajorRelapse);
    /* updateHadSevereInfection updates whether patient has had severe infection */
    void updateHadSevereInfection(bool hadSevereInfection);
    /* updateHadCovid updates whether patient has had covid */
    void updateHadCovid(bool hadCovid);
    /* updateHadCovidDeath updates whether patient had death from covid */
    void updateHadCovidDeath(bool hadCovidDeath);
    /* updateHadCovidHospitalization updates whether patient has had covid hospitalization */
    void updateHadCovidHospitalization(bool hadCovidHospitalization);
    /* updateHadANCARise updates whether patient has had anca rise */
    void updateHadANCARise(bool hadANCARise);
    /* updateHadBCellRise updates whether patient has had b cell rise */
    void updateHadBCellRise(bool hadBCellRise);
    /* updateMonthOfFirstMinorRelapse sets the month of first minor relapse once it happens */
    void updateMonthOfFirstMinorRelapse(int monthOfFirstMinorRelapse);
    /* updateMonthOfFirstMajorRelapse sets the month of first major relapse once it happens */
    void updateMonthOfFirstMajorRelapse(int monthOfFirstMajorRelapse);
    /* updateMonthLastMajorRelapse sets the month of last major relapse. */
    void updateMonthLastMajorRelapse(int monthLastMajorRelapse);
    /* updateMonthLastMinorRelapse sets the month of last minor relapse */
    void updateMonthLastMinorRelapse(int monthLastMinorRelapse);
    /* update majorRelapseFromInactiveDisease boolean */ // DELETE AFTER DEBUGGING?
    void updateMajorRelapseFromInactiveDisease(bool majorRelapseFromInactiveDisease);
    /* incrementMonth updates the simulation month number and patient age */
    void incrementMonth();
    /* updatePatientSurvival updates the patient state for LMs. */
    void updatePatientSurvival();
    /* updateOverallSurvival updates the statistics for LMs */
    void updateOverallSurvival();
    /* setCurrMutuallyExclusiveState sets the current mutually exclusive state */
    void setCurrMutuallyExclusiveState(int state);
    /* setPrevMutuallyExclusiveState sets the previous mutually exclusive state (in the previous month)*/
    void setPrevMutuallyExclusiveState(int prevState);
    /* setTwoMonthsAgoState sets the mutually exclusive state from two months ago */
    void setTwoMonthsAgoState(int twoMonthsAgoState);
    /* Increases the total nuber of persons who have been initialized by one */
    void incrementCohortSize();

protected:
    /** Pointer to the patient state */
    Patient *patient;
    /** Pointer to the simulation context */
    SimContext *simContext;

private:
    /**
	 * \class StateUpdater
	 * The private functions are utility functions that are used by multiple updater functions */

	/* getTimeSummary returns a non-const pointer to the TimeSummary object for the current time period,
		creates a new one if needed or returns null if not keeping longitudinal stats */
	
};