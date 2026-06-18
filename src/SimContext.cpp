#include "include.h"
#include <typeinfo>

/* Constructor takes run name as parameter */
SimContext::SimContext(string runName) {
    counter = 0;
    inputFileName = runName;
    inputFileName.append(".in");
}

/* Destructor cleans up the allocated memory*/
SimContext::~SimContext(void) {
    // clean up allocated memory here
}

/* Initialize the number of patients to trace to 100 */
int SimContext::numPatientsToTrace = 100;

void SimContext::expandFloatVect(vector<vector<float>> vect, float arr[][101], vector<int> buckets) {
    int i, j, k;
    int num_rows = vect.size();
    int num_cols = vect[0].size();
    for (i = 0; i < num_rows; i++) {
        for (j = buckets[i]; j < buckets[i + 1]; j++) {
            for (k = 0; k < num_cols; k++) {
                arr[k][j] = vect[i][k];
            }
        }
    }
}

/* readInputs function reads in all the inputs from the given input file,
	throws exception if there is an error */
void SimContext::readInputs() {
    /* Open the input file for reading */
    AavUtil::changeDirectoryToInputs();
    inputFile = AavUtil::openFile(inputFileName.c_str(), "r");
    if (inputFile == NULL) {
        string errorString = "  ERROR - Could not open input file ";
        errorString.append(inputFileName);
        throw errorString;
    }
    
    
    /* Read all te input data from the file */
    readCohortParamsInputs();
    readRelapseInfectionInputs();
    readCovidInputs();
    readESRDInputs();
    readESRDMortalityInputs();
    readInfectionMortalityInputs();
    readRemissionRelapseMortalityInputs();
    readDiabetesInputs();

    

    /* Close the input file */
    AavUtil::closeFile(inputFile);
} /* end readInputs */

/* readCohortParamsInputs reads data from the Cohort Parameters tab of the input sheet */
void SimContext::readCohortParamsInputs() {
    char buffer[256];
    bool tempBool;
    // read in cohort size
    readAndSkipPast("CohortSize", inputFile);
    fscanf(inputFile, "%d", &cohortParamsInputs.numPatients);
    // read in mean age
    readAndSkipPast("AgeMean", inputFile);
    fscanf(inputFile, "%f", &cohortParamsInputs.ageMean);
    
    // read in age std dev
    readAndSkipPast("AgeStdDev", inputFile);
    fscanf(inputFile, "%f", &cohortParamsInputs.ageStdDev);
    // read in probability male
    readAndSkipPast("ProbMale", inputFile);
    fscanf(inputFile, "%f", &cohortParamsInputs.probMale);
    // read in probability renal involvement
    readAndSkipPast("ProbRenalInv", inputFile);
    fscanf(inputFile, "%f", &cohortParamsInputs.probRenalInv);
    // read in probabiliity pr3 anca type
    readAndSkipPast("ProbPR3", inputFile);
    fscanf(inputFile, "%f", &cohortParamsInputs.probPR3);
    // read in distribution type
    readAndSkipPast("DistType", inputFile);
    fscanf(inputFile, "%d", &cohortParamsInputs.distType);
    /* Reading in initial cohort distributions */
    // read in proportion of patients initially in Remission State
    readAndSkipPast("InitRemission", inputFile);
    fscanf(inputFile, "%f", &cohortParamsInputs.initRemission);
    // read in proportion of patients initially in Active Newly Diagnosed State
    readAndSkipPast("InitActiveNewlyDiagnosed", inputFile);
    fscanf(inputFile, "%f", &cohortParamsInputs.initActive[NEW_ACTIVE]);
    // read in proportion of patients initially in Active Relapsing state
    readAndSkipPast("InitActiveRelapsing", inputFile);
    fscanf(inputFile, "%f", &cohortParamsInputs.initActive[RELAPSE_ACTIVE]);
    // read in proportion of patients initially in Severe Infection State
    readAndSkipPast("InitSevere", inputFile);
    fscanf(inputFile, "%f", &cohortParamsInputs.initSevere);
    // read in proportion of patients initially in Covid State
    readAndSkipPast("InitCovid", inputFile);
    fscanf(inputFile, "%f", &cohortParamsInputs.initCovid);
    // read in proportion of patients initially in ESRD State
    readAndSkipPast("InitESRD", inputFile);
    fscanf(inputFile, "%f", &cohortParamsInputs.initESRD);
    // read in proportion of patients initially in Remission State
    readAndSkipPast("InitDiabetes", inputFile);
    fscanf(inputFile, "%f", &cohortParamsInputs.initDiabetes);
    // read in the time from active to inactive state 
    /** Note: length of time from active to inactive is in days */
    readAndSkipPast("TimeFromActiveToInactive", inputFile);
    float floatBuffer;
    for (int i = 0; i < 6; i++) {
        fscanf(inputFile, "%f", &floatBuffer);
        cohortParamsInputs.lengthActiveToInactiveProbabilities.push_back(floatBuffer);
    }

    // read in month that simulation ends
    readAndSkipPast("EndMonth", inputFile);
    fscanf(inputFile, "%d", &cohortParamsInputs.endMonth);
    // read in monthly probability of ANCA Rise
    readAndSkipPast("MonthlyAncaRiseProb", inputFile);
    fscanf(inputFile, "%f", &cohortParamsInputs.probANCARise);
    // read in monthly probability of B Cell Rise
    readAndSkipPast("MonthlyBCellRiseProb", inputFile);
    fscanf(inputFile, "%f", &cohortParamsInputs.probBCellRise);
    // read in monthly probability of developing Renal Involvement during Active Disease
    readAndSkipPast("MonthlyRenalInvFromActiveProb", inputFile);
    fscanf(inputFile, "%f", &cohortParamsInputs.probRenalFromActive);
    // month that rituximab treatment ceases
    readAndSkipPast("MonthStopRTX", inputFile);
    fscanf(inputFile, "%d", &cohortParamsInputs.monthStopRTX);

    /* Cohort Vaccination Toggles */
    // read in whether patient is vaccinated at b cell rise
    readAndSkipPast("VaccAtBCellRise", inputFile);
    fscanf(inputFile, "%d", &tempBool);
    cohortParamsInputs.vaccAtBCellRise = (bool) tempBool;
    // read in vaccine effectiveness delay
    readAndSkipPast("VaccEffDelay", inputFile);
    fscanf(inputFile, "%d", &cohortParamsInputs.vaccEffDelay);
    
    // read in strategy after vaccine
    readAndSkipPast("StratAfterVacc", inputFile);
    
    fscanf(inputFile, "%d", &cohortParamsInputs.stratAfterVacc);
    
    
    // read in strategy change delay months
    readAndSkipPast("StratChangeFromVaccDelayMonths", inputFile);
    fscanf(inputFile, "%d", &cohortParamsInputs.stratDelayMonthsVacc);
    
    /* Cohort Relapse Toggles */
    // read in whether strategy changes after relapse
    readAndSkipPast("StratChangeAfterRelapse", inputFile);
    fscanf(inputFile, "%d", &tempBool);
    cohortParamsInputs.changeStratAfterRelapse = tempBool;
    // read in whether to change strategy after minor relapse as well
    readAndSkipPast("IncludeMinorRelapse", inputFile);
    fscanf(inputFile, "%d", &tempBool);
    cohortParamsInputs.includeMinorRelapse = tempBool;
    // read in strategy to switch to after relapse
    readAndSkipPast("StratAfterRelapse", inputFile);
    fscanf(inputFile, "%d", &cohortParamsInputs.stratAfterRelapse);
    // read in strategy change delay months
    readAndSkipPast("StratChangeFromRelapseDelayMonths", inputFile);
    fscanf(inputFile, "%d", &cohortParamsInputs.stratDelayMonthsRelapse);
    // read in random seed by time
    readAndSkipPast("RandSeedByTime", inputFile);
    fscanf(inputFile, "%d", &tempBool);
    cohortParamsInputs.randSeedByTime = tempBool;
} /* end readCohortParamsInputs */

