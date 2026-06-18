#pragma once

using namespace std;
#define _CRT_SECURE_NO_WARNINGS


#include <iostream>
#include <string.h>
#include <string>
#include <vector>
#include <array>
#include <list>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <assert.h>
#include <time.h>
#include <map>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>

#include <stdio.h>
#include <float.h>
using namespace std;

/** Predefine classes that have circular dependencies */
class SimContext;
class SummaryStats;
class Patient;
class InitializePatient;

#include "SimContext.h"
#include "Tracer.h"
#include "StateUpdater.h"
#include "InitializePatient.h"
#include "Patient.h"
#include "DecisionTree.h"
#include "mtrand.h"
#include "AavUtil.h"



/** Include platform specific header files */
#if defined(_LINUX)
	#include <sys/io.h>
#endif
#if defined(_WIN32)
	#include <io.h>
	#include <direct.h>	//for _mkdir and _CHDIR
#else
	#include <sys/types.h>
	#include <sys/stat.h>
	//used to find files in a directory
	#include <glob.h>
#endif

enum states {REMISSION, ACTIVE, SEVERE, DEATH, POST_RX_REMISSION, POST_MINOR_RELAPSE_REMISSION, COVID, POST_MINOR_RELAPSE_ACTIVE_REMISSION};
enum event {NOTHING, ANCA_RISE, BCELL_RISE};
enum activeNewOrRelapse {NEW_ACTIVE, RELAPSE_ACTIVE};
enum covidHospEnums {Unvaccinated, VaccinatedOnly, VaccinatedEvusheld, VaccinatedPaxlovid};
enum esrdStatus {NO_ESRD, WITH_ESRD};
enum diabetesStatus {NO_DIABETES, WITH_DIABETES};
enum renalInvStatus {NO_RENAL_INV, WITH_RENAL_INV};
enum ancaType {MPO, PR3};
enum relapseTreatmentStrat {TAILORED, FIXED, MISSED_RTX, BCELL_UNDET, BCELL_REPOP, ACTIVE_WITH_REMISSION};
enum treatmentArm {TAILORED_ARM, FIXED_ARM, BCELL_ARM};
enum gender {MALE, FEMALE};

enum steroid_regimens {REDUCED, STANDARD, OBSERVATIONAL, NO_STEROID };
enum covidPrEP {PrEP_A, PrEP_B, PrEP_C, NO_PrEP};
enum covidTreatment {Treatment_A, Treatment_B, Treatment_C, No_Treatment};
enum covid_states {Non_Immune, Past_Infection, Vaccinated, Vaccinated_Past_infection, Vaccine_Up_To_Date, PrEP_Option_A, 
                    PrEP_Option_B, PrEP_Option_C, Treatment_Option_A, Treatment_Option_B, Treatment_Option_C};
enum initCovidStats {NO_PAST_INFECTION_OR_VACCINATION, PAST_INFECTION_NO_VACCINE, VACCINATED_NO_PAST_INFECTION, VACCINATED_AND_PAST_INFECTION,
					VACCINE_UP_TO_DATE, VACCINE_UP_TO_DATE_PAST_INFECTION};