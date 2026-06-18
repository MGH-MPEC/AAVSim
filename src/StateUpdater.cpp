#include "include.h"

/** Constructor takes in the patient object */
StateUpdater::StateUpdater(Patient *patient) :
		patient(patient)
{

}

/** Destructor is empty, no cleanup required */
StateUpdater::~StateUpdater(void) {

}
/**
 * \brief Virtual function to perform the initial updates upon patient creation.
 * Sets the simContext, and tracer to match that of this->patient */
void StateUpdater::performInitialization() {
	// Copy the pointers to the simContext and tracer objects
	this->simContext = this->patient->simContext;
}
/**
 * \brief Virtual function to perform all updates for a simulated month.
 * Empty for now, no actions to perform if child does not override
 * */
void StateUpdater::performMonthlyUpdates() {
	// Empty for now, no actions to perform if child does not override
}

/** \brief performInitialization initializes the patient's basic state
 * \param patientNum an integer argument that assigns the patient a (hopefully unique!) identifier
 * \param tracingEnabled a bool argument that is true if this patient is to be output in the trace file
 *
 * This function initializes patient->monthNum to the initial month number (0 unless otherwise specified by the transmission model),
 * all costs to 0, all life months (LMs) and discounted LMs to 0, the discount factor to 1, and marks the patient as alive */
void StateUpdater::initializePatient(int patientNum, bool tracingEnabled) {
    patient -> generalState.patientNum = patientNum;
    patient -> generalState.tracingEnabled = tracingEnabled;
}

void StateUpdater::resetMonthNum() {
	patient -> generalState.monthNum = 0;
}
void StateUpdater::setPatientAgeGender(bool gender, int age) {
	patient->generalState.gender = gender;
	patient->generalState.age = age;
	patient->generalState.ageMonths = age * 12;
}

void StateUpdater::updateRenalInv(bool renalInv) {
	patient->generalState.renalInv = renalInv;
}

void StateUpdater::setPrevActiveDisease(bool prevActiveDisease) {
	patient->diseaseState.prevActiveDisease = prevActiveDisease;
}

void StateUpdater::updateESRDStatus(bool hasESRD) {
	patient -> diseaseState.hasESRD = hasESRD;
}

void StateUpdater::updateDiabetesStatus(bool hasDiabetes) {
	patient -> diseaseState.hasDiabetes = hasDiabetes;
}

void StateUpdater::setANCAType(bool ancaType) {
	patient->generalState.ancaType = ancaType;
}

void StateUpdater::initializeMutuallyExclusiveState(int state) {
	patient->generalState.state = state;
	patient->generalState.initialState = state;
}

void StateUpdater::setPreviousExclusiveState(int prevState) {
	patient->generalState.prevState = prevState;
}

void StateUpdater::setInactive(bool inactiveDisease) {
	if (inactiveDisease == true && patient->getGeneralState()->state != ACTIVE) {
		cout << "WARNING: TRYING TO SET patient->generalState.inactiveDisease TO TRUE EVEN THOUGH NOT IN ACTIVE STATE. CHECK WHERE setInactive() is called" << endl;
	}
	else {
		patient->generalState.inactiveDisease = inactiveDisease;
	}
}

void StateUpdater::drawlengthActiveToInactive() {
	int drawnLength;
	float randNum = AavUtil::getRandomDouble();
	vector<float> lengthActiveToInactive;
	for (int i = 0; i < simContext->getCohortParamsInputs()->lengthActiveToInactiveProbabilities.size(); i++) {
		lengthActiveToInactive.push_back(simContext->getCohortParamsInputs()->lengthActiveToInactiveProbabilities[i]);
	}
	drawnLength = selectBasedOnOdds(lengthActiveToInactive, randNum) + 1;
	patient->generalState.lengthActiveToInactive = drawnLength;
	//cout << drawnLength << endl;cin.get();
}

void StateUpdater::setExitedInitialState(bool exitedInitialState) {
	patient->generalState.exitedInitialState = exitedInitialState;
}

void StateUpdater::incrementMonth() {
	patient->generalState.monthNum++;
	patient->generalState.ageMonths++;
	patient->generalState.age = (int)((patient->generalState.ageMonths)/12);

}

void StateUpdater::setPriorCovidImmunity(bool priorImmunity) {
	patient->covidState.priorImmunity = priorImmunity;
}