/* readRelapseInfectionInputs reads data from the Relapse and Infection Risk tab of the input sheet */
void SimContext::readRelapseInfectionInputs() {
    int i;
    // read in major relapse probabilities
    readAndSkipPast("MajorRelapseNoESRD", inputFile);
    for (i = 0; i < NUM_RELAPSE_FACTORS; i++) {
        readAndSkipPast(RELAPSE_FACTORS_STRS[i], inputFile);
        fscanf(inputFile, "%f", &(relapseInfectionInputs.majRelapse[i][MPO][NO_ESRD]));
        fscanf(inputFile, "%f", &(relapseInfectionInputs.majRelapse[i][PR3][NO_ESRD]));
    }

    readAndSkipPast("MajorRelapseWithESRD", inputFile);
    for (i = 0; i < NUM_RELAPSE_FACTORS; i++) {
        readAndSkipPast(RELAPSE_FACTORS_STRS[i], inputFile);
        fscanf(inputFile, "%f", &(relapseInfectionInputs.majRelapse[i][MPO][WITH_ESRD]));
        fscanf(inputFile, "%f", &(relapseInfectionInputs.majRelapse[i][PR3][WITH_ESRD]));
    }

    // read in minor relapse probabilities
    readAndSkipPast("MinorRelapseNoESRD", inputFile);
    for (i = 0; i < NUM_RELAPSE_FACTORS; i++) {
        readAndSkipPast(RELAPSE_FACTORS_STRS[i], inputFile);
        fscanf(inputFile, "%f", &(relapseInfectionInputs.minRelapse[i][MPO][NO_ESRD]));
        fscanf(inputFile, "%f", &(relapseInfectionInputs.minRelapse[i][PR3][NO_ESRD]));
    }
    readAndSkipPast("MinorRelapseWithESRD", inputFile);
    for (i = 0; i < NUM_RELAPSE_FACTORS; i++) {
        readAndSkipPast(RELAPSE_FACTORS_STRS[i], inputFile);
        fscanf(inputFile, "%f", &(relapseInfectionInputs.minRelapse[i][MPO][NO_ESRD]));
        fscanf(inputFile, "%f", &(relapseInfectionInputs.minRelapse[i][PR3][NO_ESRD]));
    }

    // read in severe infection probabilities
    readAndSkipPast("SevereInfection", inputFile);
    for (i = 0; i < NUM_TREATMENT_ARMS; i++ ) {
        readAndSkipPast(TREATMENT_ARMS[i], inputFile);
        fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionRemission[i][NO_DIABETES][NO_ESRD]));
        fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionRemission[i][WITH_DIABETES][NO_ESRD]));
        fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionRemission[i][NO_DIABETES][WITH_ESRD]));
        fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionRemission[i][WITH_DIABETES][WITH_ESRD]));
    }

    

    // read in severe infection from active (new) state probabilities
    readAndSkipPast("ActiveNewlyDiagnosedToSevereReducedSteroid", inputFile);
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[REDUCED][NO_DIABETES][NO_ESRD][NEW_ACTIVE]));
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[REDUCED][WITH_DIABETES][NO_ESRD][NEW_ACTIVE]));
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[REDUCED][NO_DIABETES][WITH_ESRD][NEW_ACTIVE]));
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[REDUCED][WITH_DIABETES][WITH_ESRD][NEW_ACTIVE]));
    readAndSkipPast("ActiveNewlyDiagnosedToSevereStandardSteroid", inputFile);
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[STANDARD][NO_DIABETES][NO_ESRD][NEW_ACTIVE]));
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[STANDARD][WITH_DIABETES][NO_ESRD][NEW_ACTIVE]));
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[STANDARD][NO_DIABETES][WITH_ESRD][NEW_ACTIVE]));
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[STANDARD][WITH_DIABETES][WITH_ESRD][NEW_ACTIVE]));
    readAndSkipPast("ActiveNewlyDiagnosedToSevereObservationalSteroid", inputFile);
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[OBSERVATIONAL][NO_DIABETES][NO_ESRD][NEW_ACTIVE]));
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[OBSERVATIONAL][WITH_DIABETES][NO_ESRD][NEW_ACTIVE]));
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[OBSERVATIONAL][NO_DIABETES][WITH_ESRD][NEW_ACTIVE]));
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[OBSERVATIONAL][WITH_DIABETES][WITH_ESRD][NEW_ACTIVE]));
    readAndSkipPast("ActiveNewlyDiagnosedToSevereNoSteroid", inputFile);
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[NO_STEROID][NO_DIABETES][NO_ESRD][NEW_ACTIVE]));
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[NO_STEROID][WITH_DIABETES][NO_ESRD][NEW_ACTIVE]));
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[NO_STEROID][NO_DIABETES][WITH_ESRD][NEW_ACTIVE]));
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[NO_STEROID][WITH_DIABETES][WITH_ESRD][NEW_ACTIVE]));

    // read in severe infection from active (relapsing) state probabilities
    readAndSkipPast("ActiveRelapsingToSevereReducedSteroid", inputFile);
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[REDUCED][NO_DIABETES][NO_ESRD][RELAPSE_ACTIVE]));
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[REDUCED][WITH_DIABETES][NO_ESRD][RELAPSE_ACTIVE]));
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[REDUCED][NO_DIABETES][WITH_ESRD][RELAPSE_ACTIVE]));
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[REDUCED][WITH_DIABETES][WITH_ESRD][RELAPSE_ACTIVE]));
    readAndSkipPast("ActiveRelapsingToSevereStandardSteroid", inputFile);
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[STANDARD][NO_DIABETES][NO_ESRD][RELAPSE_ACTIVE]));
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[STANDARD][WITH_DIABETES][NO_ESRD][RELAPSE_ACTIVE]));
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[STANDARD][NO_DIABETES][WITH_ESRD][RELAPSE_ACTIVE]));
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[STANDARD][WITH_DIABETES][WITH_ESRD][RELAPSE_ACTIVE]));
    readAndSkipPast("ActiveRelapsingToSevereObservationalSteroid", inputFile);
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[OBSERVATIONAL][NO_DIABETES][NO_ESRD][RELAPSE_ACTIVE]));
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[OBSERVATIONAL][WITH_DIABETES][NO_ESRD][RELAPSE_ACTIVE]));
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[OBSERVATIONAL][NO_DIABETES][WITH_ESRD][RELAPSE_ACTIVE]));
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[OBSERVATIONAL][WITH_DIABETES][WITH_ESRD][RELAPSE_ACTIVE]));
    readAndSkipPast("ActiveRelapsingToSevereNoSteroid", inputFile);
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[NO_STEROID][NO_DIABETES][NO_ESRD][RELAPSE_ACTIVE]));
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[NO_STEROID][WITH_DIABETES][NO_ESRD][RELAPSE_ACTIVE]));
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[NO_STEROID][NO_DIABETES][WITH_ESRD][RELAPSE_ACTIVE]));
    fscanf(inputFile, "%f", &(relapseInfectionInputs.infectionActive[NO_STEROID][WITH_DIABETES][WITH_ESRD][RELAPSE_ACTIVE]));
};

