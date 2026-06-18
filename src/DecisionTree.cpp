// DecisionTree.cpp
// initial creation October-18-2022
// Last Updated March-21-2025
// Model specified and designed by Zachary Wallace, M.D. 
// Software written by Aaron Wu (aawu@mgh.harvard.edu), with contributions from Virginia Talbot, Zachary Wallace, M.D, and Naomi Patel, M.D. 
// 
// file containing the decision trees for the three treatment arms
/* three separate functions are made for the three treatment options.They have some duplicate code but are organized this way to help avoid confusion and so that something done
  in one has no chance of messing up how the other treatment arm works. If something about the structure of the decision trees needs to be changed, all three functions should
  receive the same changes. */


#include "include.h"

using namespace std;


int monthsMinorRelapse = 0;
int monthsSinceLastBCellRise = 0;
int monthsSinceLastAncaRise = 0;

bool anca_rise_this_month = false;
bool bcell_rise_this_month = false;
bool rtx_missed_this_month = false;
bool covid_hospitalization_this_month = false;
bool esrd_this_month = false;
bool diabetes_this_month = false;

//////////////////////////////////////////////////
// decision tree making time.

//health states:
// 0 = inactive disease, 1 = active disease, 2 = severe infection, 3 = ESRD, 4 = death, 5 = post-rx remission, 6 = post-relapse remission,
// 7 = diabetes, 8 = cardiovascular disease, 9 = fracture, 10 = covid-19 infection

// functions used to roll for next month's state depending on current state. 

// the nextStateGivenProbs functions takes in probabilities of events happening and rolls for which one will happen given the probabilities



bool probOfHappening(float prob) {
	double randNum = AavUtil::getRandomDouble();
	if (randNum <= prob) {
		return 1;  // if random probability is less than the probability, then return True. Unless equal zero, since that could cause problems if the probability is zero. 
	}
	else if (randNum > prob || randNum == 0) {
		return 0; // if random probability is greater than the probability, then return False, or if the random number is 0
	}
}

// function that determines what the patient's probability of death should be in the current month
float getProbDeath(Patient* patient, SimContext* simContext) {
	int month = patient->getGeneralState()->monthNum;
	float greatestProbDeath = 0;
	if (patient->diseaseState.isAlive) {
		// Barring all other factors, death is set to the Mortality from Remission state values
		greatestProbDeath = simContext->getRemissionRelapseMortalityInputs()->remissionMortality[patient->generalState.gender][patient->generalState.age][patient->diseaseState.hasDiabetes];
		if (patient->generalState.state == COVID) {
			float covidMortality = simContext->getCovidInputs()->covidMortality[patient->getCovidState()->covidPrEPType][patient->getGeneralState()->age];
			if (covidMortality > greatestProbDeath) {
				greatestProbDeath = covidMortality;
			}
			if (patient->generalState.prevState == ACTIVE && (month - patient->monitoringState.monthLastMajorRelapse) < 5) {
				float relapseMortality = simContext->getRemissionRelapseMortalityInputs()->activeMortality[patient->generalState.gender][patient->generalState.age][patient->diseaseState.hasDiabetes];
				if (patient->generalState.inactiveDisease) {
					relapseMortality = simContext->getRemissionRelapseMortalityInputs()->inactiveDiseaseMortality[patient->generalState.gender][patient->generalState.age][patient->diseaseState.hasDiabetes];
				}
				if (relapseMortality > greatestProbDeath) {
					greatestProbDeath = relapseMortality;
				}
			}
		}
		if (patient->generalState.state == SEVERE) {
			float infectionMortality = simContext->getInfectionMortalityInputs()->infectionMortality[patient->getGeneralState()->age][patient->getDiseaseState()->hasDiabetes];
			if (infectionMortality > greatestProbDeath) {
				greatestProbDeath = infectionMortality;
			}
			if (patient->generalState.prevState == ACTIVE && (month - patient->monitoringState.monthLastMajorRelapse) < 5) {
				float relapseMortality = simContext->getRemissionRelapseMortalityInputs()->activeMortality[patient->generalState.gender][patient->generalState.age][patient->diseaseState.hasDiabetes];
				if (patient->generalState.inactiveDisease) {
					relapseMortality = simContext->getRemissionRelapseMortalityInputs()->inactiveDiseaseMortality[patient->generalState.gender][patient->generalState.age][patient->diseaseState.hasDiabetes];
				}
				if (relapseMortality > greatestProbDeath) {
					greatestProbDeath = relapseMortality;
				}
			}
		}
		if (patient->generalState.state == ACTIVE) {
			float relapseMortality = simContext->getRemissionRelapseMortalityInputs()->activeMortality[patient->generalState.gender][patient->generalState.age][patient->diseaseState.hasDiabetes];
			if (patient->generalState.inactiveDisease) {
				relapseMortality = simContext->getRemissionRelapseMortalityInputs()->inactiveDiseaseMortality[patient->generalState.gender][patient->generalState.age][patient->diseaseState.hasDiabetes];
			}
			if (relapseMortality > greatestProbDeath) {
				greatestProbDeath = relapseMortality;
			}
		}
		if (patient->diseaseState.hasESRD) {
			float esrdMortality = simContext->getESRDMortalityInputs()->ESRDMortality[patient->generalState.age][patient->generalState.gender][patient->diseaseState.hasDiabetes];
			if (esrdMortality > greatestProbDeath) {
				greatestProbDeath = esrdMortality;
			}
		}
	}
	
	//cout << "Month " << month << ". Patient " << patient->generalState.patientNum << ". Gender: " << patient->generalState.gender << ". Age: " << patient->generalState.age << endl;
	//cout << "State: " << patient->generalState.state << endl;
	//cout << "Prev State: " << patient->generalState.prevState << endl;
	//cout << "Diabetes: " << patient->diseaseState.hasDiabetes << endl;
	//cout << "ESRD: " << patient->diseaseState.hasESRD << endl;
	//cout << "PROB DEATH: " << greatestProbDeath << endl;
	//cin.get();
	
	return greatestProbDeath;
}

// function that takes in probabilities of events happening and rolls for the one that occurs. 
float RollNextStateGivenProbs(float pInactive, float pActive, float pSevere, float pDeath, float pMinor,
	float pCovid, float pMinorFromActive)  //CHANGE IF ADDING MORE STATES
{
	float sumProbArray[7] = { (pInactive), (pActive + pInactive), (pActive + pInactive + pSevere),
		(pActive + pInactive + pSevere + pDeath), (pActive + pInactive + pSevere + pDeath + pMinor),
	(pActive + pInactive + pSevere + pDeath + pMinor + pCovid),
	(pActive + pInactive + pSevere + pDeath + pMinor + pCovid + pMinorFromActive) };
	double randNum = AavUtil::getRandomDouble();

	if (randNum <= sumProbArray[0]) {
		return REMISSION;
	}
	else if (randNum > sumProbArray[0] && randNum <= sumProbArray[1]) {
		return ACTIVE;
	}
	else if (randNum > sumProbArray[1] && randNum <= sumProbArray[2]) {
		return SEVERE;
	}
	else if (randNum > sumProbArray[2] && randNum <= sumProbArray[3]) {
		return DEATH;
	}
	else if (randNum > sumProbArray[3] && randNum <= sumProbArray[4]) {
		return POST_MINOR_RELAPSE_REMISSION;
	}
	else if (randNum >= sumProbArray[4] && randNum <= sumProbArray[5]) {
		return COVID;
	}
	else if (randNum >= sumProbArray[5] && randNum <= sumProbArray[6]) {
		return POST_MINOR_RELAPSE_ACTIVE_REMISSION;
	}
	else {
		std::cout << "ERROR: " << sumProbArray[4] << ": " << "randprob: " << randNum << ": ";
	};
};

// roll for ANCA or B Cell rise next month. Inputs: monthly anca, monthly b cell, probability of neither events, next state (if no ANCA or B Cell rise)
int RollAncaOrBCellRise(float probANCA, float probBCell, float nothing, Patient *patient) {
	float sumProbArray[3] = { probANCA, probANCA + probBCell, probANCA + probBCell + nothing };
	double randNum = AavUtil::getRandomDouble();
	if (randNum < sumProbArray[0] || randNum == 0) {
		if ((patient->getGeneralState()->monthNum - patient->getTreatment()->monthLastRTX)< 6) {
			return NOTHING;
		}
		else {
			return ANCA_RISE;
		}
	}
	else if (randNum >= sumProbArray[0] && randNum < sumProbArray[1]) {
		if ((patient->getGeneralState()->monthNum - patient->getTreatment()->monthLastRTX) < 6) {
			return NOTHING;
		}
		else {
			return BCELL_RISE;
		}
	}
	else if (randNum >= sumProbArray[1] && randNum < sumProbArray[2]) {
		return NOTHING;
	}
}

