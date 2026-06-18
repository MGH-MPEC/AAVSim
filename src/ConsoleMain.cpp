#include "include.h"

string stateMapping[8] = {"Remission", "Active", "Severe", "Death", "Post-RTX Remission", "Post-Minor-Relapse Remission", "Covid", "Post-Minor Relapse from Inactive"};
string treatmentMapping[5] = { "tailored", "fixed", "missed RTX", "bCellTailored", "bCellTailored rise" }; // list of strings to map the enumerated treatment types to the proper strings for output purposes

/** \brief Main function for a console based application */
int main(int argc, char *argv[]) {
    /** Console application uses the directory given on the command line or the
		current directory if none is given (like the old model) */
	if (argc > 1) {
		AavUtil::inputsDirectory = argv[1];
		AavUtil::changeDirectoryToInputs();
	}
	else {
		AavUtil::useCurrentDirectoryForInputs();
		#if __APPLE__
			AavUtil::changeDirectoryToInputs();
		#endif
	}
	/** Create the results directory and the summaries stats object */
	AavUtil::createResultsDirectory();
	string summariesFileName = AavUtil::FILE_NAME_SUMMARIES;

	AavUtil::findInputFiles();
	
	for (vector<string>::iterator inputFileIter = AavUtil::filesToRun.begin();
		inputFileIter != AavUtil::filesToRun.end(); inputFileIter++) {

		/** Display the file name being run */
		printf("Running File: %s\n", (*inputFileIter).c_str());
		std::cout << *inputFileIter << endl;
		string inputFileName = *inputFileIter;
		string runName = inputFileName.substr(0, inputFileName.find(AavUtil::FILE_EXTENSION_FOR_INPUT));
		//AavUtil::createTraceFilesDirectory();

		/** Create the results sub directory for the run (same name as the .in file) */
		#if defined(_WIN32)
			string resultsSubDirectory = AavUtil::resultsDirectory;
			resultsSubDirectory.append("\\");
			size_t lastindex = inputFileName.find_last_of("."); 
			string rawFileName = inputFileName.substr(0, lastindex); 
			resultsSubDirectory.append(rawFileName);
			_mkdir(resultsSubDirectory.c_str());
			// set up trace directory inside of run results subdirectory
			string traceDirectory = resultsSubDirectory;
			traceDirectory.append("\\trace");
			_mkdir(traceDirectory.c_str());
		#else
			string resultsSubDirectory = AavUtil::resultsDirectory;
			resultsSubDirectory.append("/");
			size_t lastindex = inputFileName.find_last_of("."); 
			string rawFileName = inputFileName.substr(0, lastindex); 
			resultsSubDirectory.append(rawFileName);
			mkdir(resultsSubDirectory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			// set up trace directory inside of run results subdirectory
			string traceDirectory = resultsSubDirectory;
			traceDirectory.append("/trace");
			mkdir(traceDirectory.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		#endif

		/** Create the simulation context and read in the input file/ */
		SimContext* simContext = new SimContext(runName);
		
		try {
			simContext->readInputs();
		}
		catch (string &errorString) {
			printf("%s\n", errorString.c_str());
			continue;
		}
		

		/** Initializes the random number generator with either a random or fixed seed */
		AavUtil::setRandomSeedType(simContext->getCohortParamsInputs()->randSeedByTime);

		int endMonth = simContext->getCohortParamsInputs()->endMonth;
		int monthStopRTX = simContext->getCohortParamsInputs()->monthStopRTX;
		// BEGIN setting up event counters
				// tailored arm event counters (start them all at 0)
		int minor_relapse_total_tailored = 0;
		int major_relapse_total_tailored = 0;
		int minor_relapse_from_remission_tailored = 0;
		int major_relapse_from_remission_tailored = 0;
		int minor_relapse_from_inactive_disease_tailored = 0;
		int major_relapse_from_inactive_disease_tailored = 0;
		int months_active_disease_tailored = 0;
		int months_inactive_disease_tailored = 0;
		int severe_infection_total_tailored = 0;
		int esrd_total_tailored = 0;
		int covid_infection_total_tailored = 0;
		int covid_hospitalizations_tailored = 0;
		int diabetes_total_tailored = 0;
		int num_anca_rise_tailored = 0;
		int num_bcell_rise_tailored = 0;
		int missed_rtx_tailored = 0;
		int num_rituximab_tailored = 0; // number of rituximab infusions counter for tailored
		int relapse_free_survival_total_tailored = 0;
		int survived_tailored = 0;
		int remission_death_tailored = 0;
		int active_death_tailored = 0;
		int infection_from_remission_death_tailored = 0; // death from infection when infection was from remission for tailored arm
		int infection_from_active_death_tailored = 0; // death from infection when infection was from active disease state for tailored arm
		int esrd_death_tailored = 0;
		int covid_death_tailored = 0;
		int diabetes_death_tailored = 0;
		
		// fixed arm event counters
		int minor_relapse_total_fixed = 0;
		int major_relapse_total_fixed = 0;
		int minor_relapse_from_remission_fixed = 0;
		int major_relapse_from_remission_fixed = 0;
		int minor_relapse_from_inactive_disease_fixed = 0;
		int major_relapse_from_inactive_disease_fixed = 0;
		int months_active_disease_fixed = 0;
		int months_inactive_disease_fixed = 0;
		int severe_infection_total_fixed = 0;
		int esrd_total_fixed = 0;
		int covid_infection_total_fixed = 0;
		int covid_hospitalizations_fixed = 0;
		int diabetes_total_fixed = 0;
		int num_anca_rise_fixed = 0;
		int num_bcell_rise_fixed = 0;
		int missed_rtx_fixed = 0;
		int num_rituximab_fixed = 0; // number of rituximab infusions counter for fixed
		int relapse_free_survival_total_fixed = 0;
		int survived_fixed = 0;
		int remission_death_fixed = 0;
		int active_death_fixed = 0;
		int infection_from_remission_death_fixed = 0; // death from infection when infection was from remission for fixed arm
		int infection_from_active_death_fixed = 0; // death from infection when infection was from active for fixed arm
		int esrd_death_fixed = 0;
		int covid_death_fixed = 0;
		int diabetes_death_fixed = 0;
		
		
		// b cell arm event counters
		int minor_relapse_total_bcell = 0;
		int major_relapse_total_bcell = 0;
		int minor_relapse_from_remission_bcell = 0;
		int major_relapse_from_remission_bcell = 0;
		int minor_relapse_from_inactive_disease_bcell = 0;
		int major_relapse_from_inactive_disease_bcell = 0;
		int months_active_disease_bcell = 0;
		int months_inactive_disease_bcell = 0;
		int severe_infection_total_bcell = 0;
		int esrd_total_bcell = 0;
		int covid_infection_total_bcell = 0;
		int covid_hospitalizations_bcell = 0;
		int diabetes_total_bcell = 0;
		int num_anca_rise_bcell = 0;
		int num_bcell_rise_bcell = 0;
		int missed_rtx_bcell = 0;
		int num_rituximab_bcell = 0; // number of rituximab infusions counter for B Cell arm
		int relapse_free_survival_total_bcell = 0;
		int survived_bcell = 0;
		int remission_death_bcell = 0;
		int active_death_bcell = 0;
		int infection_from_remission_death_bcell = 0;  // death from infection when infection was from remission for B Cell arm
		int infection_from_active_death_bcell = 0; // death from infection when infection was from active for B Cell arm
		int esrd_death_bcell = 0;
		int covid_death_bcell = 0;
		int diabetes_death_bcell = 0;
		//vectors used to count up number of patients who have experienced events by month
		vector<int> relapseFreeTailored(endMonth + 1, 0);
		vector<int> relapseFreeFixed(endMonth + 1, 0);
		vector<int> relapseFreeBCell(endMonth + 1, 0); // vector for number of patients free of relapse by month. All values initialized to 0. 
		vector<int> _deathTailored(endMonth + 1, 0);
		vector<int> _deathFixed(endMonth + 1, 0);
		vector<int> _deathBCell(endMonth + 1, 0);
		vector<int> _majorRelapseTailored(endMonth + 1, 0);
		vector<int> _majorRelapseFixed(endMonth + 1, 0);
		vector<int> _majorRelapseBCell(endMonth + 1, 0);
		vector<int> _minorRelapseTailored(endMonth + 1, 0);
		vector<int> _minorRelapseFixed(endMonth + 1, 0);
		vector<int> _minorRelapseBCell(endMonth + 1, 0);
		vector<int> _severeTailored(endMonth + 1, 0);
		vector<int> _severeFixed(endMonth + 1, 0);
		vector<int> _severeBCell(endMonth + 1, 0);
		vector<int> _covidTailored(endMonth + 1, 0);
		vector<int> _covidFixed(endMonth + 1, 0);
		vector<int> _covidBCell(endMonth + 1, 0);
		vector<int> _covidHospTailored(endMonth + 1, 0);
		vector<int> _covidHospFixed(endMonth + 1, 0);
		vector<int> _covidHospBCell(endMonth + 1, 0);
		vector<int> _covidDeathTailored(endMonth + 1, 0);
		vector<int> _covidDeathFixed(endMonth + 1, 0);
		vector<int> _covidDeathBCell(endMonth + 1, 0);
		vector<int> _esrdTailored(endMonth + 1, 0);
		vector<int> _esrdFixed(endMonth + 1, 0);
		vector<int> _esrdBCell(endMonth + 1, 0);
		vector<int> _diabetesTailored(endMonth + 1, 0);
		vector<int> _diabetesFixed(endMonth + 1, 0);
		vector<int> _diabetesBCell(endMonth + 1, 0);

		vector<int> bCellRepop_Tailored(endMonth + 1, 0);
		vector<int> bCellRepop_Fixed(endMonth + 1, 0);
		vector<int> bCellRepop_BCell(endMonth + 1, 0); // vector for number of patients who have had b cell repopulation by month
		vector<int> vaccineEligible_Tailored(endMonth + 1, 0);
		vector<int> vaccineEligible_Fixed(endMonth + 1, 0);
		vector<int> vaccineEligible_BCell(endMonth + 1, 0); // vector for number of patients who are vaccine eligible by month
		
		// vectors for number of patients who have been vaccinated and are vaccine-up-to-date
		vector<int> _vaccineUpToDateTailored(endMonth + 1, 0); 
		vector<int> _vaccineUpToDateFixed(endMonth + 1, 0);
		vector<int> _vaccineUpToDateBCell(endMonth + 1, 0);

		vector<int> renalInvTailoredMonthly(endMonth + 1, 0); // vector for number of patients with renal involvement each month in tailored arm
		vector<int> renalInvFixedMonthly(endMonth + 1, 0); // vector for number of patients with renal involvement each month in fixed arm
		vector<int> renalInvBCellMonthly(endMonth + 1, 0); // vector for number of patients with renal involvement each month in b cell arm

		// vectors for number of patients that are vaccinated at each month
		vector<int> numberVaccinatedTailored(endMonth + 1, 0);
		vector<int> numberVaccinatedFixed(endMonth + 1, 0);
		vector<int> numberVaccinatedBCell(endMonth + 1, 0);

		// END SETTING UP EVENT COUNTERS
		AavUtil::changeDirectoryToResults();

		// Seting up trace files
		std::ofstream tailoredTraceFile;
		std::ofstream fixedTraceFile;
		std::ofstream bCellTraceFile;
		std::ofstream outputFile;
		tailoredTraceFile.open(traceDirectory + "/tailored.txt");
		fixedTraceFile.open(traceDirectory+"/fixed.txt");
		bCellTraceFile.open(traceDirectory + "/bcell.txt");

		tailoredTraceFile << "Monthly outcomes for patiens with TAILORED TREATMENT" << endl;
		fixedTraceFile << "Monthly outcomes for patients with FIXED TREATMENT" << endl;
		bCellTraceFile << "Monthly outcomes for patients with B CELL TREATMENT" << endl;

		/** Load the number of cohorts and settings for stopping simulation */
		int numCohortsLimit = simContext->getCohortParamsInputs()->numPatients;
		int numRun = 0;
		
		
		while (numRun < numCohortsLimit ) {
			

			/* Creating patient object */
			Patient *patient = new Patient(simContext, numRun);

			/* Setting up three versions of the same patient for each of the three treatment arms */
			Patient tailoredPatient = *patient;

			/* Setting up the state updaters for the patient for each treatment arm */
			StateUpdater* stateUpdaterTailored = new StateUpdater(&tailoredPatient);

			
			/* TAILORED ARM SIMULATION*/
			tailoredTraceFile << "Patient " << numRun << ". " << "Sex: " << tailoredPatient.getGeneralState()->gender << ". Age: " << tailoredPatient.getGeneralState()->age << 
				". Type: " << tailoredPatient.getGeneralState()->ancaType << ". Renal: " << tailoredPatient.getGeneralState()->renalInv << 
				". Active Length (months): " << tailoredPatient.getGeneralState()->lengthActiveToInactive <<
				". Previous Active: " << tailoredPatient.getDiseaseState()->prevActiveDisease  << 
				 "." << '\n';
			
			/* Resetting month trackers */
			diabetes_this_month = false;
			esrd_this_month = false;
			anca_rise_this_month = false;
			bcell_rise_this_month = false;

			while (tailoredPatient.getGeneralState()->monthNum <= simContext->getCohortParamsInputs()->endMonth) {
				
				int currentState = tailoredPatient.getGeneralState()->state;
				int prevState = tailoredPatient.getGeneralState()->prevState;
				int twoMonthsAgoState = tailoredPatient.getGeneralState()->twoMonthsAgoState;
				int month = tailoredPatient.getGeneralState()->monthNum;
				int nextState;

				/* if going into disease state from non-active, mark this month as the month of last major relapse. */
				// if coming from Active, Severe, Covid, or Post Minor Relapse from Inactive (called post_minor_relapse_active_remission still), DO NOT reset the month last major relapse because these states will be part of the current 6 months of Active.
				std::array<int, 4> possibleActiveStates = {ACTIVE, SEVERE, COVID, POST_MINOR_RELAPSE_ACTIVE_REMISSION};
				if (currentState == ACTIVE && std::find(possibleActiveStates.begin(), possibleActiveStates.end(), prevState) == possibleActiveStates.end()) {
					stateUpdaterTailored->updateMonthLastMajorRelapse(month);
				}

				// if no longer in disease state, turn off Inactive marker in patient
				if (std::find(possibleActiveStates.begin(), possibleActiveStates.end(), prevState) == possibleActiveStates.end()) {
					if (tailoredPatient.getGeneralState()->inactiveDisease) {
						stateUpdaterTailored->setInactive(false);
					}
				}

				// if current state is disease and NOT in Inactive state, increment num months in active (not Inactive)
				if ((currentState == ACTIVE  || currentState == SEVERE || currentState == COVID) && (month - tailoredPatient.monitoringState.monthLastMajorRelapse) < 6 && !tailoredPatient.generalState.inactiveDisease) {
					tailoredPatient.monitoringState.numMonthsSpentActive++;
				}

				// if current state is disease AND in Inactive state, increment num months in Inactive
				if ((currentState == ACTIVE || currentState == SEVERE || currentState == COVID || currentState == POST_MINOR_RELAPSE_ACTIVE_REMISSION) && (month - tailoredPatient.monitoringState.monthLastMajorRelapse) < 6 && tailoredPatient.generalState.inactiveDisease) {
					tailoredPatient.monitoringState.numMonthsSpentInactive++;
				}

				// checking if has exited initial state, set the exitedInitialState patient parameter to true if they have. 
				if (currentState != tailoredPatient.getGeneralState()->initialState && tailoredPatient.getGeneralState()->exitedInitialState == false) {
					stateUpdaterTailored->setExitedInitialState(true);
					if (tailoredPatient.getGeneralState()->initialState == ACTIVE && month - tailoredPatient.getMonitoringState()->monthLastMajorRelapse < 6) {
						stateUpdaterTailored->setExitedInitialState(false);
					}
				}
				// checking if covid vaccine is up to date
				if (tailoredPatient.getCovidState()->covidVacc && tailoredPatient.getCovidState()->vaccUpToDate &&  
					((tailoredPatient.getGeneralState()->monthNum - tailoredPatient.getCovidState()->monthCovidVaccine) <= tailoredPatient.getCovidState()->numMonthsVaccUpToDate)) {
					stateUpdaterTailored->setVaccUpToDate(true);
				}
				else {
					stateUpdaterTailored->setVaccUpToDate(false);
				}

				tailoredTraceFile << tailoredPatient.getGeneralState()->monthNum << " state: " << stateMapping[currentState] << ". ";
				if (tailoredPatient.getGeneralState()->inactiveDisease) {
					tailoredTraceFile << "In Inactive state. ";
				}
				tailoredTraceFile << "Previous Active: " << tailoredPatient.diseaseState.prevActiveDisease <<". ";
				tailoredTraceFile << "Last Major Relapse: " << tailoredPatient.monitoringState.monthLastMajorRelapse << ". ";
				tailoredTraceFile << "\n";
				
				if (esrd_this_month == true) {
					stateUpdaterTailored->updateESRDStatus(true);
					tailoredTraceFile << month << ": ESRD. " << '\n';
				}
			
				if (diabetes_this_month == true) {
					stateUpdaterTailored->updateDiabetesStatus(true);
					tailoredTraceFile << month << ": Diabetes. " << '\n';
				}
				
				// roll for renal involvement if major relapse occurred for those without renal involvement
				if (currentState == ACTIVE && prevState != ACTIVE && !tailoredPatient.getGeneralState()->renalInv && (month - tailoredPatient.monitoringState.monthLastMajorRelapse) < 6) {
					bool renal = probOfHappening(simContext->getCohortParamsInputs()->probRenalFromActive);
					if (renal == true && !tailoredPatient.getGeneralState()->renalInv) {
						stateUpdaterTailored->updateRenalInv(true);
					}
				}

				// count up number of patients w/ renal involvement by month
				if (tailoredPatient.getGeneralState()->renalInv) {
					renalInvTailoredMonthly[month]++;
				}
				
				// writing anca and b cell rise to output file when/if they happen. THIS IS JUST FOR THE OUTPUT FILE and is not critical to how the model runs. 
				if (anca_rise_this_month == true) {
					num_anca_rise_tailored++;
					tailoredTraceFile << "ANCA Rise" << endl;
					if (rtx_missed_this_month == false && tailoredPatient.getGeneralState()->state != DEATH) {
						tailoredTraceFile << month << ": Rituximab" << endl;
						num_rituximab_tailored++; // count up number of rituximab infusions for b cell arm
						stateUpdaterTailored->updateMonthLastRTX(month);
					}
					else {
						if (tailoredPatient.getGeneralState()->state != DEATH) {
							missed_rtx_tailored++;
							tailoredTraceFile << "No Rituximab" << endl;
						}
					}
				}
				if (bcell_rise_this_month == true) {
					num_bcell_rise_tailored++;
					stateUpdaterTailored->updateHadBCellRise(true);
					tailoredTraceFile << "B Cell Rise" << endl;
					if (rtx_missed_this_month == false && tailoredPatient.getGeneralState()->state != DEATH) {
						tailoredTraceFile << month << ": Rituximab" << endl;
						num_rituximab_tailored++; // count up number of rituximab infusions for b cell arm
						stateUpdaterTailored->updateMonthLastRTX(month);
					}
					else {
						if (tailoredPatient.getGeneralState()->state != DEATH) {
							missed_rtx_tailored++;
							tailoredTraceFile << "No Rituximab" << endl;
						}
					}
				}
				if (covid_hospitalization_this_month == true) {
					covid_hospitalizations_tailored++;
					stateUpdaterTailored->updateHadCovidHospitalization(true);
					tailoredTraceFile << "Hospitalization due to Covid" << endl;
				}

				nextState = tailoredUpdateState(&tailoredPatient, simContext, stateUpdaterTailored);
				// CHANGED TRANSITION TIMING
				// counting up months since relapse if it has happened during trial 
				if ((currentState == POST_MINOR_RELAPSE_REMISSION || currentState == POST_MINOR_RELAPSE_ACTIVE_REMISSION) ){//&& month < simContext->getCohortParamsInputs()->endMonth) {
					stateUpdaterTailored->updateHadMinorRelapse(true);
					if (prevState == REMISSION && currentState == POST_MINOR_RELAPSE_REMISSION) {
						tailoredPatient.monitoringState.hadMinorRelapseFromRemission = true;
					}
					else if (prevState == ACTIVE) {
						tailoredPatient.monitoringState.hadMinorRelapseFromInactiveDisease = true;
					}
				}

				/* this conditional is just for the trace file, so that the minor relapse trace appears during the month before the patient actually enters the Post-Minor Relapse Remission or Post Minor Relapse Active Remission state.*/
				/* we can get rid of this after reconfiguring the trace system */
				if (currentState == REMISSION && nextState == POST_MINOR_RELAPSE_REMISSION) {
					tailoredTraceFile << "Month " << month << ": Minor Relapse from Remission.\n";
				}
				else if (currentState == ACTIVE) {
					tailoredTraceFile << "Month " << month << ": Minor Relapse from Inactive.\n";
				}

				// if remission to disease state, set had major relapse from remission to be true
				// CHANGED TRANSITION TIMING
				if (prevState == REMISSION && currentState == ACTIVE) {
					tailoredPatient.monitoringState.hadMajorRelapseFromRemission = true;
				}

				/* this conditional is just for the trace file, so that the major relapse trace appears during the month before the patient actually enters the Active state.*/
				if (currentState == REMISSION && nextState == ACTIVE) {
					tailoredTraceFile << "Month " << tailoredPatient.getGeneralState()->monthNum << ": Major Relapse from Remission. Length of Active drawn to be " << tailoredPatient.getGeneralState()->lengthActiveToInactive << " months." << endl;
				}


				/* In future versions of the model, this will not be necessary if we reconfigure trace files to be globally accessible like in CEPAC.*/
				if (tailoredPatient.getMonitoringState()->majorRelapseFromInactiveDiseaseThisMonth == true) {
					tailoredTraceFile << "Month " << month << ": Major Relapse from Inactive. Length of Active drawn to be " << tailoredPatient.getGeneralState()->lengthActiveToInactive << " months." << endl;
					tailoredPatient.monitoringState.hadMajorRelapseFromInactiveDisease = true;
					tailoredPatient.monitoringState.majorRelapseFromInactiveDiseaseThisMonth = false;		
				}

				if (currentState == SEVERE && prevState != SEVERE) {
					stateUpdaterTailored->updateHadSevereInfection(true);
				}

				//count up deaths by cause
				// CHANGED TRANSITION TIMING
				if (currentState == DEATH && prevState != DEATH) {
					stateUpdaterTailored->setAliveStatus(false);
					if (prevState == REMISSION || prevState == POST_MINOR_RELAPSE_REMISSION || prevState == POST_RX_REMISSION) {
						remission_death_tailored++;
					}
					else if (prevState == ACTIVE) {
						bool aav_death = probOfHappening(simContext->getRemissionRelapseMortalityInputs()->pAAV_death);
						if (aav_death == true) { // roll for whether or not death was actually caused by active disease or if just background (remission) death
							active_death_tailored++;
						}
						else {
							remission_death_tailored++;
						}
					}
					else if (prevState == SEVERE) {
						// can add an increment total infection death here if desired
						if (twoMonthsAgoState == ACTIVE) {
							infection_from_active_death_tailored++;
						}
						else { // CHANGED
							infection_from_remission_death_tailored++;
						}
					}

					else if (currentState == COVID) {
						covid_death_tailored++;
					}
					
					// Counting deaths with Diabetes
					if (tailoredPatient.getDiseaseState()->hasDiabetes) {
						diabetes_death_tailored++;
					}

					// Counting deaths with ESRD
					if (tailoredPatient.getDiseaseState()->hasESRD) {
						esrd_death_tailored++;
					}

					
				}

				// setting prior covid immunity if patient had covid
				if (currentState == COVID) {
					stateUpdaterTailored->setPriorCovidImmunity(true);
					stateUpdaterTailored->updateHadCovid(true);
				}

				// setting whether major or minor relapse has occurred.
				if (tailoredPatient.getMonitoringState()->hadMajorRelapseFromRemission || tailoredPatient.getMonitoringState()->hadMajorRelapseFromInactiveDisease) {
					tailoredPatient.monitoringState.hadMajorRelapse = true;
				}
				if (tailoredPatient.getMonitoringState()->hadMinorRelapseFromRemission || tailoredPatient.getMonitoringState()->hadMinorRelapseFromInactiveDisease) {
					tailoredPatient.monitoringState.hadMinorRelapse = true;
				}

				// monthly counters for whether or not an event has occurred
				if (!tailoredPatient.getMonitoringState()->hadMajorRelapse && !tailoredPatient.getMonitoringState()->hadMinorRelapse) {
					relapseFreeTailored[month]++;
				}
				if (tailoredPatient.getMonitoringState()->hadMajorRelapse) {
					_majorRelapseTailored[month]++;
				}
				if (tailoredPatient.getMonitoringState()->hadMinorRelapse) {
					_minorRelapseTailored[month]++;
				}
				if (tailoredPatient.getMonitoringState()->hadSevereInfection) {
					_severeTailored[month]++;
				}
				if (tailoredPatient.getDiseaseState()->hasESRD) {
					_esrdTailored[month]++;
				}
				if (tailoredPatient.getMonitoringState()->hadCovid) {
					_covidTailored[month]++;
				}
				if (tailoredPatient.getMonitoringState()->hadCovidHospitalization) {
					_covidHospTailored[month]++;
				}
				if (tailoredPatient.getMonitoringState()->hadCovidDeath) {
					_covidDeathTailored[month]++;
				}
				if (tailoredPatient.getDiseaseState()->hasDiabetes) {
					_diabetesTailored[month]++;
				}
				if (currentState == DEATH) {
					_deathTailored[month]++;
				}
				if (tailoredPatient.getCovidState()->covidVacc) {
					numberVaccinatedTailored[month]++;
				}
				if (tailoredPatient.getCovidState()->vaccUpToDate) {
					_vaccineUpToDateTailored[month]++;
				}

				//counting up monthsSince parameters
				monthsSinceLastBCellRise++;

				//SETTING STATE FOR PATIENT if before endmonth
				if (month < endMonth) {
					stateUpdaterTailored->setPreviousExclusiveState(currentState);
					stateUpdaterTailored->setCurrMutuallyExclusiveState(nextState);
					stateUpdaterTailored->setTwoMonthsAgoState(prevState);
				}

				/* Increment Month Num*/
				stateUpdaterTailored->incrementMonth();
			}
			tailoredTraceFile << "END SIMULATION FOR PATIENT " << numRun << '.' << '\n' << '\n';

			// count up agregate events (after last month)
			if (tailoredPatient.getMonitoringState()->hadMinorRelapseFromRemission || tailoredPatient.getMonitoringState()->hadMinorRelapseFromInactiveDisease) {
				minor_relapse_total_tailored++;
			}
			if (tailoredPatient.getMonitoringState()->hadMajorRelapseFromRemission || tailoredPatient.getMonitoringState()->hadMajorRelapseFromInactiveDisease) {
				major_relapse_total_tailored++;
			}
			if (tailoredPatient.getMonitoringState()->hadMinorRelapseFromRemission) {
				minor_relapse_from_remission_tailored++;
			}
			if (tailoredPatient.getMonitoringState()->hadMajorRelapseFromRemission) {
				major_relapse_from_remission_tailored++;
			}
			if (tailoredPatient.getMonitoringState()->hadMinorRelapseFromInactiveDisease) {
				minor_relapse_from_inactive_disease_tailored++;
			}
			if (tailoredPatient.getMonitoringState()->hadMajorRelapseFromInactiveDisease) {
				major_relapse_from_inactive_disease_tailored++;
			}
			months_active_disease_tailored += tailoredPatient.getMonitoringState()->numMonthsSpentActive;
			months_inactive_disease_tailored += tailoredPatient.getMonitoringState()->numMonthsSpentInactive;
			if (tailoredPatient.getDiseaseState()->hasESRD) {
				esrd_total_tailored++;
			}
			if (tailoredPatient.getMonitoringState()->hadSevereInfection) {
				severe_infection_total_tailored++;
			}
			if (!tailoredPatient.getMonitoringState()->hadMinorRelapse && !tailoredPatient.getMonitoringState()->hadMajorRelapse && tailoredPatient.getGeneralState()->state != DEATH) {
				relapse_free_survival_total_tailored++;
			}
			if (tailoredPatient.getMonitoringState()->hadCovid) {
				covid_infection_total_tailored++;
			}
			if (tailoredPatient.getDiseaseState()->hasDiabetes) {
				diabetes_total_tailored++;
			}
			if (tailoredPatient.getGeneralState()->state != DEATH) {
				survived_tailored++;
			}
			/* deallocate the stateUpdaterTailored object memory space */
			delete (stateUpdaterTailored);
			stateUpdaterTailored = NULL;

			/* FIXED ARM SIMULATION */

			/* Creating patient object */
			Patient fixedPatient = *patient;
			StateUpdater* stateUpdaterFixed = new StateUpdater(&fixedPatient);

			fixedTraceFile << "Patient " << numRun << ". " << "Sex: " << fixedPatient.getGeneralState()->gender << ". Age: " << fixedPatient.getGeneralState()->age << 
				". Type: " << fixedPatient.getGeneralState()->ancaType << ". Renal: " << fixedPatient.getGeneralState()->renalInv << 
				". Active (not Inactive) Length (months): " << fixedPatient.getGeneralState()->lengthActiveToInactive <<
				". Previous Active: " << fixedPatient.getDiseaseState()->prevActiveDisease  << '\n';
			
			/* Resetting month trackers */
			diabetes_this_month = false;
			esrd_this_month = false;
			anca_rise_this_month = false;
			bcell_rise_this_month = false;

			while (fixedPatient.getGeneralState()->monthNum <= simContext->getCohortParamsInputs()->endMonth) {
				int currentState = fixedPatient.getGeneralState()->state;
				int prevState = fixedPatient.getGeneralState()->prevState;
				int twoMonthsAgoState = fixedPatient.getGeneralState()->twoMonthsAgoState;
				int month = fixedPatient.getGeneralState()->monthNum;
				
				int nextState;

				/* if going into disease state from non-active, mark this month as the month of last major relapse. */
				// if coming from Active, Severe, Covid, or Post Minor Relapse from Inactive (called post_minor_relapse_active_remission still), DO NOT reset the month last major relapse because these states will be part of the current 6 months of Active.
				std::array<int, 4> possibleActiveStates = {ACTIVE, SEVERE, COVID, POST_MINOR_RELAPSE_ACTIVE_REMISSION};
				if (currentState == ACTIVE && std::find(possibleActiveStates.begin(), possibleActiveStates.end(), prevState) == possibleActiveStates.end()) {
					stateUpdaterFixed->updateMonthLastMajorRelapse(month);
				}

				// if no longer in disease state, turn off Inactive marker in patient
				if (std::find(possibleActiveStates.begin(), possibleActiveStates.end(), prevState) == possibleActiveStates.end()) {
					if (fixedPatient.getGeneralState()->inactiveDisease) {
						stateUpdaterFixed->setInactive(false);
					}
				}

				// if current state is disease and NOT in inactive state, increment num months in active (not Inactive)
				if ((currentState == ACTIVE  || currentState == SEVERE || currentState == COVID) && (month - fixedPatient.monitoringState.monthLastMajorRelapse) < 6 && !fixedPatient.generalState.inactiveDisease) {
					fixedPatient.monitoringState.numMonthsSpentActive++;
				}

				// if current state is disease AND in inactive state, increment num months in inactive
				if ((currentState == ACTIVE || currentState == SEVERE || currentState == COVID || currentState == POST_MINOR_RELAPSE_ACTIVE_REMISSION) && (month - fixedPatient.monitoringState.monthLastMajorRelapse) < 6 && fixedPatient.generalState.inactiveDisease) {
					fixedPatient.monitoringState.numMonthsSpentInactive++;
				}

				// checking if has exited initial state, set the exitedInitialState patient parameter to true if they have. 
				if (currentState != fixedPatient.getGeneralState()->initialState && fixedPatient.getGeneralState()->exitedInitialState == false) {
					stateUpdaterFixed->setExitedInitialState(true);
					if (fixedPatient.getGeneralState()->initialState == ACTIVE && month - fixedPatient.getMonitoringState()->monthLastMajorRelapse < 6) {
						stateUpdaterFixed->setExitedInitialState(false);
					}
				}

				// checking if covid vaccine is up to date
				if (fixedPatient.getCovidState()->covidVacc && fixedPatient.getCovidState()->vaccUpToDate && (month - fixedPatient.getCovidState()->monthCovidVaccine) <= fixedPatient.getCovidState()->numMonthsVaccUpToDate) {
					stateUpdaterFixed->setVaccUpToDate(true);
				}
				else {
					stateUpdaterFixed->setVaccUpToDate(false);
				}

				fixedTraceFile << month << " state: " << stateMapping[currentState] << ". ";
				if (fixedPatient.getGeneralState()->inactiveDisease) {
					fixedTraceFile << "In Inactive state. ";
				}
				fixedTraceFile << "Previous Active: " << fixedPatient.diseaseState.prevActiveDisease <<". ";
				fixedTraceFile << "\n";

				if (esrd_this_month == true) {
					stateUpdaterFixed->updateESRDStatus(true);
					fixedTraceFile << month << ": ESRD. " << '\n';
				}
				if (diabetes_this_month == true) {
					stateUpdaterFixed->updateDiabetesStatus(true);
					fixedTraceFile << month << ": Diabetes. " << '\n';
				}
				

				// roll for renal involvement if major relapse occurred for those without renal involvement
				if (currentState == ACTIVE && prevState != ACTIVE && !fixedPatient.getGeneralState()->renalInv && (month - fixedPatient.monitoringState.monthLastMajorRelapse) < 6) {
					bool renal = probOfHappening(simContext->getCohortParamsInputs()->probRenalFromActive); // roll for renal involvement
					if (renal == true) {
						stateUpdaterFixed->updateRenalInv(true);
					}
				}

				// count up number of patients w/ renal involvement by month
				if (fixedPatient.getGeneralState()->renalInv) {
					renalInvFixedMonthly[month]++;
				}

				if (covid_hospitalization_this_month == true) {
					covid_hospitalizations_fixed++;
					stateUpdaterFixed->updateHadCovidHospitalization(true);
					fixedTraceFile << "Hospitalization due to Covid" << endl;
				}

				// draw for next state
				nextState = fixedUpdateState(&fixedPatient, simContext, stateUpdaterFixed);

				// counting up months since relapse if it has happened during trial 
				if (currentState == POST_MINOR_RELAPSE_REMISSION || currentState == POST_MINOR_RELAPSE_ACTIVE_REMISSION) {
					stateUpdaterFixed->updateHadMinorRelapse(true);
					if (prevState == REMISSION && nextState == POST_MINOR_RELAPSE_REMISSION) {
						fixedPatient.monitoringState.hadMinorRelapseFromRemission = true;
					}
					else if (prevState == ACTIVE) {
						fixedPatient.monitoringState.hadMinorRelapseFromInactiveDisease = true;
					}
					
				}
				/* this conditional is just for the trace file, so that the minor relapse trace appears during the month before the patient actually enters the Post-Minor Relapse Remission or Post Minor Relapse Active Remission state.*/
				/* we can get rid of this after reconfiguring the trace system */
				if (currentState == REMISSION && nextState == POST_MINOR_RELAPSE_REMISSION) {
					fixedTraceFile << "Month " << month << ": Minor Relapse from Remission.\n";
				}
				else if (currentState == ACTIVE) {
					fixedTraceFile << "Month " << month << ": Minor Relapse from Inactive.\n";
				}
				/*
				if (nextState == ACTIVE) {
					//num_major_relapse++;
					if (fixedPatient.getGeneralState()->initialState != ACTIVE || fixedPatient.getGeneralState()->exitedInitialState) {
						stateUpdaterFixed->updateHadMajorRelapse(true);
					}
				}
				*/

				// if remission to active state, set had major relapse from remission to be true
				if (prevState == REMISSION && currentState == ACTIVE) {
					fixedPatient.monitoringState.hadMajorRelapseFromRemission = true;
				}

				/* this conditional is just for the trace file, so that the major relapse trace appears during the month before the patient actually enters the Active state.*/
				if (currentState == REMISSION && nextState == ACTIVE) {
					fixedTraceFile << "Month " << month << ": Major Relapse from Remission. Length of Active drawn to be " << fixedPatient.getGeneralState()->lengthActiveToInactive << " months." << endl;

				}

				/* In future versions of the model, this will not be necessary if we reconfigure trace files to be globally accessible like in CEPAC.*/
				if (fixedPatient.getMonitoringState()->majorRelapseFromInactiveDiseaseThisMonth == true) {
					fixedTraceFile << "Major Relapse from Inactive. Length of Active drawn to be " << fixedPatient.getGeneralState()->lengthActiveToInactive << " months." << endl;
					fixedPatient.monitoringState.hadMajorRelapseFromInactiveDisease = true;
					fixedPatient.monitoringState.majorRelapseFromInactiveDiseaseThisMonth = false;
				}

				/* THIS DOESNT ALWAYS WORK BECAUSE NEXTSTATE IS NOT ALWAYS ACTIVE EVEN IF PATIENT IS STILL ACTUALLY IN ACTIVE STATE (eg Severe or Covid from Active)
				if (currentState == ACTIVE && nextState == ACTIVE && (month - fixedPatient.getMonitoringState()->monthLastMajorRelapse) == 0) {
					fixedTraceFile << "Major Relapse from Inactive" << endl;
				}
				*/

				if (currentState == SEVERE && prevState != SEVERE) {
					stateUpdaterFixed->updateHadSevereInfection(true);
				}

				// count up deaths by cause
				if (currentState == DEATH && prevState != DEATH) {
					if (prevState == REMISSION || prevState == POST_RX_REMISSION || prevState == POST_MINOR_RELAPSE_REMISSION) {
						remission_death_fixed++;
					}
					else if (prevState == ACTIVE) {
						bool aav_death = probOfHappening(simContext->getRemissionRelapseMortalityInputs()->pAAV_death);
						if (aav_death == true) {
							active_death_fixed++;
						}
						else {
							remission_death_fixed++;
						}
					}
					else if (prevState == SEVERE) {
						// can add an increment total infection death here if desired
						if (twoMonthsAgoState == ACTIVE) {
							infection_from_active_death_fixed++;
						}
						else { // CHANGED
							infection_from_remission_death_fixed++;
						}
					}

					else if (currentState == COVID) {
						covid_death_fixed++;
						stateUpdaterFixed->updateHadCovidDeath(true);
					}

					// Counting deaths with ESRD
					if (fixedPatient.getDiseaseState()->hasESRD) {
						esrd_death_fixed++;
					}
					// Counting deaths with Diabetes
					if (fixedPatient.getDiseaseState()->hasDiabetes) {
						diabetes_death_fixed++;
					}
				}
				
				if (currentState == COVID) { // if has covid at any point, set had_covid to true
					stateUpdaterFixed->updateHadCovid(true);
				}
				

				if (month - fixedPatient.getTreatment()->monthLastRTX >= 6 && month <= monthStopRTX && fixedPatient.getGeneralState()->state != DEATH) { // do rituximab infusion every 6 months untill rituximab is stopped
					num_rituximab_fixed++;
					stateUpdaterFixed->updateMonthLastRTX(month);
				}

				// setting whether major or minor relapse has occurred.
				if (fixedPatient.getMonitoringState()->hadMajorRelapseFromRemission || fixedPatient.getMonitoringState()->hadMajorRelapseFromInactiveDisease) {
					fixedPatient.monitoringState.hadMajorRelapse = true;
				}
				if (fixedPatient.getMonitoringState()->hadMinorRelapseFromRemission || fixedPatient.getMonitoringState()->hadMinorRelapseFromInactiveDisease) {
					fixedPatient.monitoringState.hadMinorRelapse = true;
				}
				// monthly counters for whether or not events have occurred
				if (!fixedPatient.getMonitoringState()->hadMajorRelapse && !fixedPatient.getMonitoringState()->hadMinorRelapse) { // if no major or minor relapse
					relapseFreeFixed[month]++;
				}
				if (fixedPatient.getMonitoringState()->hadMajorRelapse) {
					_majorRelapseFixed[month]++;
				}
				if (fixedPatient.getMonitoringState()->hadMinorRelapse) {
					_minorRelapseFixed[month]++;
				}
				if (fixedPatient.getMonitoringState()->hadCovid) {
					_covidFixed[month]++;
				}
				if (fixedPatient.getMonitoringState()->hadSevereInfection) {
					_severeFixed[month]++;
				}
				if (fixedPatient.getDiseaseState()->hasESRD) {
					_esrdFixed[month]++;
				}
				if (fixedPatient.getDiseaseState()->hasDiabetes) {
					_diabetesFixed[month]++;
				}
				if (fixedPatient.getMonitoringState()->hadCovidHospitalization) {
					_covidHospFixed[month]++;
				}
				if (fixedPatient.getMonitoringState()->hadCovidDeath) {
					_covidDeathFixed[month]++;
				}
				if (currentState == DEATH) {
					_deathFixed[month]++;
				}
				if (fixedPatient.getCovidState()->covidVacc) {
					numberVaccinatedFixed[month]++;
				}
				if (fixedPatient.getCovidState()->vaccUpToDate) {
					_vaccineUpToDateFixed[month]++;
				}

				//counting up monthsSince parameters
				monthsSinceLastBCellRise++;

				//SETTING STATE FOR PATIENT if before endmonth
				if (month < endMonth) {
					stateUpdaterFixed->setPreviousExclusiveState(currentState);
					stateUpdaterFixed->setCurrMutuallyExclusiveState(nextState);
					stateUpdaterFixed->setTwoMonthsAgoState(prevState);
				}

				/* Increment month num */
				stateUpdaterFixed->incrementMonth();
			}
			fixedTraceFile << "END SIMULATION FOR PATIENT " << numRun << '.' << '\n' << '\n';

			// count up aggregate events (after the last month)
			if (fixedPatient.getMonitoringState()->hadMinorRelapse) {
				minor_relapse_total_fixed++;
			}
			if (fixedPatient.getMonitoringState()->hadMajorRelapse) {
				major_relapse_total_fixed++;
			}
			if (fixedPatient.getMonitoringState()->hadMinorRelapseFromRemission) {
				minor_relapse_from_remission_fixed++;
			}
			if (fixedPatient.getMonitoringState()->hadMajorRelapseFromRemission) {
				major_relapse_from_remission_fixed++;
			}
			if (fixedPatient.getMonitoringState()->hadMinorRelapseFromInactiveDisease) {
				minor_relapse_from_inactive_disease_fixed++;
			}
			if (fixedPatient.getMonitoringState()->hadMajorRelapseFromInactiveDisease) {
				major_relapse_from_inactive_disease_fixed++;
			}
			months_active_disease_fixed += fixedPatient.getMonitoringState()->numMonthsSpentActive;
			months_inactive_disease_fixed += fixedPatient.getMonitoringState()->numMonthsSpentInactive;
			if (fixedPatient.getDiseaseState()->hasESRD) {
				esrd_total_fixed++;
			}
			if (fixedPatient.getMonitoringState()->hadSevereInfection) {
				severe_infection_total_fixed++;
			}
			if (!fixedPatient.getMonitoringState()->hadMajorRelapse && !fixedPatient.getMonitoringState()->hadMinorRelapse && fixedPatient.getGeneralState()->state != DEATH) {
				relapse_free_survival_total_fixed++;
			}
			if (fixedPatient.getMonitoringState()->hadCovid) {
				covid_infection_total_fixed++;
			}
			if (fixedPatient.getDiseaseState()->hasDiabetes) {
				diabetes_total_fixed++;
			}
			if (fixedPatient.getGeneralState()->state != DEATH) {
				survived_fixed++;
			}

			/* Deallocate the stateUpdaterFixed object memory space */
			delete (stateUpdaterFixed);
			stateUpdaterFixed = NULL;

			/* B CELL ARM SIMULATION*/

			/* Creating patient object */
			Patient bCellPatient = *patient;
			StateUpdater* stateUpdaterBCell = new StateUpdater(&bCellPatient);
			int treatment = BCELL_UNDET;
			bool vaccinatedDuringTrial = false;
			int monthsSinceVaccination = 0;
			int monthsSinceRelapse = 0;

			bCellTraceFile << "Patient " << numRun << ". " << "Sex: " << bCellPatient.getGeneralState()->gender << ". Age: " << bCellPatient.getGeneralState()->age << 
				". Type: " << bCellPatient.getGeneralState()->ancaType << ". Renal: " << bCellPatient.getGeneralState()->renalInv << ". Months Vacc Up To Date: " << bCellPatient.getCovidState()->numMonthsVaccUpToDate << 
				". Active (not Inactive) Length (months): " << bCellPatient.getGeneralState()->lengthActiveToInactive << 
				". Previous Active: " << bCellPatient.getDiseaseState()->prevActiveDisease <<'\n';
			
			/* Resetting month trackers */
			diabetes_this_month = false;
			esrd_this_month = false;
			anca_rise_this_month = false;
			bcell_rise_this_month = false;

			while (bCellPatient.getGeneralState()->monthNum <= simContext->getCohortParamsInputs()->endMonth) {
				int currentState = bCellPatient.getGeneralState()->state;
				int prevState = bCellPatient.getGeneralState()->prevState;
				int twoMonthsAgoState = bCellPatient.getGeneralState()->twoMonthsAgoState;
				int month = bCellPatient.getGeneralState()->monthNum;
				int nextState;

				/* if going into disease state from non-active, mark this month as the month of last major relapse. */
				// if coming from Active, Severe, Covid, or Post Minor Relapse from Inactive (called post_minor_relapse_active_remission still), DO NOT reset the month last major relapse because these states will be part of the current 6 months of Active.
				std::array<int, 4> possibleActiveStates = {ACTIVE, SEVERE, COVID, POST_MINOR_RELAPSE_ACTIVE_REMISSION};
				if (currentState == ACTIVE && std::find(possibleActiveStates.begin(), possibleActiveStates.end(), prevState) == possibleActiveStates.end()) {
					stateUpdaterBCell->updateMonthLastMajorRelapse(month);
				}

				// if no longer in disease state, turn off Inactive marker in patient
				if (std::find(possibleActiveStates.begin(), possibleActiveStates.end(), prevState) == possibleActiveStates.end()) {
					if (fixedPatient.getGeneralState()->inactiveDisease) {
						stateUpdaterBCell->setInactive(false);
					}
				}

				// if current state is disease and disease is active, increment num months in active (not Inactive)
				if ((currentState == ACTIVE  || currentState == SEVERE || currentState == COVID) && (month - bCellPatient.monitoringState.monthLastMajorRelapse) < 6 && !bCellPatient.generalState.inactiveDisease) {
					bCellPatient.monitoringState.numMonthsSpentActive++;
				}

				// if current state is disease AND disease is inactive, increment num months in inactive
				if ((currentState == ACTIVE || currentState == SEVERE || currentState == COVID || currentState == POST_MINOR_RELAPSE_ACTIVE_REMISSION) && (month - bCellPatient.monitoringState.monthLastMajorRelapse) < 6 && bCellPatient.generalState.inactiveDisease) {
					bCellPatient.monitoringState.numMonthsSpentInactive++;
				}

				// checking if has exited initial state, set the exitedInitialState patient parameter to true if they have. 
				if (currentState != bCellPatient.getGeneralState()->initialState && bCellPatient.getGeneralState()->exitedInitialState == false) {
					stateUpdaterBCell->setExitedInitialState(true);
					if (bCellPatient.getGeneralState()->initialState == ACTIVE && month - bCellPatient.getMonitoringState()->monthLastMajorRelapse < 6) {
						stateUpdaterBCell->setExitedInitialState(false);
					}
				}

				// checking if vaccine is up to date
				if (bCellPatient.getCovidState()->covidVacc && bCellPatient.getCovidState()->vaccUpToDate && monthsSinceVaccination <= bCellPatient.getCovidState()->numMonthsVaccUpToDate) {
					stateUpdaterBCell->setVaccUpToDate(true);
					
				}
				else {
					stateUpdaterBCell->setVaccUpToDate(false);
				}

				
		// writing anca and b cell rise to output file when/if they happen. Set patient history booleans (had_anca_rise, had_bcell_rise, etc)
				if (anca_rise_this_month == true) {
					stateUpdaterBCell->updateHadANCARise(true);
					num_anca_rise_bcell++;
				}
				if (bcell_rise_this_month == true) {
					stateUpdaterBCell->updateHadBCellRise(true);
					num_bcell_rise_bcell++;
				}

				// outputting state to trace file
				bCellTraceFile << month << " state: " << stateMapping[currentState] << ". ";
				if (bCellPatient.getGeneralState()->inactiveDisease) {
					bCellTraceFile << "In INACTIVE state. ";
				}
				bCellTraceFile << "Current strat is " << treatmentMapping[treatment] << ". ";
				if (bCellPatient.getCovidState()->vaccUpToDate) {
					bCellTraceFile << "Vaccine up to date. ";// << '\n';
				}
				else {
					bCellTraceFile << "Vaccine NOT up to date. ";// << '\n';
				}
				bCellTraceFile<< "Previous Active: " << bCellPatient.diseaseState.prevActiveDisease <<". ";
				bCellTraceFile << "\n";
				if (esrd_this_month == true) {
					stateUpdaterBCell->updateESRDStatus(true);
					bCellTraceFile << month << ": ESRD. " << '\n';
				}
				if (diabetes_this_month == true) {
					stateUpdaterBCell->updateDiabetesStatus(true);
					bCellTraceFile << month << ": Diabetes. " << '\n';
				}


			//
			// 
			// 
			// 
			//REVISION STARTING HERE
				//
				//
				//
				if (treatment == BCELL_UNDET || treatment == BCELL_REPOP) {
					if (bCellPatient.getMonitoringState()->hadBCellRise) { // if had b cell rise already, switch to bCellTailored_Rise treatment plan (only affects the odds, not the structure of the decision tree)
						treatment = BCELL_REPOP;
					}
					if (bCellPatient.getMonitoringState()->hadBCellRise && simContext->getCohortParamsInputs()->vaccAtBCellRise) {// if had b cell rise
						if (bcell_rise_this_month == true) {// if b cell rise happened in this particular month and vaccine also happened,
							bCellTraceFile << month << ": B Cell Rise" << endl;
							bCellTraceFile << "Vaccinated at month " << month << ". Wait " << simContext->getCohortParamsInputs()->vaccEffDelay << " months before taking effect" << endl;
							monthsSinceVaccination = 0;
							vaccinatedDuringTrial = true;
						}

						if (monthsSinceVaccination == simContext->getCohortParamsInputs()->vaccEffDelay) {
							stateUpdaterBCell->setCovidVaccine(true);
							stateUpdaterBCell->setVaccUpToDate(true);
							stateUpdaterBCell->setMonthOfCovidVaccine(month);
							bCellTraceFile << month << ": Vaccine now in effect." << endl;
							bCellTraceFile << "Vacc Up To Date is: " << bCellPatient.getCovidState()->vaccUpToDate << endl;
						}
						if (monthsSinceVaccination == simContext->getCohortParamsInputs()->stratDelayMonthsVacc) {
							if (simContext->getCohortParamsInputs()->stratAfterVacc == 0) {
								treatment = TAILORED;
								bCellTraceFile << "Strategy switched to Tailored at month " << month << '.' << endl;
								if (month <= monthStopRTX && bCellPatient.getGeneralState()->state != DEATH) {
									bCellTraceFile << month << ": Rituximab" << endl;
									num_rituximab_bcell++;
									// set month last RTX to current month
									stateUpdaterBCell->updateMonthLastRTX(month);
								}
								else {
									if (bCellPatient.getGeneralState()->state != DEATH) {
										bCellTraceFile << "No Rituximab" << endl;
										missed_rtx_bcell++;
									}
								}
								// roll for next state
								nextState = tailoredUpdateState(&bCellPatient, simContext, stateUpdaterBCell);
								if (nextState != DEATH) {
									nextState = POST_RX_REMISSION;
								}
								bcell_rise_this_month = false;
								
								
							}
							else {
								treatment = FIXED;
								bCellTraceFile << "Strategy switched to Fixed at month " << month << '.' << endl;
								if (month <= monthStopRTX && bCellPatient.getGeneralState()->state != DEATH) {
									bCellTraceFile << month << ": Rituximab" << endl;
									num_rituximab_bcell++;
									// set month last RTX to current month
									stateUpdaterBCell->updateMonthLastRTX(month);
								}
								else {
									if (bCellPatient.getGeneralState()->state != DEATH) {
										bCellTraceFile << "No Rituximab" << endl;
										missed_rtx_bcell++;
									}
								}
								// draw for next state
								nextState = fixedUpdateState(&bCellPatient, simContext, stateUpdaterBCell);
								
								
							}
						}
						else { // in between when b cell rises and strategy is changed due to vaccination
							// draw for next state
							nextState = bCellUpdateState(treatment, &bCellPatient, simContext, stateUpdaterBCell);
							
						}
					}
					// if no b cell rise yet or no change in strat due to b cell rise, continue on with the b cell arm unless there is relapse
					else {
						// draw for next state
						nextState = bCellUpdateState(treatment, &bCellPatient, simContext, stateUpdaterBCell);
						
						if ((nextState == POST_MINOR_RELAPSE_REMISSION || nextState == POST_MINOR_RELAPSE_ACTIVE_REMISSION) && monthsSinceRelapse == 0) { // if minor relapse, output message in first month to trace file
							if (nextState == POST_MINOR_RELAPSE_REMISSION) {
								bCellTraceFile << "Minor Relapse (from Remission). ";
							}
							else if (nextState == POST_MINOR_RELAPSE_ACTIVE_REMISSION) {
								bCellTraceFile << "Minor Relapse (from Inactive). ";
							}
							
							if (simContext->getCohortParamsInputs()->includeMinorRelapse) {
								bCellTraceFile << "Wait " << simContext->getCohortParamsInputs()->stratDelayMonthsRelapse << " months before switching to " << simContext->getCohortParamsInputs()->stratAfterRelapse << " strategy.";
							}
							bCellTraceFile << endl;	
						}
						if ((nextState == ACTIVE || bCellPatient.getMonitoringState()->hadMajorRelapse) && simContext->getCohortParamsInputs()->changeStratAfterRelapse && (monthsSinceRelapse == simContext->getCohortParamsInputs()->stratDelayMonthsRelapse)) {
							// stuff that happens if Major Relapse
							if (simContext->getCohortParamsInputs()->stratAfterRelapse == TAILORED) {
								treatment = TAILORED;
								bCellTraceFile << month << ": Strategy switched to Tailored due to major relapse." << endl;
								if (month <= monthStopRTX && bCellPatient.getGeneralState()->state != DEATH) { // if prior to month when stop administration of rituximab, give rituximab
									bCellTraceFile << month << ": Rituximab" << endl;
									num_rituximab_bcell++;
									// set month last RTX to current month
									stateUpdaterBCell->updateMonthLastRTX(month);
								}
								else {
									if (bCellPatient.getGeneralState()->state != DEATH) {
										bCellTraceFile << "Missed RTX" << endl;
										missed_rtx_bcell++;
									}
								}
							}
							else if (simContext->getCohortParamsInputs()->stratAfterRelapse == FIXED) {
								treatment = FIXED; // fixed
								bCellTraceFile << "Major Relapse, Strategy switched to Fixed due to major relapse at month " << month << '.' << endl;
								if (month <= monthStopRTX && bCellPatient.getGeneralState()->state!=DEATH) { // if prior to month when stop administration of rituximab, give rituximab
									bCellTraceFile << month << ": Rituximab" << endl;
									num_rituximab_bcell++;
									// set month last RTX to current month
									stateUpdaterBCell->updateMonthLastRTX(month);
								}
								else {
									if (bCellPatient.getGeneralState()->state != DEATH) {
										bCellTraceFile << "Missed RTX" << endl;
										missed_rtx_bcell++;
									}
								}
							}
							else {
								bCellTraceFile << "next state is Active but ERROR: strategy string bad" << endl;
							}
							// set month last RTX to current month
							stateUpdaterBCell->updateMonthLastRTX(month);
						}
						if ((nextState == POST_MINOR_RELAPSE_REMISSION || bCellPatient.getMonitoringState()->hadMinorRelapse) && simContext->getCohortParamsInputs()->includeMinorRelapse && (monthsSinceRelapse == simContext->getCohortParamsInputs()->stratDelayMonthsRelapse)) {
							if (simContext->getCohortParamsInputs()->stratAfterRelapse == TAILORED) {
								treatment = TAILORED;
								bCellTraceFile << month << ": Strategy switched to Tailored due to previous minor relapse." << endl;
								if (month <= monthStopRTX) { // if prior to month when stop administration of rituximab, give rituximab
									bCellTraceFile << month + 1 << ": Rituximab" << endl;
									num_rituximab_bcell++;
									// set month last RTX to current month
									stateUpdaterBCell->updateMonthLastRTX(month);
								}
								else {
									bCellTraceFile << "Missed RTX" << endl;
									missed_rtx_bcell++;
								}
							}
							else if (simContext->getCohortParamsInputs()->stratAfterRelapse == FIXED) {
								treatment = FIXED;
								bCellTraceFile << month << ": Strategy switched to Fixed due to previous minor relapse." << endl;
								if (month <= monthStopRTX) { // if prior to month when stop administration of rituximab, give rituximab
									bCellTraceFile << month + 1 << ": Rituximab" << endl;
									num_rituximab_bcell++;
									// set month last RTX to current month
									stateUpdaterBCell->updateMonthLastRTX(month);
								}
								else {
									bCellTraceFile << "Missed RTX" << endl;
									missed_rtx_bcell++;
								}
							}
							else {
								bCellTraceFile << "next state is Post Minor Relapse but ERROR: strategy string bad" << endl;
							}
						}
					}
				}

				else if (treatment == TAILORED) {
					
					nextState = tailoredUpdateState(&bCellPatient, simContext, stateUpdaterBCell);
					

					/* If had either B Cell rise or ANCA rise, count the events and administer Rituximab as needed*/
					if ((bcell_rise_this_month || anca_rise_this_month)) {
						// if had b cell rise, count event
						if (bcell_rise_this_month) {
							stateUpdaterBCell->updateHadBCellRise(true);
							bCellTraceFile << month << ": B Cell Rise" << endl;
						}
						// if had anca rise, count event
						else {
							bCellTraceFile << month << ": ANCA Rise" << endl;
						}
						// if month is less than the month in which rituximab is stopped, administer rituximab
						if (month <= monthStopRTX) {
							bCellTraceFile << month << ": Rituximab" << endl;
							num_rituximab_bcell++;
							// set month last RTX to current month
							stateUpdaterBCell->updateMonthLastRTX(month);
						}
						// if month is greater than the month in which rituximab is stopped
						else {
							bCellTraceFile << "No Rituximab" << endl;
							missed_rtx_bcell++;
						}
					}
				}

				else { // if treatment is fixed
					nextState = fixedUpdateState(&bCellPatient, simContext, stateUpdaterBCell);
					
					if (month - bCellPatient.getTreatment()->monthLastRTX >= 6) {
						if (month - bCellPatient.getTreatment()->monthLastRTX <= monthStopRTX) {
							bCellTraceFile << month << ": Rituximab" << endl;
							num_rituximab_bcell++;
							// set month last RTX to current month
							stateUpdaterBCell->updateMonthLastRTX(month);
						}
						else {
							bCellTraceFile << "No Rituximab" << endl;
							missed_rtx_bcell++;
						}
					}
				}
				
				if (covid_hospitalization_this_month == true) {
					stateUpdaterBCell->updateHadCovidHospitalization(true);
					covid_hospitalizations_bcell++;
					bCellTraceFile << "Hospitalization due to Covid" << endl;
				}

				// counting up months since relapse if it has happened during trial 
				if (currentState == POST_MINOR_RELAPSE_REMISSION || currentState == POST_MINOR_RELAPSE_ACTIVE_REMISSION) {
					stateUpdaterBCell->updateHadMinorRelapse(true);
					if (prevState == REMISSION && nextState == POST_MINOR_RELAPSE_REMISSION) {
						bCellPatient.monitoringState.hadMinorRelapseFromRemission = true;
					}
					else if (prevState == ACTIVE) {
						bCellPatient.monitoringState.hadMinorRelapseFromInactiveDisease = true;
					}
				}

				/* this conditional is just for the trace file, so that the minor relapse trace appears during the month before the patient actually enters the Post-Minor Relapse Remission or Post Minor Relapse Active Remission state.*/
				/* we can get rid of this after reconfiguring the trace system */
				if (currentState == REMISSION && nextState == POST_MINOR_RELAPSE_REMISSION) {
					bCellTraceFile << "Month " << month << ": Minor Relapse from Remission.\n";
				}
				else if (currentState == ACTIVE) {
					bCellTraceFile << "Month " << month << ": Minor Relapse from Inactive.\n";
				}

				/*
				if (nextState == ACTIVE) { // if nextstate is active, set had major relapse to true already (don't wait until currentstate is active, or will wait one extra month to start counting months since relapse). 
					//num_major_relapse++;
					if (bCellPatient.getGeneralState()->initialState != ACTIVE || bCellPatient.getGeneralState()->exitedInitialState) {
						stateUpdaterBCell->updateHadMajorRelapse(true);
					}
				}
				*/

				// if remission to active state, set had major relapse from remission to be true
				if (prevState == REMISSION && currentState == ACTIVE) {
					bCellPatient.monitoringState.hadMajorRelapseFromRemission = true;
				}

				/* this conditional is just for the trace file, so that the major relapse trace appears during the month before the patient actually enters the Active state.*/
				if (currentState == REMISSION && nextState == ACTIVE) {
					bCellTraceFile << "Month " << month << ": Major Relapse from Remission. Length of Active (not Inactive) drawn to be " << bCellPatient.getGeneralState()->lengthActiveToInactive << " months." << endl;
				}
				
				/* In future versions of the model, this will not be necessary if we reconfigure trace files to be globally accessible like in CEPAC.*/
				if (bCellPatient.getMonitoringState()->majorRelapseFromInactiveDiseaseThisMonth == true) {
					bCellTraceFile << "Major Relapse from Inactive. Length of Active (not Inactive) drawn to be " << bCellPatient.getGeneralState()->lengthActiveToInactive << " months." << endl;
					bCellPatient.monitoringState.hadMajorRelapseFromInactiveDisease = true;
					bCellPatient.monitoringState.majorRelapseFromInactiveDiseaseThisMonth = false;
				}
				/*
				if (bCellPatient.getGeneralState()->inactiveDisease && currentState == ACTIVE && nextState == ACTIVE && (month - bCellPatient.getMonitoringState()->monthLastMajorRelapse) == 0) {
					bCellTraceFile << "Major Relapse from Inactive" << endl;
				} 
				*/

				if (bCellPatient.getMonitoringState()->hadMajorRelapse || (bCellPatient.getMonitoringState()->hadMinorRelapse && simContext->getCohortParamsInputs()->includeMinorRelapse)) {
					monthsSinceRelapse++;
				}
				// counting up months since vaccination if it has happened during trial
				if (bCellPatient.getCovidState()->covidVacc) {
					monthsSinceVaccination++;
				}

			// Counting up events and setting event conditionals from here on out
				

				if (currentState == ACTIVE && prevState != ACTIVE && !bCellPatient.getGeneralState()->renalInv && (month - bCellPatient.monitoringState.monthLastMajorRelapse) < 6) {
					// roll for renal involvement if major relapse occurred for those without renal involvement
					bool renal = probOfHappening(simContext->getCohortParamsInputs()->probRenalFromActive);
					if (renal == true && !bCellPatient.getGeneralState()->renalInv) {
						stateUpdaterBCell->updateRenalInv(true);
					}
				}

				if (currentState == SEVERE && prevState != SEVERE) {
					stateUpdaterBCell->updateHadSevereInfection(true);
				}

				if (currentState == COVID) { // if has covid at any point, set had_covid to true
					stateUpdaterBCell->updateHadCovid(true);
				}

				
				
				// if death, count up deaths by cause
				if (currentState == DEATH && prevState != DEATH) {
					if (prevState == REMISSION || prevState == POST_MINOR_RELAPSE_REMISSION || prevState == POST_RX_REMISSION) {
						remission_death_bcell++;
					}
					else if (prevState == ACTIVE) {
						bool aav_death = probOfHappening(simContext->getRemissionRelapseMortalityInputs()->pAAV_death);
						if (aav_death == true) { // roll for whether or not death was actually caused by active disease or if just background (remission) death
							active_death_bcell++;
						}
						else {
							remission_death_bcell++;
						}
					}
					else if (prevState == SEVERE) {
						// can add an increment total infection death here if desired
						if (twoMonthsAgoState == ACTIVE) {
							infection_from_active_death_bcell++;
						}
						else { // else if remission
							infection_from_remission_death_bcell++;
						}
					}

					else if (prevState == COVID) {
						covid_death_bcell++;
						stateUpdaterBCell->updateHadCovidDeath(true);
					}
					// Counting deaths with ESRD
					if (bCellPatient.getDiseaseState()->hasESRD) {
						esrd_death_bcell++;
					}

					// Counting deaths with Diabetes
					if (bCellPatient.getDiseaseState()->hasDiabetes) {
						diabetes_death_bcell++;
					}
				}

				// setting whether major and minor relapse has occurred.
				if (bCellPatient.getMonitoringState()->hadMajorRelapseFromRemission || bCellPatient.getMonitoringState()->hadMajorRelapseFromInactiveDisease) {
					bCellPatient.monitoringState.hadMajorRelapse = true;
				}
				
				// Counting monthly patient statuses by month
				if (bCellPatient.getMonitoringState()->hadMajorRelapse) {
					_majorRelapseBCell[month]++;
				}
				if (bCellPatient.getMonitoringState()->hadMinorRelapse) {
					_minorRelapseBCell[month]++;
				}
				if (bCellPatient.getMonitoringState()->hadBCellRise) {
					bCellRepop_BCell[month]++; // will end up counting number of patients that have had b cell rise each month
				}
				if (bCellPatient.getMonitoringState()->hadCovid) {
					_covidBCell[month]++;
				}
				if (bCellPatient.getMonitoringState()->hadCovidHospitalization) {
					_covidHospBCell[month]++;
				}
				if (bCellPatient.getMonitoringState()->hadCovidDeath) {
					_covidDeathBCell[month]++;
				}
				if (bCellPatient.getMonitoringState()->hadSevereInfection) {
					_severeBCell[month]++;
				}
				if (!bCellPatient.getMonitoringState()->hadMajorRelapse && !bCellPatient.getMonitoringState()->hadMinorRelapse && bCellPatient.getMonitoringState()->hadBCellRise) {
					vaccineEligible_BCell[month]++; // if relapse free and has had b cell rise, patient is vaccine eligible
				}

				if (bCellPatient.getDiseaseState()->hasESRD) {
					_esrdBCell[month]++;
				}
				if (bCellPatient.getDiseaseState()->hasDiabetes) {
					_diabetesBCell[month]++;
				}
				// count up number of patients w/ renal involvement by month
				if (bCellPatient.getGeneralState()->renalInv) {
					renalInvBCellMonthly[month]++;
				}
				
				// count up number of patients who are relapse free
				if (!bCellPatient.getMonitoringState()->hadMajorRelapse && !bCellPatient.getMonitoringState()->hadMinorRelapse) {
					relapseFreeBCell[month]++; // will end up counting number of patients that have had major or minor relapse each month
				}
				if (currentState == DEATH) {
					_deathBCell[month]++; // count up total b cell deaths by month
				}
				// count up number of patients who are vaccinated by month
				if (bCellPatient.getCovidState()->covidVacc) {
					numberVaccinatedBCell[month]++;
				}
				if (bCellPatient.getCovidState()->vaccUpToDate) {
					_vaccineUpToDateBCell[month]++;
				}
					//counting up monthsSince parameters
				monthsSinceLastBCellRise++;

				//SETTING STATE FOR PATIENT if before endmonth
				if (month < endMonth) {
					stateUpdaterBCell->setPreviousExclusiveState(currentState);
					stateUpdaterBCell->setCurrMutuallyExclusiveState(nextState);
					stateUpdaterBCell->setTwoMonthsAgoState(prevState);
				}

				/* Increment Month Num*/
				stateUpdaterBCell->incrementMonth();

			}
			bCellTraceFile << "END SIMULATION FOR PATIENT " << numRun << '.' << '\n' << '\n';

			// count up agregate events (after last month)
			if (bCellPatient.getMonitoringState()->hadMinorRelapse) {
				minor_relapse_total_bcell++;
			}
			if (bCellPatient.getMonitoringState()->hadMajorRelapse) {
				major_relapse_total_bcell++;
			}
			if (bCellPatient.getMonitoringState()->hadMinorRelapseFromRemission) {
				minor_relapse_from_remission_bcell++;
			}
			if (bCellPatient.getMonitoringState()->hadMajorRelapseFromRemission) {
				major_relapse_from_remission_bcell++;
			}
			if (bCellPatient.getMonitoringState()->hadMinorRelapseFromInactiveDisease) {
				minor_relapse_from_inactive_disease_bcell++;
			}
			if (bCellPatient.getMonitoringState()->hadMajorRelapseFromInactiveDisease) {
				major_relapse_from_inactive_disease_bcell++;
			}
			months_active_disease_bcell += bCellPatient.getMonitoringState()->numMonthsSpentActive;
			months_inactive_disease_bcell += bCellPatient.getMonitoringState()->numMonthsSpentInactive;
			
			if (bCellPatient.getDiseaseState()->hasESRD) {
				esrd_total_bcell++;
			}
			if (bCellPatient.getMonitoringState()->hadSevereInfection) {
				severe_infection_total_bcell++;
			}
			if (!bCellPatient.getMonitoringState()->hadMinorRelapse && !bCellPatient.getMonitoringState()->hadMajorRelapse && bCellPatient.getGeneralState()->state != DEATH) {
				relapse_free_survival_total_bcell++;
			}
			if (bCellPatient.getMonitoringState()->hadCovid) {
				covid_infection_total_bcell++;
			}
			if (bCellPatient.getDiseaseState()->hasDiabetes) {
				diabetes_total_bcell++;
			}
			if (bCellPatient.getGeneralState()->state != DEATH) {
				survived_bcell++;
			}

			/* Deallocate the stateupdaterBCell object memory space */
			delete (stateUpdaterBCell);
			stateUpdaterBCell = NULL;
			
			delete (patient);
			patient = NULL;
			/* INCREMENT PATIENT NUMBER */
			numRun++;
			
		}
		tailoredTraceFile.close();
		fixedTraceFile.close();
		bCellTraceFile.close();

		/** outputting to final output document */
		float numPatients = (float)numCohortsLimit;
		//SETTING UP THE OUTPUTS
			// tailored arm outputs
		float proportion_minor_relapse_tailored = (float)minor_relapse_total_tailored / numPatients;
		float proportion_major_relapse_tailored = (float)major_relapse_total_tailored / numPatients;
		float proportion_minor_relapse_from_remission_tailored = (float)minor_relapse_from_remission_tailored / numPatients;
		float proportion_major_relapse_from_remission_tailored = (float)major_relapse_from_remission_tailored / numPatients;
		float proportion_minor_relapse_from_inactive_disease_tailored = (float)minor_relapse_from_inactive_disease_tailored / numPatients;
		float proportion_major_relapse_from_inactive_disease_tailored = (float)major_relapse_from_inactive_disease_tailored / numPatients;
		float proportion_severe_infection_tailored = (float)severe_infection_total_tailored / numPatients;
		float proportion_esrd_tailored = (float)esrd_total_tailored / numPatients;
		float proportion_covid_tailored = (float)covid_infection_total_tailored / numPatients;
		float proportion_covid_hosp_tailored = (float)covid_hospitalizations_tailored / numPatients;
		float proportion_diabetes_tailored = (float)diabetes_total_tailored / numPatients;
		float proportion_relapse_free_tailored = (float)relapse_free_survival_total_tailored / numPatients;
		float proportion_survived_tailored = (float)survived_tailored / numPatients;
		float mean_anca_rise_tailored = (float)num_anca_rise_tailored / numPatients;
		float mean_bcell_rise_tailored = (float)num_bcell_rise_tailored / numPatients;
		float mean_missed_rtx_tailored = (float)missed_rtx_tailored / numPatients;

		// fixed arm outputs
		float proportion_minor_relapse_fixed = (float)minor_relapse_total_fixed / numPatients;
		float proportion_major_relapse_fixed = (float)major_relapse_total_fixed / numPatients;
		float proportion_minor_relapse_from_remission_fixed = (float)minor_relapse_from_remission_fixed / numPatients;
		float proportion_major_relapse_from_remission_fixed = (float)major_relapse_from_remission_fixed / numPatients;
		float proportion_minor_relapse_from_inactive_disease_fixed = (float)minor_relapse_from_inactive_disease_fixed / numPatients;
		float proportion_major_relapse_from_inactive_disease_fixed = (float)major_relapse_from_inactive_disease_fixed / numPatients;
		float proportion_severe_infection_fixed = (float)severe_infection_total_fixed / numPatients;
		float proportion_esrd_fixed = (float)esrd_total_fixed / numPatients;
		float proportion_covid_fixed = (float)covid_infection_total_fixed / numPatients;
		float proportion_covid_hosp_fixed = (float)covid_hospitalizations_fixed / numPatients;
		float proportion_diabetes_fixed = (float)diabetes_total_fixed / numPatients;
		float proportion_relapse_free_fixed = (float)relapse_free_survival_total_fixed / numPatients;
		float proportion_survived_fixed = (float)survived_fixed / numPatients;
		float mean_anca_rise_fixed = (float)num_anca_rise_fixed / numPatients;
		float mean_bcell_rise_fixed = (float)num_bcell_rise_fixed / numPatients;
		float mean_missed_rtx_fixed = (float)missed_rtx_fixed / numPatients;

		// b cell arm outputs
		float proportion_minor_relapse_bcell = (float)minor_relapse_total_bcell / (float)numPatients;
		float proportion_major_relapse_bcell = (float)major_relapse_total_bcell / (float)numPatients;
		float proportion_minor_relapse_from_remission_bcell = (float)minor_relapse_from_remission_bcell / (float)numPatients;
		float proportion_major_relapse_from_remission_bcell = (float)major_relapse_from_remission_bcell / (float)numPatients;
		float proportion_minor_relapse_from_inactive_disease_bcell = (float)minor_relapse_from_inactive_disease_bcell / numPatients;
		float proportion_major_relapse_from_inactive_disease_bcell = (float)major_relapse_from_inactive_disease_bcell / numPatients;
		float proportion_severe_infection_bcell = (float)severe_infection_total_bcell / (float)numPatients;
		float proportion_esrd_bcell = (float)esrd_total_bcell / (float)numPatients;
		float proportion_covid_bcell = (float)covid_infection_total_bcell / numPatients;
		float proportion_covid_hosp_bcell = (float)covid_hospitalizations_bcell / numPatients;
		float proportion_diabetes_bcell = (float)diabetes_total_bcell / numPatients;
		float proportion_relapse_free_bcell = (float)relapse_free_survival_total_bcell / (float)numPatients;
		float proportion_survived_bcell = (float)survived_bcell / (float)numPatients;
		float mean_anca_rise_bcell = (float)num_anca_rise_bcell / (float)numPatients;
		float mean_bcell_rise_bcell = (float)num_bcell_rise_bcell / (float)numPatients;
		float mean_missed_rtx_bcell = (float)missed_rtx_bcell / (float)numPatients;

		outputFile.open(resultsSubDirectory + "/" +  rawFileName + "_outputs.csv");
		outputFile << " " << "," << "Tailored, " << "Fixed," << "B cell, \n";
		outputFile << "All Minor Relapses, " << proportion_minor_relapse_tailored <<  ',' << proportion_minor_relapse_fixed << ',' << proportion_minor_relapse_bcell << ',' << '\n';
		outputFile << "All Major Relapses, " << proportion_major_relapse_tailored << ',' << proportion_major_relapse_fixed << ',' << proportion_major_relapse_bcell << ',' << '\n';
		outputFile << "Minor Relapse from Remission, " << proportion_minor_relapse_from_remission_tailored << ',' << proportion_minor_relapse_from_remission_fixed << ',' << proportion_minor_relapse_from_remission_bcell << ',' << "\n";
		outputFile << "Major Relapse from Remission, " << proportion_major_relapse_from_remission_tailored << ',' << proportion_major_relapse_from_remission_fixed << ',' << proportion_major_relapse_from_remission_bcell << ',' <<"\n";
		
		outputFile << "Minor Relapse from Inactive, " << proportion_minor_relapse_from_inactive_disease_tailored << ',' << proportion_minor_relapse_from_inactive_disease_fixed << ',' << proportion_minor_relapse_from_inactive_disease_bcell << ',' <<'\n';
		outputFile << "Major Relapse from Inactive, " << proportion_major_relapse_from_inactive_disease_tailored << ',' << proportion_major_relapse_from_inactive_disease_fixed << ',' << proportion_major_relapse_from_inactive_disease_bcell << ',' <<'\n';
		outputFile << "Severe Infection, " << proportion_severe_infection_tailored << ',' << proportion_severe_infection_fixed << ',' <<proportion_severe_infection_bcell << ',' <<'\n';
		outputFile << "ESRD, " << proportion_esrd_tailored << ',' << proportion_esrd_fixed << ',' << proportion_esrd_bcell << ',' << '\n';
		outputFile << "COVID Infection, " << proportion_covid_tailored << ',' << proportion_covid_fixed << ',' << proportion_covid_bcell << ',' << '\n';
		outputFile << "COVID Hospitalization, " << proportion_covid_hosp_tailored << ',' << proportion_covid_hosp_fixed << ',' << proportion_covid_hosp_bcell << ',' << '\n';
		outputFile << "Diabetes, " << proportion_diabetes_tailored << ',' << proportion_diabetes_fixed << ',' << proportion_diabetes_bcell << ',' << '\n';

		outputFile << "Relapse-Free Survival, " << proportion_relapse_free_tailored << ',' << proportion_relapse_free_fixed << ',' << proportion_relapse_free_bcell << ',' << '\n';
		outputFile << "Proportion Survived, " << proportion_survived_tailored << ',' << proportion_survived_fixed << ',' << proportion_survived_bcell << ',' << '\n';
		outputFile << "Mean ANCA Rises, " << mean_anca_rise_tailored << ',' << mean_anca_rise_fixed << ',' << mean_anca_rise_bcell << ',' << '\n';
		outputFile << "Mean B Cell Rises, " << mean_bcell_rise_tailored << ',' << mean_bcell_rise_fixed << ',' << mean_bcell_rise_bcell << ',' << '\n';
		outputFile << "Mean Missed RTX, " << mean_missed_rtx_tailored << ',' << mean_missed_rtx_fixed << ',' << mean_missed_rtx_bcell <<',' << '\n';
		outputFile << "Mean RTX Infusions, " << num_rituximab_tailored / numPatients << ',' << num_rituximab_fixed / numPatients << ',' << num_rituximab_bcell / numPatients << ',' << '\n';
		
		outputFile << "\nNumber of months Active (not Inactive),"<< (float)months_active_disease_tailored/numPatients << ',' << (float)months_active_disease_fixed/numPatients << ',' << (float)months_active_disease_bcell/numPatients << ',' << '\n';
		outputFile << "Number of months Inactive," << (float)months_inactive_disease_tailored/numPatients << ',' << (float)months_inactive_disease_fixed/numPatients << ',' << (float)months_inactive_disease_bcell/numPatients << ',' << '\n';
		
		outputFile << "\n Causes of Death: \n";
		outputFile << " " << "," << "Tailored, " << "Fixed," << "B cell, \n";
		outputFile << "Remission, " << (float)remission_death_tailored / numPatients << ',' << (float)remission_death_fixed / numPatients << ',' << (float)remission_death_bcell / numPatients << '\n';
		outputFile << "Active, " << (float)active_death_tailored / numPatients << ',' << (float)active_death_fixed / numPatients << ',' << (float)active_death_bcell / numPatients << '\n';
		outputFile << "Severe Infection from Remission, " << (float)infection_from_remission_death_tailored / numPatients << ',' << (float)infection_from_remission_death_fixed / numPatients << ',' << (float)infection_from_remission_death_bcell / numPatients << '\n';
		outputFile << "Severe Infection from Active, " << (float)infection_from_active_death_tailored / numPatients << ',' << (float)infection_from_active_death_fixed / numPatients << ',' << (float)infection_from_active_death_bcell / numPatients << '\n';
		outputFile << "Covid 19, " << (float)covid_death_tailored / numPatients << ',' << (float)covid_death_fixed / numPatients << ',' << (float)covid_death_bcell / numPatients << '\n';
		outputFile << "Had ESRD, " << (float)esrd_death_tailored / numPatients << ',' << (float)esrd_death_fixed / numPatients << ',' << (float)esrd_death_bcell / numPatients << '\n';
		outputFile << "Had Diabetes, " << (float)diabetes_death_tailored / numPatients << ',' << (float)diabetes_death_fixed / numPatients << ',' << (float)diabetes_death_bcell / numPatients << '\n';

		outputFile << '\n' << "Proportion of Patients in B Cell Tailored Arm:" << '\n';
		outputFile << "Month, " << "Relapse Free, " << "B Cell Rise," << "Vaccination Opportunity," << "Renal Involvement," << "Death," << "Major Relapse (>=1), " << 
			"Minor Relapse (>=1), " << "Severe Infection, " << "COVID, " << "COVID Hospitalization," << "COVID Death," << "ESRD," << "Vaccinated," << "Vaccination Up-To-Date," << '\n';
		for (int i = 0; i <= endMonth; i++) {
			outputFile << i << ',' << (float)relapseFreeBCell[i] / numPatients << ',' << (float)bCellRepop_BCell[i] / numPatients << ',' << (float)vaccineEligible_BCell[i] / numPatients << ',' << 
				(float)renalInvBCellMonthly[i] / numPatients << ',' << (float)_deathBCell[i] / numPatients << ',' << (float)_majorRelapseBCell[i]/numPatients << ',' << 
				(float)_minorRelapseBCell[i] / numPatients << ',' << (float)_severeBCell[i] / numPatients << ',' << (float)_covidBCell[i] / numPatients << ',' << (float)_covidHospBCell[i] / numPatients << 
				',' << (float)_covidDeathBCell[i] / numPatients << ',' << (float)_esrdBCell[i] / numPatients << ',' << (float)numberVaccinatedBCell[i] / numPatients << ',' << (float)_vaccineUpToDateBCell[i] / numPatients << ',' << '\n';
		}

		outputFile << '\n' << "Proportion of Patients in Tailored Arm:" << '\n';
		outputFile << "Month, " << "Relapse Free, " << "Renal Involvement," << "Death," << "MAJOR Relapse (>=1), " << "MINOR Relapse (>=1)," << "Severe Infection," << "COVID," <<
			"COVID Hospitalization," << "COVID Death," << "ESRD," << "Vaccinated," << "Diabetes," << '\n';
		for (int i = 0; i <= endMonth; i++) {
			outputFile << i << ',' << (float)relapseFreeTailored[i] / numPatients << ',' << (float)renalInvTailoredMonthly[i] / numPatients << ',' << (float)_deathTailored[i] / numPatients << ',' <<
				(float)_majorRelapseTailored[i] / numPatients << ',' << (float)_minorRelapseTailored[i] / numPatients << ',' << (float)_severeTailored[i] / numPatients << ',' << 
				(float)_covidTailored[i] / numPatients << ',' << (float)_covidHospTailored[i] / numPatients << ',' <<(float) _covidDeathTailored[i] / numPatients << ',' << 
				(float)_esrdTailored[i] / numPatients << ',' << (float)numberVaccinatedTailored[i] / numPatients << ',' << (float)_diabetesTailored[i] / numPatients <<',' <<'\n';
		}

		outputFile << '\n' << "Proportion of Patients in Fixed Arm:" << '\n';
		outputFile << "Month, " << "Relapse Free, " << "Renal Involvement," << "Death," << "MAJOR Relapse (>=1), " << "MINOR Relapse (>=1)," << "Severe Infection," << "COVID," <<
			"COVID Hospitalization," << "COVID Death," << "ESRD," << "Vaccinated," << "Diabetes," << '\n';
		for (int i = 0; i <= endMonth; i++) {
			outputFile << i << ',' << (float)relapseFreeFixed[i] / numPatients << ',' << (float)renalInvFixedMonthly[i] / numPatients << ',' << (float)_deathFixed[i] / numPatients << ',' <<
				(float)_majorRelapseFixed[i] / numPatients << ',' << (float)_minorRelapseFixed[i] / numPatients << ',' << (float)_severeFixed[i] / numPatients << ',' << 
				(float)_covidFixed[i] / numPatients << ',' << (float)_covidHospFixed[i] / numPatients << ',' <<(float) _covidDeathFixed[i] / numPatients << ',' << 
				(float)_esrdFixed[i] / numPatients << ',' << (float)numberVaccinatedFixed[i] / numPatients << ',' << (float)_diabetesFixed[i] / numPatients <<',' <<'\n';
		}
		outputFile.close();
		AavUtil::changeDirectoryToInputs();

		/* deallocate simContext memory space */
		delete (simContext);
		simContext = NULL;
		
	}
	std::cout << "Successfully finished simulation. Please press ENTER to exit." << endl;
	std::cin.get();
	// end loop over flies
}



// end main function