/* readCovidInputs reads data from the COVID tab of the input sheet */
void SimContext::readCovidInputs() {
    int i, j, k, tempInt, tempBool;
    char scratch[256];
    vector<int> covidAgeBuckets = { 0, 18, 30, 50, 65, 80, 101 };
    /* Read in initial COVID Stats */
    readAndSkipPast("PastInfectionNoVaccine", inputFile);
    fscanf(inputFile, "%f", &covidInputs.pastCovidInfection);
    readAndSkipPast("VaccinatedNoPastInfection", inputFile);
    fscanf(inputFile, "%f", &covidInputs.probCovidVaccine);
    readAndSkipPast("VaccinatedPastInfection", inputFile);
    fscanf(inputFile, "%f", &covidInputs.probPastInfectionAndVaccine);
    readAndSkipPast("VaccinatedUpToDate", inputFile);
    fscanf(inputFile, "%f", &covidInputs.probVaccineUpToDate);
    readAndSkipPast("VaccinatedUpToDatePastInfection", inputFile);
    fscanf(inputFile, "%f", &covidInputs.probVaccineUpToDateAndPastInfection);

    /* Read in Pre-Exposure Prophylaxis proportions */
    readAndSkipPast("ProbPrepA", inputFile);
    fscanf(inputFile, "%f", &covidInputs.covidPrEPA);
    readAndSkipPast("ProbPrepB", inputFile);
    fscanf(inputFile, "%f", &covidInputs.covidPrEPB);
    readAndSkipPast("ProbPrepC", inputFile);
    fscanf(inputFile, "%f", &covidInputs.covidPrEPC);

    /* Read in Treatment proportions */
    readAndSkipPast("TreatmentA", inputFile);
    fscanf(inputFile, "%f", &covidInputs.covidTreatmentA);
    readAndSkipPast("TreatmentB", inputFile);
    fscanf(inputFile, "%f", &covidInputs.covidTreatmentB);
    readAndSkipPast("TreatmentC", inputFile);
    fscanf(inputFile, "%f", &covidInputs.covidTreatmentC);

    /* Read in Covid Surge Parameters */
    readAndSkipPast("CovidSurgeOnOff", inputFile);
    fscanf(inputFile, "%d", &tempBool);
    covidInputs.covidSurge = tempBool;
    readAndSkipPast("CovidSurgeStartMonth", inputFile);
    fscanf(inputFile, "%d", &covidInputs.covidSurgeMonthStart);
    readAndSkipPast("CovidSurgeEndMonth", inputFile);
    fscanf(inputFile, "%d", &covidInputs.covidSurgeMonthEnd);
    readAndSkipPast("CovidSurgeMultiplier", inputFile);
    fscanf(inputFile, "%f", &covidInputs.covidSurgeMultiplier);

    /* Read in num months vaccine up to date parameters */
    readAndSkipPast("MonthsVaccUpToDateMean", inputFile);
    fscanf(inputFile, "%d", &covidInputs.vaccineUpToDateMonthsMean);
    readAndSkipPast("MonthsVaccUpToDateStdDev", inputFile);
    fscanf(inputFile, "%d", &covidInputs.vaccineUpToDateMonthsStdDev);

    /* Read in monthly covid infection probabilities*/
    vector<float> monthlyCovidInfectionProbsForBucket(NUM_COVID_INFECT_FACTORS, 0);
    vector<vector<float>> covidMonthlyInfectionVect(NUM_COVID_AGE_BUCKETS, { monthlyCovidInfectionProbsForBucket });
    readAndSkipPast("MonthlyCovidInfectionProbs", inputFile);
    for (i = 0; i < NUM_COVID_AGE_BUCKETS; i++) {
        readAndSkipPast(COVID_AGE_BUCKETS[i], inputFile);
        std::fill(monthlyCovidInfectionProbsForBucket.begin(), monthlyCovidInfectionProbsForBucket.end(), 0);
        for (int a = 0; a < NUM_COVID_INFECT_FACTORS; a++) {
            fscanf(inputFile, "%f", &(monthlyCovidInfectionProbsForBucket[a]));
        }
        covidMonthlyInfectionVect[i] = monthlyCovidInfectionProbsForBucket;
    }
    
    for (i = 0; i < NUM_COVID_AGE_BUCKETS; i++) {
        for (j = covidAgeBuckets[i]; j < covidAgeBuckets[i + 1]; j++) {
            for (k = 0; k < NUM_COVID_INFECT_FACTORS; k++) {
                covidInputs.covidInfection[k][j] = covidMonthlyInfectionVect[i][k];
            }
        }
    }

    /* Read in monthly covid hospitalizaiton probabilities  */
    vector<float> monthlyCovidHospProbsForBucket(NUM_COVID_HOSP_MORTALITY_FACTORS, 0);
    vector<vector<float>> covidMonthlyHospVect(NUM_COVID_AGE_BUCKETS, { monthlyCovidHospProbsForBucket });
    readAndSkipPast("MonthlyCovidHospitalizationProbs", inputFile);
    for (i = 0; i < NUM_COVID_AGE_BUCKETS; i++) {
        readAndSkipPast(COVID_AGE_BUCKETS[i], inputFile);
        std::fill(monthlyCovidHospProbsForBucket.begin(), monthlyCovidHospProbsForBucket.end(), 0);
        for (int a = 0; a < NUM_COVID_HOSP_MORTALITY_FACTORS; a++) {
            fscanf(inputFile, "%f", &(monthlyCovidHospProbsForBucket[a]));
        }
        covidMonthlyHospVect[i] = monthlyCovidHospProbsForBucket;
    }

    /* Read in monthly covid mortlaity probabilities */
    vector<float> monthlyCovidMortalityProbsForBucket(NUM_COVID_HOSP_MORTALITY_FACTORS, 0);
    vector<vector<float>> covidMonthlyMortalityVect(NUM_COVID_AGE_BUCKETS, { monthlyCovidMortalityProbsForBucket });
    readAndSkipPast("MonthlyCovidMortalityProbs", inputFile);
    for (i = 0; i < NUM_COVID_AGE_BUCKETS; i++) {
        readAndSkipPast(COVID_AGE_BUCKETS[i], inputFile);
        std::fill(monthlyCovidMortalityProbsForBucket.begin(), monthlyCovidMortalityProbsForBucket.end(), 0);
        for (j = 0; j < NUM_COVID_HOSP_MORTALITY_FACTORS; j++) {
            fscanf(inputFile, "%f", &(monthlyCovidMortalityProbsForBucket[j]));
        }
        covidMonthlyMortalityVect[i] = monthlyCovidMortalityProbsForBucket;
    }


    for (i = 0; i < NUM_COVID_AGE_BUCKETS; i++) {
        for (j = covidAgeBuckets[i]; j < covidAgeBuckets[i + 1]; j++) {
            for (k = 0; k < NUM_COVID_HOSP_MORTALITY_FACTORS; k++) {
                covidInputs.covidHosp[k][j] = covidMonthlyHospVect[i][k];
                covidInputs.covidMortality[k][j] = covidMonthlyMortalityVect[i][k];
            }
        }
    }

};