// draw for number of months that the patient will be in the ACTIVE (not INACTIVE) STATE each time they have a major relapse
int drawlengthActiveToInactive(SimContext* simContext, Patient *patient) {
	int drawnLength;
	float randNum = AavUtil::getRandomDouble();
	vector<float> lengthActive;
	for (int i = 0; i < simContext->getCohortParamsInputs()->lengthActiveToInactiveProbabilities.size(); i++) {
		lengthActive.push_back(simContext->getCohortParamsInputs()->lengthActiveToInactiveProbabilities[i]);
	}
	drawnLength = selectBasedOnOdds(lengthActive, randNum) + 1;
	return drawnLength;
}


//Open output file
// TAILORED STRATEGY DECISION TREES
int tailoredUpdateState(Patient* patient, SimContext* simContext, StateUpdater* stateUpdater) {
	anca_rise_this_month = false;
	bcell_rise_this_month = false;
	rtx_missed_this_month = false;
	covid_hospitalization_this_month = false;
	esrd_this_month = false;
	diabetes_this_month = false;

	int month = patient->getGeneralState()->monthNum;
	monthsMinorRelapse = (month+1) - patient->getMonitoringState()->monthLastMinorRelapse; // month+1 because that's what nextmonth's number will be

	int state = patient->getGeneralState()->state;
	int prevState = patient->getGeneralState()->prevState;

	int age = patient->getGeneralState()->age;
	int sex = patient->getGeneralState()->gender;
	int renalInv = patient->getGeneralState()->renalInv;
	int type = patient->getGeneralState()->ancaType;
	int covidVaccine = patient->getCovidState()->covidVacc;
	int covidImmunity = patient->getCovidState()->priorImmunity;
	int steroidRegimen = patient->getTreatment()->steroidRegimen;
	bool diabetes = patient->getDiseaseState()->hasDiabetes;
	bool esrd = patient->getDiseaseState()->hasESRD;
	bool prevActiveDisease = patient->getDiseaseState()->prevActiveDisease;
	float pDeath = getProbDeath(patient, simContext);

	int tx = TAILORED; // treatment option is Tailored (0), since the fixed treatment strategy is off
	int arm = TAILORED_ARM; // treatment arm is tailored (0)

	if (month > simContext->getCohortParamsInputs()->monthStopRTX) {
		tx = MISSED_RTX;
	}

	// change the treatment strategy to inactive if currently in active state and has inactive
	if ((state == ACTIVE || state==POST_MINOR_RELAPSE_ACTIVE_REMISSION) && patient->getGeneralState()->inactiveDisease == true) {
		tx = ACTIVE_WITH_REMISSION;
	}

	if (state == REMISSION) { // if in remission state
		float pESRD = simContext->getESRDInputs()->ESRDFromRemission[age][sex][renalInv][diabetes];
		float pSevere = simContext->getRelapseInfectioninputs()->infectionRemission[arm][diabetes][esrd];
		float pMinor = simContext->getRelapseInfectioninputs()->minRelapse[tx][type][esrd];
		float pActive = simContext->getRelapseInfectioninputs()->majRelapse[tx][type][esrd];
		float pCovid = simContext->getCovidInputs()->covidInfection[covidInfectionProbColumn(patient)][age];
		float pMinorFromActive = 0;
		// If Covid Surge is activated, add surge during the months specified
		if (simContext->getCovidInputs()->covidSurge && month >= simContext->getCovidInputs()->covidSurgeMonthStart && patient->getGeneralState()->monthNum < simContext->getCovidInputs()->covidSurgeMonthEnd) {
			pCovid = simContext->getCovidInputs()->covidSurgeMultiplier * pCovid;
		}
		float pRemission = 1 - pDeath - pActive - pMinor - pSevere - pCovid;
		float pDiabetes = simContext->getDiabetesInputs()->diabetesFromRemission[age][sex];
		float probANCARise = simContext->getCohortParamsInputs()->probANCARise;
		float probBCellRise = simContext->getCohortParamsInputs()->probBCellRise;

		int nextState = RollNextStateGivenProbs(pRemission, pActive, pSevere, pDeath, pMinor, pCovid, pMinorFromActive);

		if (nextState == REMISSION) { // if stays in remission, roll for ANCA and B Cell rises next
			int ancaOrBCell = RollAncaOrBCellRise(probANCARise, probBCellRise, 1 - probANCARise - probBCellRise, patient); // for for ANCA rise, B Cell rise, or nothing happening

			if (ancaOrBCell == ANCA_RISE) { // if ANCA Rise occurs, do the following, as long as not after month 28 because we're not continuing after that.
				anca_rise_this_month = true;
				if (month <= simContext->getCohortParamsInputs()->monthStopRTX) {
					stateUpdater->updateMonthLastRTX(month);
					nextState = POST_RX_REMISSION; // go to post rx state
				}
				else if (month > simContext->getCohortParamsInputs()->monthStopRTX) {
					rtx_missed_this_month = true;
					nextState = REMISSION;
				}
			}

			else if (ancaOrBCell == BCELL_RISE) {
				bcell_rise_this_month = true;
				if (month <= simContext->getCohortParamsInputs()->monthStopRTX) {
					stateUpdater->updateMonthLastRTX(month);
					nextState = POST_RX_REMISSION; // go to post rx state
				}
				else if (month > simContext->getCohortParamsInputs()->monthStopRTX) {
					rtx_missed_this_month = true;
					nextState = REMISSION;
				}
			}
			// Roll for Diabetes if patient does not have Diabetes
			if (probOfHappening(pDiabetes) && !patient->getDiseaseState()->hasDiabetes) {
				diabetes_this_month = true;
			}
			// Roll for ESRD if patient does not have ESRD
			if (probOfHappening(pESRD) && patient->getDiseaseState()->hasESRD == false) {
				esrd_this_month = true;
			}
		}
		else if (nextState == POST_MINOR_RELAPSE_REMISSION) {
			patient->monitoringState.monthLastMinorRelapse = month + 1;
		}
		else if (nextState == ACTIVE) {
			patient->monitoringState.monthLastMajorRelapse = month + 1; // +1 since it's for next month
			// drawing for length of active (not inactive)
			patient->generalState.lengthActiveToInactive = drawlengthActiveToInactive(simContext, patient);
		}
		
		return nextState;
	}

	if (state == ACTIVE) { // Active Disease 
		float pRemission = 0;
		float pESRD = simContext->getESRDInputs()->ESRDFromActive[age][sex][renalInv][diabetes];
		float pSevere = simContext->getRelapseInfectioninputs()->infectionActive[steroidRegimen][diabetes][esrd][prevActiveDisease];
		float pMinor = 0;
		float pDiabetes = 0;
		if (patient->getTreatment()->steroidRegimen != NO_STEROID) {
			pDiabetes = simContext->getDiabetesInputs()->diabetesFromActive[steroidRegimen][sex];
		}
		else {
			pDiabetes = simContext->getDiabetesInputs()->diabetesFromRemission[age][sex];
		}

		float pCovid = simContext->getCovidInputs()->covidInfection[covidInfectionProbColumn(patient)][age];
		// If Covid Surge is activated, add surge during the months specified
		if (simContext->getCovidInputs()->covidSurge && month >= simContext->getCovidInputs()->covidSurgeMonthStart && month < simContext->getCovidInputs()->covidSurgeMonthEnd) {
			pCovid = simContext->getCovidInputs()->covidSurgeMultiplier * pCovid;
		}
		float pMinorFromActive = 0;

		/** Here, add the if in active conditionals. Adjust probabilities accordingly, add a probability of relapse (set months Active back to 0) */

		if (tx == ACTIVE_WITH_REMISSION && patient->getGeneralState()->inactiveDisease == true) {
			float pMajorRelapse = simContext->getRelapseInfectioninputs()->majRelapse[tx][type][esrd];
			// WHAT ABOUT MINOR RELAPSE?
			pMinorFromActive = simContext->getRelapseInfectioninputs()->minRelapse[tx][type][esrd];
			// rolling for major relapse again
			double randNum = AavUtil::getRandomDouble();
			if (randNum <= pMajorRelapse) {
				// if major relapse occurs again, not going to go into minor relapse next month no matter what, since Major overrides it. 
				pMinorFromActive = 0;
				// major relapse occurs, (month - patient->getMonitoringState()->monthLastMajorRelapse) goes back to 0
				patient->monitoringState.monthLastMajorRelapse = month + 1;
				// set inactive back to false, since there is another major relapse
				patient->generalState.inactiveDisease = false;
				patient->monitoringState.hadMajorRelapseFromInactiveDisease = true;
				patient->monitoringState.majorRelapseFromInactiveDiseaseThisMonth = true;

				// drawing for time from active to inactive
				patient->generalState.lengthActiveToInactive = drawlengthActiveToInactive(simContext, patient);

				// Setting previous active to true, since relapsing from active
				stateUpdater->setPrevActiveDisease(true);
			}

		}

		float pActive = 1 - pDeath - pSevere - pRemission - pMinor - pCovid - pMinorFromActive;

		int nextState = RollNextStateGivenProbs(pRemission, pActive, pSevere, pDeath, pMinor, pCovid, pMinorFromActive);
		/** determining if should set Inactive state to be true for next month */
		if ((month - patient->getMonitoringState()->monthLastMajorRelapse) == patient->getGeneralState()->lengthActiveToInactive - 1) {
			patient->generalState.inactiveDisease = true;
		}

		/** setting state to be remission if it's been longer than 6 months */
		if (nextState == ACTIVE) { // if rolled for active but have been in active for more than 6 months, go back go remission
			if ((month - patient->monitoringState.monthLastMajorRelapse) >= 5) {
				nextState = REMISSION;
				// set inactive to be false again
				if (patient->generalState.inactiveDisease == true) {
					patient->generalState.inactiveDisease = false;
				}
			}
		}
		else if (nextState == POST_MINOR_RELAPSE_ACTIVE_REMISSION) {
			patient->monitoringState.monthLastMinorRelapse = month + 1;
			patient->monitoringState.hadMinorRelapseFromInactiveDisease = true;
			if ((month - patient->monitoringState.monthLastMajorRelapse) >= 5) {
				nextState = POST_MINOR_RELAPSE_REMISSION;
			}
		}
		if (nextState != ACTIVE && (month - patient->monitoringState.monthLastMajorRelapse) >= 5) {
			stateUpdater->setPrevActiveDisease(true);
		}
		// roll for Diabetes if does not have diabetes
		if (probOfHappening(pDiabetes) && patient->getDiseaseState()->hasDiabetes == false) {
			diabetes_this_month = true;
		}
		if (probOfHappening(pESRD) && patient->getDiseaseState()->hasESRD == false) {
			esrd_this_month = true;
		}
		return nextState;
	}

	if (state == SEVERE) { // Severe Infection

		float pESRD = 0;
		float pSevere = 0;
		float pMinor = 0;
		float pActive = 0;
		float pCovid = 0;
		float pMinorFromActive = 0;
		// If Covid Surge is activated, add surge during the months specified
		if (simContext->getCovidInputs()->covidSurge && month >= simContext->getCovidInputs()->covidSurgeMonthStart && month < simContext->getCovidInputs()->covidSurgeMonthEnd) {
			pCovid = simContext->getCovidInputs()->covidSurgeMultiplier * pCovid;
		}
		float pRemission = 1 - pDeath - pSevere - pMinor - pActive - pCovid;

		if (prevState == REMISSION || prevState == POST_MINOR_RELAPSE_REMISSION || prevState == POST_RX_REMISSION) { // if state before severe infection was a remission state, next state will be either death or remission, never active.
			pRemission = 1 - pDeath;
			pActive = 0;
		}
		else if (prevState == ACTIVE || prevState == POST_MINOR_RELAPSE_ACTIVE_REMISSION) { // if state before severe infection was active disease state, next state will be either death or back to active, never remission. 
			
			pActive = 1 - pDeath;
			pRemission = 0;
			// if prev state was active, check to see if need to switch to Inactive
			if (prevState == ACTIVE){
				if ((month - patient->getMonitoringState()->monthLastMajorRelapse) == patient->getGeneralState()->lengthActiveToInactive - 1) {
					if ((month - patient->getMonitoringState()->monthLastMajorRelapse) < 5) {
						patient->generalState.inactiveDisease = true;
					}
				}
			}
				
		}

		int nextState = RollNextStateGivenProbs(pRemission, pActive, pSevere, pDeath, pMinor, pCovid, pMinorFromActive);
		if (nextState == ACTIVE) {
			if ((month - patient->getMonitoringState()->monthLastMajorRelapse) < 5) {
				if (monthsMinorRelapse < 2) {
					nextState = POST_MINOR_RELAPSE_ACTIVE_REMISSION;
				}
			}
			// if in Active/Severe/Covid states for 6 months or more, go back to remission (or post minor relapse remission if during minor relapse)
			else if ((month - patient->getMonitoringState()->monthLastMajorRelapse) >= 5) {
				if (monthsMinorRelapse < 1 && patient->getGeneralState()->inactiveDisease) {
					nextState = POST_MINOR_RELAPSE_REMISSION;
				}
				else {
					nextState = REMISSION; 
				}
				// set Inactive to be false again
				if (patient->generalState.inactiveDisease == true) {
					patient->generalState.inactiveDisease = false;
				}
				// set prev active disease to be true, since now all future active flares will be considered relapse and not newly active.
				stateUpdater->setPrevActiveDisease(true);
			}
		}
		return nextState;
	}

	if (state == DEATH) { // if dead
		return DEATH;
	}

	if (state == POST_RX_REMISSION) { // post rituximab remission
		float pESRD = simContext->getESRDInputs()->ESRDFromRemission[age][sex][renalInv][diabetes];
		float pSevere = simContext->getRelapseInfectioninputs()->infectionRemission[arm][diabetes][esrd];
		float pActive = 0;
		float pMinor = 0;
		float pDiabetes = simContext->getDiabetesInputs()-> diabetesFromRemission[age][sex];
		float pCovid = simContext->getCovidInputs()-> covidInfection[covidInfectionProbColumn(patient)][age];
		float pMinorFromActive = 0;
		// If Covid Surge is activated, add surge during the months specified
		if (simContext->getCovidInputs()->covidSurge && month >= simContext->getCovidInputs()->covidSurgeMonthStart && month < simContext->getCovidInputs()->covidSurgeMonthEnd) {
			pCovid = simContext->getCovidInputs()->covidSurgeMultiplier * pCovid;
		}
		float pRemission = 1 - pDeath - pSevere - pActive - pMinor - pCovid - pMinorFromActive;

		int nextState = RollNextStateGivenProbs(pRemission, pActive, pSevere, pDeath, pMinor, pCovid, pMinorFromActive);
		if (nextState == REMISSION && patient->getGeneralState()->monthNum - patient->getTreatment()->monthLastRTX < 3) { // if does not roll death, ESRD, or Severe, stay in postRX for 3 months
			nextState = POST_RX_REMISSION;
		}
		else if (nextState == REMISSION && patient->getGeneralState()->monthNum - patient->getTreatment()->monthLastRTX >= 3) { // if does not roll death, ESRD, or Severe and it's been more than 3 months since last rx, return to remission state
			nextState = REMISSION;
		}
		// Roll for Diabetes if patient does not have Diabetes
		if (probOfHappening(pDiabetes) && patient->getDiseaseState()->hasDiabetes == false) {
			diabetes_this_month = true;
		}
		// Roll for ESRD if patient does not have ESRD
		if (probOfHappening(pESRD) && patient->getDiseaseState()->hasESRD == false) {
			esrd_this_month = true;
		}
		return nextState;
	}

	if (state == POST_MINOR_RELAPSE_REMISSION) { // Post-Relapse remission
		float pESRD = simContext->getESRDInputs()->ESRDFromRemission[age][sex][renalInv][diabetes];
		float pSevere = simContext->getRelapseInfectioninputs()->infectionRemission[arm][diabetes][esrd];
		float pActive = 0;
		float pMinor = 0;
		float pDiabetes = simContext->getDiabetesInputs()->diabetesFromRemission[age][sex];
		float pCovid = simContext->getCovidInputs()->covidInfection[covidInfectionProbColumn(patient)][age];
		float pMinorFromActive = 0;
		// If Covid Surge is activated, add surge during the months specified
		if (simContext->getCovidInputs()->covidSurge && month >= simContext->getCovidInputs()->covidSurgeMonthStart && month < simContext->getCovidInputs()->covidSurgeMonthEnd) {
			pCovid = simContext->getCovidInputs()->covidSurgeMultiplier * pCovid;
		}
		float pRemission = 1 - pDeath - pSevere - pActive - pMinor - pCovid - pMinorFromActive;

		int nextState = RollNextStateGivenProbs(pRemission, pActive, pSevere, pDeath, pMinor, pCovid, pMinorFromActive);
		if (nextState == REMISSION) {
			if (monthsMinorRelapse < 2) {
				nextState = POST_MINOR_RELAPSE_REMISSION;
			}
			else {
				nextState = REMISSION;
			}
		}
		// Roll for Diabetes if patient does not have Diabetes
		if (probOfHappening(pDiabetes) && patient->getDiseaseState()->hasDiabetes == false) {
			diabetes_this_month = true;
		}
		// Roll for ESRD if patient does not have ESRD
		if (probOfHappening(pESRD) && patient->getDiseaseState()->hasESRD == false) {
			esrd_this_month = true;
		}
		return nextState;
	}

	if (state == COVID) {
		float pHospital = simContext->getCovidInputs()->covidHosp[covidHospitalizationAndMortalityProbColumn(patient)][age];
		float pESRD = 0;
		float pSevere = 0;
		float pActive = 0;
		float pMinor = 0;
		float pCovid = 0;
		float pMinorFromActive = 0;
		float pRemission = 1 - pDeath - pSevere - pActive - pMinor - pCovid- pMinorFromActive;

		covid_hospitalization_this_month = probOfHappening(pHospital); // roll for hospitalization
		if (covid_hospitalization_this_month == true) { // if hospitalized, mark hospitalization this month to True and change pDeath
			if (pDeath < simContext->getCovidInputs()->covidMortality[covidHospitalizationAndMortalityProbColumn(patient)][age]) {
				pDeath = simContext->getCovidInputs()->covidMortality[covidHospitalizationAndMortalityProbColumn(patient)][age];
			}
		}

		if (prevState == REMISSION || prevState == POST_MINOR_RELAPSE_REMISSION || prevState == POST_RX_REMISSION) { // if state before severe infection was a remission state, next state will be either death or remission, never active.
			pRemission = 1 - pDeath;
			pActive = 0;
		}
		else if (prevState == ACTIVE || prevState == POST_MINOR_RELAPSE_ACTIVE_REMISSION) { // if state before severe infection was active disease state, next state will be either death or back to active, never remission. 
			pActive = 1 - pDeath;
			pRemission = 0;
			// if prev state was active, check to see if need to switch to Inactive
			if ((month - patient->getMonitoringState()->monthLastMajorRelapse) == patient->getGeneralState()->lengthActiveToInactive - 1) {
				if ((month - patient->getMonitoringState()->monthLastMajorRelapse) < 5) {
					patient->generalState.inactiveDisease = true;
				}
			}
		}
		
		
		int nextState = RollNextStateGivenProbs(pRemission, pActive, pSevere, pDeath, pMinor, pCovid, pMinorFromActive);
		if (nextState == ACTIVE) {
			if ((month - patient->getMonitoringState()->monthLastMajorRelapse) < 5) {
				if (monthsMinorRelapse < 2) {
					nextState = POST_MINOR_RELAPSE_ACTIVE_REMISSION;
				}
			}
			// if in Active/Severe/Covid states for 6 months or more, go back to remission (or post minor relapse remission if during minor relapse)
			else if ((month - patient->getMonitoringState()->monthLastMajorRelapse) >= 5) {
				if (monthsMinorRelapse < 1 && patient->getGeneralState()->inactiveDisease) {
					nextState = POST_MINOR_RELAPSE_REMISSION;
				}
				else {
					nextState = REMISSION; 
					
				}
				// set Inactive to be false again
				if (patient->generalState.inactiveDisease == true) {
					patient->generalState.inactiveDisease = false;
				}
				// set prev active disease to be true, since now all future active flares will be considered relapse and not newly active.
				stateUpdater->setPrevActiveDisease(true);
			}
		}
		return nextState;
	}

	if (state == POST_MINOR_RELAPSE_ACTIVE_REMISSION) {
		float pRemission = 0;
		float pESRD = simContext->getESRDInputs()->ESRDFromActive[age][sex][renalInv][diabetes];
		float pSevere = simContext->getRelapseInfectioninputs()->infectionRemission[arm][diabetes][esrd];
		float pMinor = 0;
		float pDiabetes = simContext->getDiabetesInputs()->diabetesFromActive[age][sex];
		float pCovid = simContext->getCovidInputs()->covidInfection[covidInfectionProbColumn(patient)][age];
		// If Covid Surge is activated, add surge during the months specified
		if (simContext->getCovidInputs()->covidSurge && month >= simContext->getCovidInputs()->covidSurgeMonthStart && month < simContext->getCovidInputs()->covidSurgeMonthEnd) {
			pCovid = simContext->getCovidInputs()->covidSurgeMultiplier * pCovid;
		}
		float pMinorFromActive = 0;
		float pActive = 1 - pRemission - pDeath - pSevere - pMinor - pCovid - pMinorFromActive;

		int nextState = RollNextStateGivenProbs(pRemission, pActive, pSevere, pDeath, pMinor, pCovid, pMinorFromActive);
		if (nextState == ACTIVE && patient->getGeneralState()->inactiveDisease) {
			if ((month - patient->getMonitoringState()->monthLastMajorRelapse) < 5) {
				if (monthsMinorRelapse < 2) {
					nextState = POST_MINOR_RELAPSE_ACTIVE_REMISSION;
				}
			}
			else {
				if (monthsMinorRelapse < 2) {
					nextState = POST_MINOR_RELAPSE_REMISSION;
				}
				else {
					nextState = REMISSION;
					
				}
				// set Inactive to be false again
				if (patient->generalState.inactiveDisease == true) {
					patient->generalState.inactiveDisease = false;
				}
			}
		}
		// Roll for Diabetes if patient does not have Diabetes
		if (probOfHappening(pDiabetes) && patient->getDiseaseState()->hasDiabetes == false) {
			diabetes_this_month = true;
		}
		// Roll for ESRD if patient does not have ESRD
		if (probOfHappening(pESRD) && patient->getDiseaseState()->hasESRD == false) {
			esrd_this_month = true;
		}
		return nextState;
	}
}