void StateUpdater::setCovidVaccine(bool covidVacc) {
	patient->covidState.covidVacc = covidVacc;
}
void StateUpdater::setNumMonthsVaccUpToDate(int numMonthsVaccUpToDate) {
	patient->covidState.numMonthsVaccUpToDate = numMonthsVaccUpToDate;
}
void StateUpdater::setVaccUpToDate(bool vaccUpToDate) {
	patient->covidState.vaccUpToDate = vaccUpToDate;
}
void StateUpdater::setCovidPrEPType(int covidPrEPType) {
	patient->covidState.covidPrEPType = covidPrEPType;
}
void StateUpdater::setCovidTreatment(int covidTreatment) {
	patient->covidState.covidTreatment = covidTreatment;
}
void StateUpdater::setMonthOfCovidVaccine(int monthCovidVaccine) {
	patient->covidState.monthCovidVaccine = monthCovidVaccine;
}
void StateUpdater::setPatientSteroidRegimen(int steroidRegimen) {
	patient->treatment.steroidRegimen = steroidRegimen;
}
void StateUpdater::updateMonthLastRTX(int monthLastRTX) {
	patient->treatment.monthLastRTX = monthLastRTX;
}
void StateUpdater::setAliveStatus(bool isAlive) {
	patient->diseaseState.isAlive = isAlive;
}
void StateUpdater::updateHadMinorRelapse(bool hadMinorRelapse) {
	patient->monitoringState.hadMinorRelapse = hadMinorRelapse;
}
void StateUpdater::updateHadMajorRelapse(bool hadMajorRelapse) {
	patient->monitoringState.hadMajorRelapse = hadMajorRelapse;
}
void StateUpdater::updateHadSevereInfection(bool hadSevereInfection) {
	patient->monitoringState.hadSevereInfection = hadSevereInfection;
}
void StateUpdater::updateHadCovid(bool hadCovid) {
	patient->monitoringState.hadCovid = hadCovid;
}
void StateUpdater::updateHadCovidDeath(bool hadCovidDeath) {
	patient->monitoringState.hadCovidDeath = hadCovidDeath;
}
void StateUpdater::updateHadCovidHospitalization(bool hadCovidHospitalization) {
	patient->monitoringState.hadCovidHospitalization = hadCovidHospitalization;
}
void StateUpdater::updateHadANCARise(bool hadANCARise) {
	patient->monitoringState.hadANCARise = hadANCARise;
}
void StateUpdater::updateHadBCellRise(bool hadBCellRise) {
	patient->monitoringState.hadBCellRise = hadBCellRise;
}
void StateUpdater::updateMonthOfFirstMinorRelapse(int monthOfFirstMinorRelapse) {
	patient->monitoringState.monthOfFirstMinorRelapse = monthOfFirstMinorRelapse;
}
void StateUpdater::updateMonthOfFirstMajorRelapse(int monthOfFirstMajorRelapse) {
	patient->monitoringState.monthOfFirstMajorRelapse = monthOfFirstMajorRelapse;
}
void StateUpdater::updateMonthLastMajorRelapse(int monthLastMajorRelapse) {
	patient->monitoringState.monthLastMajorRelapse = monthLastMajorRelapse;
}
void StateUpdater::updateMonthLastMinorRelapse(int monthLastMinorRelapse) {
	patient->monitoringState.monthLastMinorRelapse = monthLastMinorRelapse;
}
void StateUpdater::updateMajorRelapseFromInactiveDisease(bool majorRelapseFromInactiveDisease) {
	patient->monitoringState.majorRelapseFromInactiveDiseaseThisMonth = majorRelapseFromInactiveDisease;
}
void StateUpdater::setCurrMutuallyExclusiveState(int state) {
	patient->generalState.state = state;
}
void StateUpdater::setPrevMutuallyExclusiveState(int prevState) {
	patient->generalState.prevState = prevState;
}
void StateUpdater::setTwoMonthsAgoState(int twoMonthsAgoState) {
	patient->generalState.twoMonthsAgoState = twoMonthsAgoState;
}

/** \brief incrementCohortSize increments the total population size by one
 *
 * This is done at patient creation instead of patient death (with the other population statistics)
 * for the sake of the transmission model: since the transmission model runs multiple patients at once,
 * waiting until the end to increment the cohort size results in all persons having a patientNum of 1
 */