/* readESRDInputs reads data from the ESRD tab of the input sheet */
void SimContext::readESRDInputs() {
    int i, j, k;
    vector<int> esrdAgeBuckets = { 0, 22, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 101 };
    vector<float> monthlyProbsForBucket(4, 0);
    vector<vector<float>> ESRDRemissionNoDiabetes(NUM_ESRD_AGE_BUCKETS, monthlyProbsForBucket);
    vector<vector<float>> ESRDRemissionWithDiabetes(NUM_ESRD_AGE_BUCKETS, monthlyProbsForBucket);
    vector<vector<float>> ESRDActiveNoDiabetes(NUM_ESRD_AGE_BUCKETS, monthlyProbsForBucket);
    vector<vector<float>> ESRDActiveWithDiabetes(NUM_ESRD_AGE_BUCKETS, monthlyProbsForBucket);

    vector<vector<float>> esrdMonthlyProb(NUM_ESRD_AGE_BUCKETS, monthlyProbsForBucket);

    vector<vector<float>> esrdMonthlyProbDiabetes = esrdMonthlyProb;
    readAndSkipPast("ESRDFromRemission", inputFile);
    for (i = 0; i < NUM_ESRD_AGE_BUCKETS; i++){
        fill(monthlyProbsForBucket.begin(), monthlyProbsForBucket.end(), 0);
        readAndSkipPast(ESRD_AGE_BUCKETS[i], inputFile);
        for (j = 0; j < 4; j++) {
            fscanf(inputFile, "%f", &(monthlyProbsForBucket[j]));
        }
        ESRDRemissionNoDiabetes[i] = monthlyProbsForBucket;
    }

    readAndSkipPast("ESRDFromRemissionDiabetes", inputFile);
    for (i = 0; i < NUM_ESRD_AGE_BUCKETS; i++){
        fill(monthlyProbsForBucket.begin(), monthlyProbsForBucket.end(), 0);
        readAndSkipPast(ESRD_AGE_BUCKETS[i], inputFile);
        for (j = 0; j < 4; j++) {
            fscanf(inputFile, "%f", &(monthlyProbsForBucket[j]));
        }
        ESRDRemissionWithDiabetes[i] = monthlyProbsForBucket;
    }

    readAndSkipPast("ESRDFromActive", inputFile);
    for (i = 0; i < NUM_ESRD_AGE_BUCKETS; i++) {
        fill(monthlyProbsForBucket.begin(), monthlyProbsForBucket.end(), 0);
        readAndSkipPast(ESRD_AGE_BUCKETS[i], inputFile);
        for (j = 0; j < 4; j++) {
            fscanf(inputFile, "%f", &(monthlyProbsForBucket[j]));
        }
        ESRDActiveNoDiabetes[i] = monthlyProbsForBucket;
    }

    readAndSkipPast("ESRDFromActiveDiabetes", inputFile);
    for (i = 0; i < NUM_ESRD_AGE_BUCKETS; i++) {
        fill(monthlyProbsForBucket.begin(), monthlyProbsForBucket.end(), 0);
        readAndSkipPast(ESRD_AGE_BUCKETS[i], inputFile);
        for (j = 0; j < 4; j++) {
            fscanf(inputFile, "%f", &(monthlyProbsForBucket[j]));
        }
        ESRDActiveWithDiabetes[i] = monthlyProbsForBucket;
    }

    /** expand the vectors into the arrays based on the age buckets */
    for (i = 0; i < NUM_ESRD_AGE_BUCKETS; i++) {
        for (j = esrdAgeBuckets[i]; j < esrdAgeBuckets[i+1]; j++) {
            esrdInputs.ESRDFromRemission[j][MALE][NO_RENAL_INV][NO_DIABETES] = ESRDRemissionNoDiabetes[i][0];
            esrdInputs.ESRDFromRemission[j][MALE][WITH_RENAL_INV][NO_DIABETES] = ESRDRemissionNoDiabetes[i][1];
            esrdInputs.ESRDFromRemission[j][FEMALE][NO_RENAL_INV][NO_DIABETES] = ESRDRemissionNoDiabetes[i][2];
            esrdInputs.ESRDFromRemission[j][FEMALE][WITH_RENAL_INV][NO_DIABETES] = ESRDRemissionNoDiabetes[i][3];
            esrdInputs.ESRDFromRemission[j][MALE][NO_RENAL_INV][WITH_DIABETES] = ESRDRemissionWithDiabetes[i][0];
            esrdInputs.ESRDFromRemission[j][MALE][WITH_RENAL_INV][WITH_DIABETES] = ESRDRemissionWithDiabetes[i][1];
            esrdInputs.ESRDFromRemission[j][FEMALE][NO_RENAL_INV][WITH_DIABETES] = ESRDRemissionWithDiabetes[i][2];
            esrdInputs.ESRDFromRemission[j][FEMALE][WITH_RENAL_INV][WITH_DIABETES] = ESRDRemissionWithDiabetes[i][3];
            esrdInputs.ESRDFromActive[j][MALE][NO_RENAL_INV][NO_DIABETES] = ESRDActiveNoDiabetes[i][0];
            esrdInputs.ESRDFromActive[j][MALE][WITH_RENAL_INV][NO_DIABETES] = ESRDActiveNoDiabetes[i][1];
            esrdInputs.ESRDFromActive[j][FEMALE][NO_RENAL_INV][NO_DIABETES] = ESRDActiveNoDiabetes[i][2];
            esrdInputs.ESRDFromActive[j][FEMALE][WITH_RENAL_INV][NO_DIABETES] = ESRDActiveNoDiabetes[i][3];
            esrdInputs.ESRDFromActive[j][MALE][NO_RENAL_INV][WITH_DIABETES] = ESRDActiveWithDiabetes[i][0];
            esrdInputs.ESRDFromActive[j][MALE][WITH_RENAL_INV][WITH_DIABETES] = ESRDActiveWithDiabetes[i][1];
            esrdInputs.ESRDFromActive[j][FEMALE][NO_RENAL_INV][WITH_DIABETES] = ESRDActiveWithDiabetes[i][2];
            esrdInputs.ESRDFromActive[j][FEMALE][WITH_RENAL_INV][WITH_DIABETES] = ESRDActiveWithDiabetes[i][3];
        }
    }
};