//////////////////////////////////////////////////////////////////
//FIXED STRATEGY DECISION TREE
int fixedUpdateState( Patient* patient, SimContext *simContext, StateUpdater *stateUpdater) {
	int state = patient->getGeneralState()->state;
	int prevState = patient->getGeneralState()->prevState;

	int month = patient->getGeneralState()->monthNum;
	monthsMinorRelapse = (month + 1) - patient->getMonitoringState()->monthLastMinorRelapse; // month+1 because that's what nextmonth's number will be

	anca_rise_this_month = false;
	bcell_rise_this_month = false;
	rtx_missed_this_month = false;
	covid_hospitalization_this_month = false;
	esrd_this_month = false;
	diabetes_this_month = false;

	int age = patient->getGeneralState()->age;
	int sex = patient->getGeneralState()->gender;
	int renalInv = patient->getGeneralState()->renalInv;
	int type = patient->getGeneralState()->ancaType;
	int covidVaccine = patient->getCovidState()->covidVacc;
	int covidImmunity = patient->getCovidState()->priorImmunity;
	int steroidRegimen = patient->getTreatment()->steroidRegimen;
	bool diabetes = patient->getDiseaseState()->hasDiabetes;
	bool esrd = patient->getDiseaseState()->hasESRD;
	bool prevActiveDisease = patient->getDiseaseState()->prevActiveDisease;
	float pDeath = getProbDeath(patient, simContext);

	int tx = FIXED; // treatment is turned on (Fixed = 1)
	int arm = FIXED_ARM;

	// change the treatment strategy to Inactive if currently in active state and has Inactive
	if ((state == ACTIVE || state==POST_MINOR_RELAPSE_ACTIVE_REMISSION) && patient->getGeneralState()->inactiveDisease == true) {
		tx = ACTIVE_WITH_REMISSION;
	}

	if (state == REMISSION) { // if in remission state
		float pActive = simContext->getRelapseInfectioninputs()-> majRelapse[tx][type][esrd];
		float pMinor = simContext->getRelapseInfectioninputs()-> minRelapse[tx][type][esrd];
		float pESRD = simContext->getESRDInputs()-> ESRDFromRemission[age][sex][renalInv][diabetes];
		float pSevere = simContext->getRelapseInfectioninputs()->infectionRemission[arm][diabetes][esrd];
		float pDiabetes = simContext->getDiabetesInputs()->diabetesFromRemission[age][sex];
		float pCovid = simContext->getCovidInputs()->covidInfection[covidInfectionProbColumn(patient)][age];
		float pMinorFromActive = 0;
		// If Covid Surge is activated, add surge during the months specified
		if (simContext->getCovidInputs()->covidSurge && month >= simContext->getCovidInputs()->covidSurgeMonthStart && month < simContext->getCovidInputs()->covidSurgeMonthEnd) {
			pCovid = simContext->getCovidInputs()->covidSurgeMultiplier * pCovid;
		}
		float pRemission = 1 - pDeath - pActive - pMinor - pSevere - pCovid - pMinorFromActive;

		int nextState = RollNextStateGivenProbs(pRemission, pActive, pSevere, pDeath, pMinor, pCovid, pMinorFromActive);
		// Roll for Diabetes if patient does not have Diabetes
		if (probOfHappening(pDiabetes) && patient->getDiseaseState()->hasDiabetes == false) {
			diabetes_this_month = true;
		}
		// Roll for ESRD if patient does not have ESRD
		if (probOfHappening(pESRD) && patient->getDiseaseState()->hasESRD == false) {
			esrd_this_month = true;
		}

		if (nextState == ACTIVE) {
			patient->monitoringState.monthLastMajorRelapse = month + 1; // +1 since it's for next month
			// drawing for time from active to inactive
			patient->generalState.lengthActiveToInactive = drawlengthActiveToInactive(simContext, patient);
		}
		if (nextState == POST_MINOR_RELAPSE_REMISSION) {
			patient->monitoringState.monthLastMinorRelapse = month + 1;
		}
		return nextState;
	}

	if (state == ACTIVE) { // if in active state
		float pESRD = simContext->getESRDInputs()->ESRDFromActive[age][sex][renalInv][diabetes];
		float pSevere = simContext->getRelapseInfectioninputs()->infectionActive[steroidRegimen][diabetes][esrd][prevActiveDisease];
		float pMinor = 0;
		float pRemission = 0;
		float pDiabetes = 0;
		if (patient->getTreatment()->steroidRegimen != NO_STEROID) {
			pDiabetes = simContext->getDiabetesInputs()->diabetesFromActive[steroidRegimen][sex];
		}
		else {
			pDiabetes = simContext->getDiabetesInputs()->diabetesFromRemission[age][sex];
		}
		float pCovid = simContext->getCovidInputs()->covidInfection[covidInfectionProbColumn(patient)][age];
		// If Covid Surge is activated, add surge during the months specified
		if (simContext->getCovidInputs()->covidSurge && month >= simContext->getCovidInputs()->covidSurgeMonthStart && month < simContext->getCovidInputs()->covidSurgeMonthEnd) {
			pCovid = simContext->getCovidInputs()->covidSurgeMultiplier * pCovid;
		}
		float pMinorFromActive = 0;

		/** here, add the if in active (not inactive) conditionals. Adjust probabilities accordingly, add a probability of relapse */
		if (tx == ACTIVE_WITH_REMISSION && patient->getGeneralState()->inactiveDisease == true) {
			float pMajorRelapse = simContext->getRelapseInfectioninputs()->majRelapse[tx][type][esrd];
			// WHAT ABOUT MINOR RELAPSE?
			pMinorFromActive = simContext->getRelapseInfectioninputs()->minRelapse[tx][type][esrd];
			// rolling for major relapse again
			double randNum = AavUtil::getRandomDouble();
			if (randNum <= pMajorRelapse) {
				// if major relapse occurs again, not going to go into minor relapse next month no matter what, since Major overrides it. 
				pMinorFromActive = 0;
				// major relapse occurs, (month - fixedPatient->getMonitoringState()->monthLastMajorRelapse) goes back to 0
				patient->monitoringState.monthLastMajorRelapse = month + 1;
				// set Inactive back to false, since there is another major relapse
				patient->generalState.inactiveDisease = false;
				patient->monitoringState.hadMajorRelapseFromInactiveDisease = true;
				patient->monitoringState.majorRelapseFromInactiveDiseaseThisMonth = true;

				// drawing for time from active to inactive
				patient->generalState.lengthActiveToInactive = drawlengthActiveToInactive(simContext, patient);

				// Setting previous active to true, since relapsing from active
				stateUpdater->setPrevActiveDisease(true);
			}
		}

		float pActive = 1 - pDeath - pSevere - pMinor - pRemission - pCovid - pMinorFromActive;

		int nextState = RollNextStateGivenProbs(pRemission, pActive, pSevere, pDeath, pMinor, pCovid, pMinorFromActive);
		/** determining if should set inactive state to be true for next month */
		if ((month - patient->getMonitoringState()->monthLastMajorRelapse) == patient->getGeneralState()->lengthActiveToInactive - 1) {
			patient->generalState.inactiveDisease = true;
		}

		/** setting state to be remission if it's been longer than 6 months */
		if (nextState == ACTIVE) { // if rolled for active but have been in active for more than 6 months, go back go remission
			if ((month - patient->getMonitoringState()->monthLastMajorRelapse) >= 5) {
				nextState = REMISSION;
				// set Inactive to be false again
				if (patient->generalState.inactiveDisease == true) {
					patient->generalState.inactiveDisease = false;
				}
			}
		}
		else if (nextState == POST_MINOR_RELAPSE_ACTIVE_REMISSION) {
			patient->monitoringState.monthLastMinorRelapse = month + 1;
			patient->monitoringState.hadMinorRelapseFromInactiveDisease = true;
			if ((month - patient->monitoringState.monthLastMajorRelapse) >= 5) {
				nextState = POST_MINOR_RELAPSE_REMISSION;
			}
		}
		// if next state is death OR nest state is not disease and it's been longer than the length of disease state
		if (nextState != ACTIVE && (month - patient->monitoringState.monthLastMajorRelapse) >= 5) {
			// set prev active disease to be true, since now all future active flares will be considered relapse and not newly active.
			stateUpdater->setPrevActiveDisease(true);
		}

		// roll for Diabetes if does not have diabetes
		if (probOfHappening(pDiabetes) && patient->getDiseaseState()->hasDiabetes == false) {
			diabetes_this_month = true;
		}
		// roll for ESRD if does not have diabetes
		if (probOfHappening(pESRD) && patient->getDiseaseState()->hasESRD == false) {
			esrd_this_month = true;
		}
		return nextState;
	}

	if (state == SEVERE) { // if in severe infection state
		float pMinor = 0;
		float pSevere = 0;
		float pESRD = 0;
		float pActive = 0;
		float pCovid = 0;
		float pMinorFromActive = 0;
		float pRemission = 1 - pDeath - pMinor - pSevere - pActive - pCovid - pMinorFromActive;
		if (prevState == REMISSION || prevState == POST_MINOR_RELAPSE_REMISSION) { // if state before severe infection was a remission state, next state will be either death or remission, never active.
			pRemission = 1 - pDeath;
			pActive = 0;
		}
		else if (prevState == ACTIVE || prevState == POST_MINOR_RELAPSE_ACTIVE_REMISSION) { // if state before severe infection was active disease state, next state will be either death or back to active, never remission. 
			pActive = 1 - pDeath;
			pRemission = 0;
			// if prev state was active, check to see if need to switch to Inactive
			if (prevState == ACTIVE){
				if ((month - patient->getMonitoringState()->monthLastMajorRelapse) == patient->getGeneralState()->lengthActiveToInactive - 1) {
					if ((month - patient->getMonitoringState()->monthLastMajorRelapse) < 5) {
						patient->generalState.inactiveDisease = true;
					}
				}
			}
		}
		int nextState = RollNextStateGivenProbs(pRemission, pActive, pSevere, pDeath, pMinor,  pCovid, pMinorFromActive);
		if (nextState == ACTIVE) {
			if ((month - patient->getMonitoringState()->monthLastMajorRelapse) < 5) {
				if (monthsMinorRelapse < 2) {
					nextState = POST_MINOR_RELAPSE_ACTIVE_REMISSION;
				}
			}
			// if in Active/Severe/Covid states for 6 months or more, go back to remission (or post minor relapse remission if during minor relapse)
			else if ((month - patient->getMonitoringState()->monthLastMajorRelapse) >= 5) {
				if (monthsMinorRelapse < 2 && patient->getGeneralState()->inactiveDisease) {
					nextState = POST_MINOR_RELAPSE_REMISSION;
				}
				else {
					nextState = REMISSION; 
				}
				// set Inactive to be false again
				if (patient->generalState.inactiveDisease == true) {
					patient->generalState.inactiveDisease = false;
				}
				// set prev active disease to be true, since now all future active flares will be considered relapse and not newly active.
				stateUpdater->setPrevActiveDisease(true);
			}
		}
		return nextState;
	}

	if (state == DEATH) { // if dead
		return DEATH;
	}
	if (state == POST_RX_REMISSION) { // if in post-rx remission
		return 0; // not relevant for this arm
	}
	if (state == POST_MINOR_RELAPSE_REMISSION) { // if in post-minor-relapse remission

		float pESRD = simContext->getESRDInputs()->ESRDFromRemission[age][sex][renalInv][diabetes];
		float pSevere = simContext->getRelapseInfectioninputs()->infectionRemission[arm][diabetes][esrd];
		float pActive = 0;
		float pMinor = 0;
		float pDiabetes = simContext->getDiabetesInputs()->diabetesFromRemission[age][sex];
		float pCovid = simContext->getCovidInputs()->covidInfection[covidInfectionProbColumn(patient)][age];
		// If Covid Surge is activated, add surge during the months specified
		if (simContext->getCovidInputs()->covidSurge && month >= simContext->getCovidInputs()->covidSurgeMonthStart && month < simContext->getCovidInputs()->covidSurgeMonthEnd) {
			pCovid = simContext->getCovidInputs()->covidSurgeMultiplier * pCovid;
		}
		float pMinorFromActive = 0;
		float pRemission = 1 - pDeath - pSevere - pActive - pMinor - pCovid - pMinorFromActive;
		
		int nextState = RollNextStateGivenProbs(pRemission, pActive, pSevere, pDeath, pMinor, pCovid, pMinorFromActive);
		if (nextState == REMISSION) {
			if (monthsMinorRelapse < 2) { // if rolls for remission and has been in minor relapse state for less than 2 months, stay in minor relapse state
				nextState = POST_MINOR_RELAPSE_REMISSION;
			}
			else { // if rolls for remission and has been in minor relapse state for more than 2 months, go to remission state.
				monthsMinorRelapse = 0;
				nextState = REMISSION;
			}
		}
		// Roll for Diabetes if patient does not have Diabetes
		if (probOfHappening(pDiabetes) && patient->getDiseaseState()->hasDiabetes == false) {
			diabetes_this_month = true;
		}
		// Roll for ESRD if patient does not have ESRD
		if (probOfHappening(pESRD) && patient->getDiseaseState()->hasESRD == false) {
			esrd_this_month = true;
		}
		return nextState;
	}

	if (state == COVID) {
		float pHospital = simContext->getCovidInputs()->covidHosp[covidHospitalizationAndMortalityProbColumn(patient)][age];
		float pESRD = 0;
		float pSevere = 0;
		float pActive = 0;
		float pMinor = 0;
		float pCovid = 0;
		float pMinorFromActive = 0;
		float pRemission = 1 - pDeath - pSevere - pActive - pMinor - pCovid - pMinorFromActive;

		bool hospitalization = probOfHappening(pHospital); // roll for hospitalization
		if (hospitalization == true) { // if hospitalized, mark hospitalization this month to True and change pDeath
			covid_hospitalization_this_month = true;
			if (pDeath < simContext->getCovidInputs()->covidMortality[covidHospitalizationAndMortalityProbColumn(patient)][age]) {
				pDeath = simContext->getCovidInputs()->covidMortality[covidHospitalizationAndMortalityProbColumn(patient)][age];
			}
		}

		if (prevState == REMISSION || prevState == POST_MINOR_RELAPSE_REMISSION || prevState == POST_RX_REMISSION) { // if state before severe infection was a remission state, next state will be either death or remission, never active.
			pRemission = 1 - pDeath;
			pActive = 0;
		}
		else if (prevState == ACTIVE || prevState == POST_MINOR_RELAPSE_ACTIVE_REMISSION) { // if state before severe infection was active disease state, next state will be either death or back to active, never remission. 
			pActive = 1 - pDeath;
			pRemission = 0;
			// if prev state was active, check to see if need to switch to Inactive
			if (prevState == ACTIVE){
				if ((month - patient->getMonitoringState()->monthLastMajorRelapse) == patient->getGeneralState()->lengthActiveToInactive - 1) {
					if ((month - patient->getMonitoringState()->monthLastMajorRelapse) < 5) {
						patient->generalState.inactiveDisease = true;
					}
				}
			}
		}

		int nextState = RollNextStateGivenProbs(pRemission, pActive, pSevere, pDeath, pMinor, pCovid, pMinorFromActive);
		if (nextState == ACTIVE) {
			if ((month - patient->getMonitoringState()->monthLastMajorRelapse) < 5) {
				if (monthsMinorRelapse < 2) {
					nextState = POST_MINOR_RELAPSE_ACTIVE_REMISSION;
				}
			}
			// if in Active/Severe/Covid states for 6 months or more, go back to remission (or post minor relapse remission if during minor relapse)
			else if ((month - patient->getMonitoringState()->monthLastMajorRelapse) >= 5) {
				if (monthsMinorRelapse < 2 && patient->getGeneralState()->inactiveDisease) {
					nextState = POST_MINOR_RELAPSE_REMISSION;
				}
				else {
					nextState = REMISSION; 
					
				}
				// set Inactive to be false again
				if (patient->generalState.inactiveDisease == true) {
					patient->generalState.inactiveDisease = false;
				}
				// set prev active disease to be true, since now all future active flares will be considered relapse and not newly active.
				stateUpdater->setPrevActiveDisease(true);
			}
		}
		return nextState;
	}

	if (state == POST_MINOR_RELAPSE_ACTIVE_REMISSION) {
		float pRemission = 0;
		float pESRD = simContext->getESRDInputs()->ESRDFromActive[age][sex][renalInv][diabetes];
		float pSevere = simContext->getRelapseInfectioninputs()->infectionRemission[arm][diabetes][esrd];
		float pMinor = 0;
		float pDiabetes = simContext->getDiabetesInputs()->diabetesFromActive[age][sex];
		float pCovid = simContext->getCovidInputs()->covidInfection[covidInfectionProbColumn(patient)][age];
		// If Covid Surge is activated, add surge during the months specified
		if (simContext->getCovidInputs()->covidSurge && month >= simContext->getCovidInputs()->covidSurgeMonthStart && month < simContext->getCovidInputs()->covidSurgeMonthEnd) {
			pCovid = simContext->getCovidInputs()->covidSurgeMultiplier * pCovid;
		}
		float pMinorFromActive = 0;
		float pActive = 1 - pDeath - pSevere - pRemission - pMinor - pCovid - pMinorFromActive;

		int nextState = RollNextStateGivenProbs(pRemission, pActive, pSevere, pDeath, pMinor, pCovid, pMinorFromActive);
		if (nextState == ACTIVE && patient->getGeneralState()->inactiveDisease) {
			if ((month - patient->getMonitoringState()->monthLastMajorRelapse) < 5) {
				if (monthsMinorRelapse < 2) {
					nextState = POST_MINOR_RELAPSE_ACTIVE_REMISSION;
				}
			}
			else {
				if (monthsMinorRelapse < 2) {
					nextState = POST_MINOR_RELAPSE_REMISSION;
				}
				else {
					nextState = REMISSION;
					
				}
				// set Inactive to be false again
				if (patient->generalState.inactiveDisease == true) {
					patient->generalState.inactiveDisease = false;
				}
			}
			
		}

		// Roll for Diabetes if patient does not have Diabetes
		if (probOfHappening(pDiabetes) && patient->getDiseaseState()->hasDiabetes == false) {
			diabetes_this_month = true;
		}
		// Roll for ESRD if patient does not have ESRD
		if (probOfHappening(pESRD) && patient->getDiseaseState()->hasESRD == false) {
			esrd_this_month = true;
		}
		return nextState;
	}

}