/* readESRDMortalityInputs reads data from the ESRD Mortality tab of the input sheet */
void SimContext::readESRDMortalityInputs() {
    int i, j;
    vector<int> esrdMortalityAgeBuckets = { 0, 25, 30, 35, 40, 45, 50, 55, 60, 65, 70, 75, 80, 85, 101 };
    vector<int> esrdMortalityDiabetesAgeBuckets {0, 18, 30, 40, 50, 60, 65, 70, 75, 80, 85, 101};
    vector<float> esrdMortalityProbsForBucket(2, 0);
    vector<vector<float>> esrdMortalityVector(NUM_ESRD_MORTALITY_AGE_BUCKETS, esrdMortalityProbsForBucket);
    vector<vector<float>> esrdMortalityVectorDiabetes = esrdMortalityVector;

    readAndSkipPast("ESRDMortality", inputFile);
    for (i = 0; i < NUM_ESRD_MORTALITY_AGE_BUCKETS; i++) {
        readAndSkipPast(ESRD_MORTALITY_AGE_BUCKETS[i], inputFile);
        fill(esrdMortalityProbsForBucket.begin(), esrdMortalityProbsForBucket.end(), 0); // reset the esrdMortalityProbsForBucket vector to all 0. 
        fscanf(inputFile, "%f", &(esrdMortalityProbsForBucket[0]));
        fscanf(inputFile, "%f", &(esrdMortalityProbsForBucket[1]));
        esrdMortalityVector[i] = esrdMortalityProbsForBucket;
    }

    readAndSkipPast("ESRDMortalityDiabetes", inputFile);
    for (i = 0; i < NUM_ESRD_MORTALITY_DIABETES_AGE_BUCKETS; i++) {
        readAndSkipPast(ESRD_MORTALITY_DIABETES_AGE_BUCKETS[i], inputFile);
        fill(esrdMortalityProbsForBucket.begin(), esrdMortalityProbsForBucket.end(), 0); // reset the esrdMortalityProbsForBucket vector to all 0. 
        fscanf(inputFile, "%f", &(esrdMortalityProbsForBucket[0]));
        fscanf(inputFile, "%f", &(esrdMortalityProbsForBucket[1]));
        esrdMortalityVectorDiabetes[i] = esrdMortalityProbsForBucket;
    }
    // expand vectors to arrays here
    for (i = 0; i < NUM_ESRD_MORTALITY_AGE_BUCKETS; i++) {
        for (j = esrdMortalityAgeBuckets[i]; j < esrdMortalityAgeBuckets[i+1]; j++) {
            esrdMortalityInputs.ESRDMortality[j][MALE][NO_DIABETES] = esrdMortalityVector[i][0];
            esrdMortalityInputs.ESRDMortality[j][FEMALE][NO_DIABETES] = esrdMortalityVector[i][1];
        }
    }
    for (i = 0; i < NUM_ESRD_MORTALITY_DIABETES_AGE_BUCKETS; i++) {
        for (j = esrdMortalityDiabetesAgeBuckets[i]; j < esrdMortalityDiabetesAgeBuckets[i+1]; j++) {
            esrdMortalityInputs.ESRDMortality[j][MALE][WITH_DIABETES] = esrdMortalityVectorDiabetes[i][0];
            esrdMortalityInputs.ESRDMortality[j][FEMALE][WITH_DIABETES] = esrdMortalityVectorDiabetes[i][1];
        }
    }
};

/* readInfectionMortalityInputs reads data from the Infection Mortality tab of the input sheet */
void SimContext::readInfectionMortalityInputs() {
    int i, j;
    vector<int> infectionAgeBuckets = { 0, 1, 18, 45, 65, 85, 101 };

    vector<float> infectionMortalityProbsForBucket(2, 0);
    vector<vector<float>> infectionMortalityVector(NUM_INFECTION_AGE_BUCKETS, infectionMortalityProbsForBucket);

    readAndSkipPast("InfectionMortality", inputFile);
    for (i = 0; i < NUM_INFECTION_AGE_BUCKETS; i++) {
        readAndSkipPast(INFECTION_AGE_BUCKETS_STRS[i], inputFile);
        fill(infectionMortalityProbsForBucket.begin(), infectionMortalityProbsForBucket.end(), 0);
        fscanf(inputFile, "%f", &(infectionMortalityProbsForBucket[NO_DIABETES]));
        fscanf(inputFile, "%f", &(infectionMortalityProbsForBucket[WITH_DIABETES]));
        infectionMortalityVector[i] = infectionMortalityProbsForBucket;
    }
    // expand infectionMortalityVector into infectionMortalityInputs.infectionMortality array
    for (i = 0; i < NUM_INFECTION_AGE_BUCKETS; i++) {
        for (j = infectionAgeBuckets[i]; j < infectionAgeBuckets[i+1]; j++) {
            infectionMortalityInputs.infectionMortality[j][NO_DIABETES] = infectionMortalityVector[i][NO_DIABETES];
            infectionMortalityInputs.infectionMortality[j][WITH_DIABETES] = infectionMortalityVector[i][WITH_DIABETES];
        }
    }
};