int bCellUpdateState(int tx, Patient *patient, SimContext *simContext, StateUpdater *stateUpdater) {
	anca_rise_this_month = false;
	bcell_rise_this_month = false;
	rtx_missed_this_month = false;
	covid_hospitalization_this_month = false;
	esrd_this_month = false;
	diabetes_this_month = false;

	int state = patient->getGeneralState()->state;
	int prevState = patient->getGeneralState()->prevState;

	int month = patient->getGeneralState()->monthNum;
	monthsMinorRelapse = (month + 1) - patient->getMonitoringState()->monthLastMinorRelapse; // month+1 because that's what nextmonth's number will be

	int age = patient->getGeneralState()->age;
	int sex = patient->getGeneralState()->gender;
	int renalInv = patient->getGeneralState()->renalInv;
	int type = patient->getGeneralState()->ancaType;
	int covidVaccine = patient->getCovidState()->covidVacc;
	int covidImmunity = patient->getCovidState()->priorImmunity;
	int steroidRegimen = patient->getTreatment()->steroidRegimen;
	bool diabetes = patient->getDiseaseState()->hasDiabetes;
	bool esrd = patient->getDiseaseState()->hasESRD;
	bool prevActiveDisease = patient->getDiseaseState()->prevActiveDisease;
	float pDeath = getProbDeath(patient, simContext);

	int arm = BCELL_ARM;
	
	if (month > simContext->getCohortParamsInputs()->monthStopRTX) {
		tx = MISSED_RTX; // manually changed treatment to enumeration corresponding to missed_rtx after month stop rituximab
	}
	// change the treatment strategy to Inactive if currently in active state and has Inactive
	if ((state == ACTIVE || state==POST_MINOR_RELAPSE_ACTIVE_REMISSION) && patient->getGeneralState()->inactiveDisease == true) {
		tx = ACTIVE_WITH_REMISSION;
	}

	if (state == REMISSION) { // if in remission state
		float pESRD = simContext->getESRDInputs()->ESRDFromRemission[age][sex][renalInv][diabetes];
		float pSevere = simContext->getRelapseInfectioninputs()->infectionRemission[arm][diabetes][esrd];
		float pMinor = simContext->getRelapseInfectioninputs()->minRelapse[tx][type][esrd];
		float pActive = simContext->getRelapseInfectioninputs()->majRelapse[tx][type][esrd];
		float pDiabetes = simContext->getDiabetesInputs()->diabetesFromRemission[age][sex];
		float pCovid = simContext->getCovidInputs()->covidInfection[covidInfectionProbColumn(patient)][age];
		//float* ptr_pCovid = &pCovid;
		// If Covid Surge is activated, add surge during the months specified
		if (simContext->getCovidInputs()->covidSurge && month >= simContext->getCovidInputs()->covidSurgeMonthStart && month < simContext->getCovidInputs()->covidSurgeMonthEnd) {
			pCovid = simContext->getCovidInputs()->covidSurgeMultiplier * pCovid;
		}
		float pMinorFromActive = 0;
		float pRemission = 1 - pDeath - pActive - pMinor - pSevere - pCovid - pMinorFromActive;
		float probANCARise = simContext->getCohortParamsInputs()->probANCARise;
		float probBCellRise = simContext->getCohortParamsInputs()->probBCellRise;

		int nextState = RollNextStateGivenProbs(pRemission, pActive, pSevere, pDeath, pMinor, pCovid, pMinorFromActive);

		if (nextState == REMISSION) { // if stays in remission, roll for ANCA and B Cell rises next
			int ancaOrBCell = RollAncaOrBCellRise(probANCARise, probBCellRise, 1 - probANCARise - probBCellRise, patient); // for for ANCA rise, B Cell rise, or nothing happening

			if (ancaOrBCell == ANCA_RISE) { // if ANCA Rise occurs, do the following, as long as not after month 28 because we're not continuing after that.
				anca_rise_this_month = true;
			}
			else if (ancaOrBCell == BCELL_RISE && patient->getMonitoringState()->hadBCellRise == false) { // don't count what happens during or after month 28 because we're not continuing after that
				bcell_rise_this_month = true;
			}
		}
		else if (nextState == POST_MINOR_RELAPSE_REMISSION) {
			patient->monitoringState.monthLastMinorRelapse = month+1;
		}
		else if (nextState == ACTIVE) {
			patient->monitoringState.monthLastMajorRelapse = month + 1; // +1 since it's for next month
			// drawing for time from active to inactive
			patient->generalState.lengthActiveToInactive = drawlengthActiveToInactive(simContext, patient);
		}
		// Roll for Diabetes if patient does not have Diabetes
		if (probOfHappening(pDiabetes) && patient->getDiseaseState()->hasDiabetes == false) {
			diabetes_this_month = true;
		}
		// Roll for ESRD if patient does not have ESRD
		if (probOfHappening(pESRD) && patient->getDiseaseState()->hasESRD == false) {
			esrd_this_month = true;
		}
		
		return nextState;
	}


	if (state == ACTIVE) { // Active Disease 
		float pRemission = 0;
		float pESRD = simContext->getESRDInputs()->ESRDFromActive[age][sex][renalInv][diabetes];
		float pSevere = simContext->getRelapseInfectioninputs()->infectionActive[steroidRegimen][diabetes][esrd][prevActiveDisease];
		float pMinor = 0;
		float pDiabetes = 0;
		if (patient->getTreatment()->steroidRegimen != NO_STEROID) {
			pDiabetes = simContext->getDiabetesInputs()->diabetesFromActive[steroidRegimen][sex];
		}
		else {
			pDiabetes = simContext->getDiabetesInputs()->diabetesFromRemission[age][sex];
		}
		float pCovid = simContext->getCovidInputs()->covidInfection[covidInfectionProbColumn(patient)][age];
		// If Covid Surge is activated, add surge during the months specified
		if (simContext->getCovidInputs()->covidSurge && month >= simContext->getCovidInputs()->covidSurgeMonthStart && month < simContext->getCovidInputs()->covidSurgeMonthEnd) {
			pCovid = simContext->getCovidInputs()->covidSurgeMultiplier * pCovid;
		}
		float pMinorFromActive = 0;
		

		/** Here, add the if in active and inactive conditionals. Adjust probabilities accordingly, add a probability of relapse (set months Active back to 0) */

		if (tx == ACTIVE_WITH_REMISSION && patient->getGeneralState()->inactiveDisease == true) {
			
			float pMajorRelapse = simContext->getRelapseInfectioninputs()->majRelapse[tx][type][esrd];
			// WHAT ABOUT MINOR RELAPSE?
			pMinorFromActive = simContext->getRelapseInfectioninputs()->minRelapse[tx][type][esrd];
			// rolling for major relapse again
			double randNum = AavUtil::getRandomDouble();
			if (randNum <= pMajorRelapse) {
				// if major relapse occurs again, not going to go into minor relapse next month no matter what, since Major overrides it. 
				pMinorFromActive = 0;
				// major relapse occurs, (month - patient->getMonitoringState()->monthLastMajorRelapse) goes back to 0
				patient->monitoringState.monthLastMajorRelapse = month + 1;
				// set Inactive back to false, since there is another major relapse
				patient->generalState.inactiveDisease = false;
				patient->monitoringState.hadMajorRelapseFromInactiveDisease = true;
				patient->monitoringState.majorRelapseFromInactiveDiseaseThisMonth = true;

				// drawing for time from active to inactive
				patient->generalState.lengthActiveToInactive = drawlengthActiveToInactive(simContext, patient);

				// Setting previous active to true, since relapsing from active
				stateUpdater->setPrevActiveDisease(true);
			}

		}
		float pActive = 1 - pDeath - pSevere - pRemission - pMinor - pCovid - pMinorFromActive;

		int nextState = RollNextStateGivenProbs(pRemission, pActive, pSevere, pDeath, pMinor, pCovid, pMinorFromActive);
		/** determining if should set inactive state to be true for next month */
		if ((month - patient->getMonitoringState()->monthLastMajorRelapse) == patient->getGeneralState()->lengthActiveToInactive - 1) {
			patient->generalState.inactiveDisease = true;
		}

		/** setting state to be remission if it's been longer than 6 months */
		if (nextState == ACTIVE) { // if rolled for active but have been in active for more than 6 months, go back go remission
			if ((month - patient->getMonitoringState()->monthLastMajorRelapse) >= 5) {
				nextState = REMISSION;
				// set Inactive to be false again
				if (patient->generalState.inactiveDisease == true) {
					patient->generalState.inactiveDisease = false;
				}
			}
		}
		else if (nextState == POST_MINOR_RELAPSE_ACTIVE_REMISSION) {
			patient->monitoringState.monthLastMinorRelapse = month + 1;
			patient->monitoringState.hadMinorRelapseFromInactiveDisease = true;
			if ((month - patient->monitoringState.monthLastMajorRelapse) >= 5) {
				nextState = POST_MINOR_RELAPSE_REMISSION;
			}
		}
		if (nextState != ACTIVE && (month - patient->monitoringState.monthLastMajorRelapse) >= 5) {
			// set prev active disease to be true, since now all future active flares will be considered relapse and not newly active.
			stateUpdater->setPrevActiveDisease(true);
		}
		// roll for Diabetes if does not have diabetes
		if (probOfHappening(pDiabetes) && patient->getDiseaseState()->hasDiabetes == false) {
			diabetes_this_month = true;
		}
		// roll for ESRD if does not have diabetes
		if (probOfHappening(pESRD) && patient->getDiseaseState()->hasESRD == false) {
			esrd_this_month = true;
		}
		return nextState;
	}

	if (state == SEVERE) { // Severe Infection

		float pESRD = 0;
		float pSevere = 0;
		float pMinor = 0;
		float pActive = 0;
		float pDiabetes = 0;
		float pCovid = 0;
		float pMinorFromActive = 0;
		float pRemission = 1 - pDeath - pSevere - pMinor - pActive - pCovid - pMinorFromActive;

		if (prevState == REMISSION || prevState == POST_MINOR_RELAPSE_REMISSION || prevState == POST_RX_REMISSION) { // if state before severe infection was a remission state, next state will be either death or remission, never active.
			pRemission = 1 - pDeath;
			pActive = 0;
		}
		else if (prevState == ACTIVE || prevState == POST_MINOR_RELAPSE_ACTIVE_REMISSION) { // if state before severe infection was active disease state, next state will be either death or back to active, never remission. 
			pActive = 1 - pDeath;
			pRemission = 0;
			// if prev state was active, check to see if need to switch to Inactive
			if (prevState == ACTIVE){
				if ((month - patient->getMonitoringState()->monthLastMajorRelapse) == patient->getGeneralState()->lengthActiveToInactive - 1) {
					if ((month - patient->getMonitoringState()->monthLastMajorRelapse) < 5) {
						patient->generalState.inactiveDisease = true;
					}
				}
			}
		}

		int nextState = RollNextStateGivenProbs(pRemission, pActive, pSevere, pDeath, pMinor, pCovid, pMinorFromActive);
		
		if (nextState == ACTIVE) {
			if ((month - patient->getMonitoringState()->monthLastMajorRelapse) < 5) {
				if (monthsMinorRelapse < 2) {
					nextState = POST_MINOR_RELAPSE_ACTIVE_REMISSION;
				}
			}
			// if in Active/Severe/Covid states for 6 months or more, go back to remission (or post minor relapse remission if during minor relapse)
			else if ((month - patient->getMonitoringState()->monthLastMajorRelapse) >= 5) {
				if (monthsMinorRelapse < 1 && patient->getGeneralState()->inactiveDisease) {
					nextState = POST_MINOR_RELAPSE_REMISSION;
				}
				else {
					nextState = REMISSION; 
				}
				// set Inactive to be false again
				if (patient->generalState.inactiveDisease == true) {
					patient->generalState.inactiveDisease = false;
				}
				// set prev active disease to be true, since now all future active flares will be considered relapse and not newly active.
				stateUpdater->setPrevActiveDisease(true);
			}
		}
		return nextState;
	}
	if (state == DEATH) {
		return DEATH;
	}

	if (state == POST_RX_REMISSION) { // post rituximab remission - NOT RELEVANT FOR THIS STRATEGY (rx is never given until flare, in which case tailored strategy is activated)
		float pESRD = simContext->getESRDInputs()->ESRDFromRemission[age][sex][renalInv][diabetes];
		float pSevere = 0;
		float pActive = 0;
		float pMinor = 0;
		float pDiabetes = simContext->getDiabetesInputs()->diabetesFromRemission[age][sex];
		float pCovid = simContext->getCovidInputs()->covidInfection[covidInfectionProbColumn(patient)][age];
		// If Covid Surge is activated, add surge during the months specified
		if (simContext->getCovidInputs()->covidSurge && month >= simContext->getCovidInputs()->covidSurgeMonthStart && month < simContext->getCovidInputs()->covidSurgeMonthEnd) {
			pCovid = simContext->getCovidInputs()->covidSurgeMultiplier * pCovid;
		}
		float pMinorFromActive = 0;
		float pRemission = 1 - pDeath - pSevere - pActive - pMinor - pCovid - pMinorFromActive;

		int nextState = RollNextStateGivenProbs(pRemission, pActive, pSevere, pDeath, pMinor, pCovid, pMinorFromActive);
		// Roll for Diabetes if patient does not have Diabetes
		if (probOfHappening(pDiabetes) && patient->getDiseaseState()->hasDiabetes == false) {
			diabetes_this_month = true;
		}
		// Roll for ESRD if patient does not have ESRD
		if (probOfHappening(pESRD) && patient->getDiseaseState()->hasESRD == false) {
			esrd_this_month = true;
		}
		return nextState;
	}

	if (state == POST_MINOR_RELAPSE_REMISSION) { // Post-Relapse remission
		float pESRD = simContext->getESRDInputs()->ESRDFromRemission[age][sex][renalInv][diabetes];
		float pSevere = simContext->getRelapseInfectioninputs()->infectionRemission[arm][diabetes][esrd];
		float pActive = 0;
		float pMinor = 0;
		float pDiabetes = simContext->getDiabetesInputs()->diabetesFromRemission[age][sex];
		float pCovid = simContext->getCovidInputs()->covidInfection[covidInfectionProbColumn(patient)][age];
		// If Covid Surge is activated, add surge during the months specified
		if (simContext->getCovidInputs()->covidSurge && month >= simContext->getCovidInputs()->covidSurgeMonthStart && month < simContext->getCovidInputs()->covidSurgeMonthEnd) {
			pCovid = simContext->getCovidInputs()->covidSurgeMultiplier * pCovid;
		}
		float pMinorFromActive = 0;
		float pRemission = 1 - pDeath - pSevere - pActive - pMinor - pCovid - pMinorFromActive;

		int nextState = RollNextStateGivenProbs(pRemission, pActive, pSevere, pDeath, pMinor, pCovid, pMinorFromActive);
		if (nextState == REMISSION) {
			if (monthsMinorRelapse < 2) {
				nextState = POST_MINOR_RELAPSE_REMISSION;
			}
			else {
				nextState = REMISSION;
			}
		}
		// Roll for Diabetes if patient does not have Diabetes
		if (probOfHappening(pDiabetes) && patient->getDiseaseState()->hasDiabetes == false) {
			diabetes_this_month = true;
		}
		// Roll for ESRD if patient does not have ESRD
		if (probOfHappening(pESRD) && patient->getDiseaseState()->hasESRD == false) {
			esrd_this_month = true;
		}
		return nextState;
	}

	if (state == COVID) {
		float pHospital = simContext->getCovidInputs()->covidHosp[covidHospitalizationAndMortalityProbColumn(patient)][age];
		float pESRD = 0;
		float pSevere = 0;
		float pActive = 0;
		float pMinor = 0;
		float pCovid = 0;
		float pMinorFromActive = 0;
		float pRemission = 1 - pDeath - pSevere - pActive - pMinor - pCovid - pMinorFromActive;

		covid_hospitalization_this_month = probOfHappening(pHospital); // roll for hospitalization
		if (covid_hospitalization_this_month == true) { // if hospitalized, mark hospitalization this month to True and change pDeath
			if (pDeath < simContext->getCovidInputs()->covidMortality[covidHospitalizationAndMortalityProbColumn(patient)][age]) {
				pDeath = simContext->getCovidInputs()->covidMortality[covidHospitalizationAndMortalityProbColumn(patient)][age];
			}
		}
		if (prevState == REMISSION || prevState == POST_MINOR_RELAPSE_REMISSION || prevState == POST_RX_REMISSION) { // if state before severe infection was a remission state, next state will be either death or remission, never active.
			pRemission = 1 - pDeath;
			pActive = 0;
		}
		else if (prevState == ACTIVE || prevState == POST_MINOR_RELAPSE_ACTIVE_REMISSION) { // if state before severe infection was active disease state, next state will be either death or back to active, never remission. 
			pActive = 1 - pDeath;
			pRemission = 0;
			// if prev state was active, check to see if need to switch to Inactive
			if (prevState == ACTIVE){
				if ((month - patient->getMonitoringState()->monthLastMajorRelapse) == patient->getGeneralState()->lengthActiveToInactive - 1) {
					if ((month - patient->getMonitoringState()->monthLastMajorRelapse) < 5) {
						patient->generalState.inactiveDisease = true;
					}
				}
			}
		}

		int nextState = RollNextStateGivenProbs(pRemission, pActive, pSevere, pDeath, pMinor, pCovid, pMinorFromActive);
		if (nextState == ACTIVE) {
			if ((month - patient->getMonitoringState()->monthLastMajorRelapse) < 5) {
				if (monthsMinorRelapse < 2) {
					nextState = POST_MINOR_RELAPSE_ACTIVE_REMISSION;
				}
			}
			// if in Active/Severe/Covid states for 6 months or more, go back to remission (or post minor relapse remission if during minor relapse)
			else if ((month - patient->getMonitoringState()->monthLastMajorRelapse) >= 5) {
				if (monthsMinorRelapse < 2 && patient->getGeneralState()->inactiveDisease) {
					nextState = POST_MINOR_RELAPSE_REMISSION;
				}
				else {
					nextState = REMISSION; 
					
				}
				// set Inactive to be false again
				if (patient->generalState.inactiveDisease == true) {
					patient->generalState.inactiveDisease = false;
				}
				// set prev active disease to be true, since now all future active flares will be considered relapse and not newly active.
				stateUpdater->setPrevActiveDisease(true);
			}
		}
		return nextState;
	}

	if (state == POST_MINOR_RELAPSE_ACTIVE_REMISSION) {
		float pRemission = 0;
		float pESRD = simContext->getESRDInputs()->ESRDFromActive[age][sex][renalInv][diabetes];
		float pSevere = simContext->getRelapseInfectioninputs()->infectionRemission[arm][diabetes][esrd];
		float pMinor = 0;
		float pDiabetes = simContext->getDiabetesInputs()->diabetesFromActive[age][sex];
		float pCovid = simContext->getCovidInputs()->covidInfection[covidInfectionProbColumn(patient)][age];
		// If Covid Surge is activated, add surge during the months specified
		if (simContext->getCovidInputs()->covidSurge && month >= simContext->getCovidInputs()->covidSurgeMonthStart && month < simContext->getCovidInputs()->covidSurgeMonthEnd) {
			pCovid = simContext->getCovidInputs()->covidSurgeMultiplier * pCovid;
		}
		float pMinorFromActive = 0;
		float pActive = 1 - pDeath - pSevere - pRemission - pMinor - pCovid - pMinorFromActive;

		int nextState = RollNextStateGivenProbs(pRemission, pActive, pSevere, pDeath, pMinor, pCovid, pMinorFromActive);
		if (nextState == ACTIVE && patient->getGeneralState()->inactiveDisease) {
			if ((month - patient->getMonitoringState()->monthLastMajorRelapse) < 5) {
				if (monthsMinorRelapse < 2) {
					nextState = POST_MINOR_RELAPSE_ACTIVE_REMISSION;
				}
			}
			else {
				if (monthsMinorRelapse < 2) {
					nextState = POST_MINOR_RELAPSE_REMISSION;
				}
				else {
					nextState = REMISSION;
					
				}
				// set Inactive to be false again
				if (patient->generalState.inactiveDisease == true) {
					patient->generalState.inactiveDisease = false;
				}
			}
			
		}

		// Roll for Diabetes if patient does not have Diabetes
		if (probOfHappening(pDiabetes) && patient->getDiseaseState()->hasDiabetes == false) {
			diabetes_this_month = true;
		}
		// Roll for ESRD if patient does not have ESRD
		if (probOfHappening(pESRD) && patient->getDiseaseState()->hasESRD == false) {
			esrd_this_month = true;
		}
		return nextState;
	}
}