/* readRemissionRelapseMortalityInputs reads data from the Mortality tab of the input sheet */
void SimContext::readRemissionRelapseMortalityInputs() {
    int i, j;
    char buffer[256];
    vector<vector<float>> remissionMortalityVector;
    vector<vector<float>> remissionWithDiabetesMortalityVector;
    vector<vector<float>> relapseMortalityVector;
    vector<vector<float>> relapseWithDiabetesMortalityVector;

    readAndSkipPast("RelapseMortality", inputFile);
    for (i = 0; i < AGE_YRS; i++) {
        // set age_str to age as a string
        string age_str = to_string(i);
        const char* age_char = age_str.c_str();
        string* age_str_ptr = &age_str;
        readAndSkipPast(age_char, inputFile);
        fscanf(inputFile, "%f", &(remissionRelapseMortalityInputs.activeMortality[MALE][i][NO_DIABETES]));
        fscanf(inputFile, "%f", &(remissionRelapseMortalityInputs.activeMortality[FEMALE][i][NO_DIABETES]));
    }
    readAndSkipPast("InactiveDiseaseMortality", inputFile);
    for (i = 0; i < AGE_YRS; i++) {
        // set age_str to age as a string
        string age_str = to_string(i);
        const char *age_char = age_str.c_str();
        string* age_str_ptr = &age_str;
        readAndSkipPast(age_char, inputFile);
        fscanf(inputFile, "%f", &(remissionRelapseMortalityInputs.inactiveDiseaseMortality[MALE][i][NO_DIABETES]));
        fscanf(inputFile, "%f", &(remissionRelapseMortalityInputs.inactiveDiseaseMortality[FEMALE][i][NO_DIABETES]));
    }
    readAndSkipPast("RemissionMortality", inputFile);
    for (i = 0; i < AGE_YRS; i++) {
        // set age_str to age as a string
        string age_str = to_string(i);
        const char *age_char = age_str.c_str();
        string* age_str_ptr = &age_str;
        readAndSkipPast(age_char, inputFile);
        fscanf(inputFile, "%f", &(remissionRelapseMortalityInputs.remissionMortality[MALE][i][NO_DIABETES]));
        fscanf(inputFile, "%f", &(remissionRelapseMortalityInputs.remissionMortality[FEMALE][i][NO_DIABETES]));
    }
    readAndSkipPast("RelapseMortalityDiabetes", inputFile);
    for (i = 0; i < AGE_YRS; i++) {
        // set age_str to age as a string
        string age_str = to_string(i);
        const char* age_char = age_str.c_str();
        string* age_str_ptr = &age_str;
        readAndSkipPast(age_char, inputFile);
        fscanf(inputFile, "%f", &(remissionRelapseMortalityInputs.activeMortality[MALE][i][WITH_DIABETES]));
        fscanf(inputFile, "%f", &(remissionRelapseMortalityInputs.activeMortality[FEMALE][i][WITH_DIABETES]));
    }
    readAndSkipPast("InactiveDiseaseMortalityDiabetes", inputFile);
    for (i = 0; i < AGE_YRS; i++) {
        for (i = 0; i < AGE_YRS; i++) {
        // set age_str to age as a string
        string age_str = to_string(i);
        const char *age_char = age_str.c_str();
        string* age_str_ptr = &age_str;
        readAndSkipPast(age_char, inputFile);
        fscanf(inputFile, "%f", &(remissionRelapseMortalityInputs.inactiveDiseaseMortality[MALE][i][WITH_DIABETES]));
        fscanf(inputFile, "%f", &(remissionRelapseMortalityInputs.inactiveDiseaseMortality[FEMALE][i][WITH_DIABETES]));
    }
    }
    readAndSkipPast("RemissionMortalityDiabetes", inputFile);
    for (i = 0; i < AGE_YRS; i++) {
        // set age_str to age as a string
        string age_str = to_string(i);
        const char *age_char = age_str.c_str();
        string* age_str_ptr = &age_str;
        readAndSkipPast(age_char, inputFile);
        fscanf(inputFile, "%f", &(remissionRelapseMortalityInputs.remissionMortality[MALE][i][WITH_DIABETES]));
        fscanf(inputFile, "%f", &(remissionRelapseMortalityInputs.remissionMortality[FEMALE][i][WITH_DIABETES]));
    }
    readAndSkipPast("ProbAAVDeath", inputFile);
    fscanf(inputFile, "%f", &remissionRelapseMortalityInputs.pAAV_death);
};

/* readDiabetesInputs reads data from the Diabetes tab of the input sheet */
void SimContext::readDiabetesInputs() {
    vector<int> diabetesAgeBuckets = {0, 18, 45, 65, 101};
    int i, j;
    char buffer[256];
    vector<double> diabetesInputsForBucket(2, 0);
    vector<vector<double>> diabetesFromRemissionVector(NUM_DIABETES_AGE_BUCKETS, diabetesInputsForBucket);

    readAndSkipPast("DiabetesFromRemissionRisk", inputFile);
    for (i = 0; i < NUM_DIABETES_AGE_BUCKETS; i++) {
        readAndSkipPast(DIABETES_AGE_BUCKET_STRS[i], inputFile);
        fill(diabetesInputsForBucket.begin(), diabetesInputsForBucket.end(), 0);
        fscanf(inputFile, "%lf", &(diabetesInputsForBucket[MALE]));
        fscanf(inputFile, "%lf", &(diabetesInputsForBucket[FEMALE]));
        diabetesFromRemissionVector[i] = diabetesInputsForBucket;
        for (j = diabetesAgeBuckets[i]; j < diabetesAgeBuckets[i+1]; j++) {
            diabetesInputs.diabetesFromRemission[j][MALE] = diabetesFromRemissionVector[i][MALE];
            diabetesInputs.diabetesFromRemission[j][FEMALE] = diabetesFromRemissionVector[i][FEMALE];
        }
    }

    readAndSkipPast("DiabetesFromActiveRisk", inputFile);
    for (i = 0; i < NUM_STEROID_REGIMENS - 1; i++) { // NUM_STEROID_REGIMENS - 1 because "No Steroid" doesn't count as an actual steroid
        vector<double> diabetesInputsForBucket(2,0);
        readAndSkipPast(STEROID_REGIMENS_STRS[i], inputFile);
        fscanf(inputFile, "%lf", &(diabetesInputs.diabetesFromActive[i][MALE]));
        fscanf(inputFile, "%lf", &(diabetesInputs.diabetesFromActive[i][FEMALE]));
    }

    readAndSkipPast("SteroidRegimen", inputFile);
    fscanf(inputFile, "%32s", buffer);
    diabetesInputs.steroidRegimen = buffer;
};

/* readAndSkipPast and readAndSkipPast2 skip over the given text in the input file */
bool SimContext::readAndSkipPast(const char* searchStr, FILE* file) {
    char temp[513];
    fscanf(file, "%512s", temp);
    while (strcmp(temp, searchStr) != 0) {
        fscanf(file, "%512s", temp);

        if (feof(file)) {
            if (counter <= 100) {
                printf("\nWARNING: unexpected end of input file. Looking for %s", searchStr);
                counter++;
            }

            return false;
        }
    }
    return true;
}  // readAndSkipPast
bool SimContext::readAndSkipPast2(const char* searchStr1, const char* searchStr2, FILE* file) {
    bool ret = readAndSkipPast(searchStr1, file);
    if (ret == true)
        ret = readAndSkipPast(searchStr2, file);
    return ret;
}  // readAndSkipPast2
