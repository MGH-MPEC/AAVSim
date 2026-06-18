#pragma once

#include "include.h"

// Age Buckets

class SimContext
{
public:
    /* Constructors and Destructor */
    SimContext(string runName);
    ~SimContext(void);
    int counter;
    /* Mis cohort and simulation constants */
    /** Max number of years that could be lived by patient */
    static const int AGE_YRS = 101;
    /** Youngest patient age possible (for life tables) */
    static const int AGE_STARTING = 0;
    /** Oldest patient age possible (for life tables )*/
    static const int AGE_MAXIMUM = 100;
    /** Number of treatment arms */
    static const int NUM_TREATMENT_ARMS = 3;
    /** Strings corresponding to the treatment arms */
    char TREATMENT_ARMS[NUM_TREATMENT_ARMS][32] = {"RemissionTailored", "RemissionFixed", "RemissionBCell"};
    /** Number of treatment factors that impact probability of relapse */
    static const int NUM_RELAPSE_FACTORS = 6;
    /** Strings corresponding to the relapse treatment factors */
    char RELAPSE_FACTORS_STRS[NUM_RELAPSE_FACTORS][32] = { "TailoredTreatment",
        "FixedTreatment", "MissedRTX", "BCellUndet", "BCellRepop", "FromInactiveDisease"};
    /** Number of genders/sexes */
    static const int NUM_GENDERS_SPECIFIED = 2;
    /** The specific genders available */
    enum GENDER_TYPE {GENDER_MALE, GENDER_FEMALE};
    /** Number of ANCA Types */
    static const int NUM_ANCA_TYPES = 2;
    /** Number of ESRD States */
    static const int ESRD_STATE = 2; // Positive or negative for ESRD
    /** Number of Diabetes States */
    static const int DIABETES_STATE = 2; // Positive or negative for Diabetes
    /** Number of Renal Involvement States */
    static const int RENAL_INV_STATE = 2;
    /** Number of factors that impact monthly probability of COVID Infection */
    static const int NUM_COVID_INFECT_FACTORS = 8;
    /** Number of factors that impact probability of COVID Hospitalization and Mortality */
    static const int NUM_COVID_HOSP_MORTALITY_FACTORS = 11;
    /** Number of age buckets for covid infection probabilities*/
    static const int NUM_COVID_AGE_BUCKETS = 6;
    /** Strings corresponding to covid age bucket ranges */
    char COVID_AGE_BUCKETS[NUM_COVID_AGE_BUCKETS][32] = { "0-17", "18-29" , "30-49", "50-64", "65-79", "80+" };
    /** Number of age buckets for ESRD */
    static const int NUM_ESRD_AGE_BUCKETS = 15;
    /** Strings corresponding to ESRD Age bucket ranges */
    char ESRD_AGE_BUCKETS[NUM_ESRD_AGE_BUCKETS][32] = { "0-21" , "22-24", "25-29" , "30-34" , "35-39" , "40-44" , "45-49" , "50-54" ,
        "55-59" , "60-64" , "65-69" , "70-74" , "75-79" , "80-84" , "85-100" };
    /** Number of age buckets for ESRD Mortality */
    static const int NUM_ESRD_MORTALITY_AGE_BUCKETS = 14;
    /** Strings corresponding to ESRD Age bucket ranges */
    char ESRD_MORTALITY_AGE_BUCKETS[NUM_ESRD_MORTALITY_AGE_BUCKETS][32] = { "0-24" , "25-29" , "30-34" , "35-39" , "40-44" , "45-49" , "50-54" ,
        "55-59" , "60-64" , "65-69" , "70-74" , "75-79" , "80-84" , "85-100" };
    /** Number of age buckets for ESRD Mortality with Diabetes */
    static const int NUM_ESRD_MORTALITY_DIABETES_AGE_BUCKETS = 11;
    /** Strings corresponding to ESRD Mortality with Diabetes  Age bucket ranges */
    char ESRD_MORTALITY_DIABETES_AGE_BUCKETS[NUM_ESRD_MORTALITY_DIABETES_AGE_BUCKETS][32] = {"0-17", "18-29", "30-39", "40-49", "50-59", "60-64", 
        "65-69", "70-74", "75-79", "80-84", "85-100"};
    /** Number of age buckets for Severe Infection */
    static const int NUM_INFECTION_AGE_BUCKETS = 6;
    /** Strings corresponding to Infection Age bucket ranges */
    char INFECTION_AGE_BUCKETS_STRS[NUM_INFECTION_AGE_BUCKETS][32] = { "0" , "1-17" , "18-44" , "45-64" , "65-84", "85+" };
    /** Number of Diabetes age buckets */
    static const int NUM_DIABETES_AGE_BUCKETS = 4;
    /** Strings corresponding to Diabetes Age bucket ranges */
    char DIABETES_AGE_BUCKET_STRS[NUM_DIABETES_AGE_BUCKETS][32] = { "0-17" , "18-44" , "45-64" , "65-100" };
    /** Number of steroid regimens */
    static const int NUM_STEROID_REGIMENS = 4;
    /** Strings corresponding to steoroid regimens */
    char STEROID_REGIMENS_STRS[NUM_STEROID_REGIMENS][32] = { "Reduced" , "Standard" , "Observational", "No Steroid" };
    /** Number of patients to trace */
    static int numPatientsToTrace;


    /** CohortParamsInputs class contains inputs from the Cohort Parameters tab */
    class CohortParamsInputs {
    public:
        string runName;
        /** Cohort Parameters B2*/
        int numPatients;
        /** Cohort Parameters B3*/
        float ageMean;
        /** Cohort Parameters B4 */
        float ageStdDev;
        /* Cohort Parameters B5 */
        float probMale;
        /* Cohort Parameters B7 */
        float probRenalInv;
        /** Cohort Parameters B9 */
        float probPR3;
        /** Cohort Parameters B11*/ 
        int distType;

        /** Cohort Parameters B14 */
        float initRemission;
        /** Cohort Parameters B15-B16 */
        float initActive[2];
        /** Cohort Parameters B17 */
        float initSevere;
        /** Cohort Parameters B18 */
        float initCovid;
        /** Cohort Parameters B21 */
        float initESRD;
        /** Cohort Parameters B22 */
        float initDiabetes;

        /* Probabilities of Length of time from Active to Inactive State */

        vector<float> lengthActiveToInactiveProbabilities;
        /** Cohort Parameters F2 */
        int endMonth;
        /** Cohort Parameters F4 */
        float probANCARise;
        /** Cohort Parameters F5 */
        float probBCellRise;
        /** Cohort Parameters F7 */
        float probRenalFromActive;
        /** Cohort Parameters F10 */
        int monthStopRTX;

        /* Vaccination Toggles*/
        /** Cohort Parameters F14 */
        bool vaccAtBCellRise;
        /** Cohort Parameters F15 */
        bool vaccEffDelay;
        /** Cohort Parameters F16 */
        int stratAfterVacc;
        /** Cohort Parameters F17 */
        int stratDelayMonthsVacc;

        /* Relapse Toggles*/
        /** Cohort Parameters F21*/
        bool changeStratAfterRelapse;
        /** Cohort Parameters F22 */
        bool includeMinorRelapse;
        /** Cohort Parameters F23 */
        int stratAfterRelapse;
        /** Cohort Parameters F24 */
        int stratDelayMonthsRelapse;
        /** Random Seed by Time B27 */
        bool randSeedByTime;
    };

    class RelapseInfectionInputs {
    public:
        /** Major Relapse Monthly Probabilities */
        float majRelapse[NUM_RELAPSE_FACTORS][NUM_ANCA_TYPES][ESRD_STATE];
        /** Minor Relapse Monthly Probabilities */
        float minRelapse[NUM_RELAPSE_FACTORS][NUM_ANCA_TYPES][ESRD_STATE];
        /** Severe Infection from Remission Monthly Probabilities */
        float infectionRemission[NUM_TREATMENT_ARMS][DIABETES_STATE][ESRD_STATE];
        /** Severe Infection from Active Monthly Probabilities [STEROID REGIMEN][DIABETES STATE][ESRD STATE][NEW OR RELAPSING ACTIVE STATE]*/
        float infectionActive[NUM_STEROID_REGIMENS][DIABETES_STATE][ESRD_STATE][2];
    };

    class CovidInputs {
    public: 
        // Initial COVID Stats variables
        float pastCovidInfection;
        float probCovidVaccine;
        float probPastInfectionAndVaccine;
        float probVaccineUpToDate;
        float probVaccineUpToDateAndPastInfection;

        // Covid Surge Variables
        bool covidSurge;
        int covidSurgeMonthStart;
        int covidSurgeMonthEnd;
        float covidSurgeMultiplier;

        // Covid PrEP variables
        float covidPrEPA;
        float covidPrEPB;
        float covidPrEPC;

        // Covid Treatment variables
        float covidTreatmentA;
        float covidTreatmentB;
        float covidTreatmentC;

        /** Vaccine Up To Date*/
        int vaccineUpToDateMonthsMean; 
        int vaccineUpToDateMonthsStdDev;

        /** Raw Age Stratified Probabilities (in age buckets)*/
        vector<vector<float>> CovidInfectionVect;
        vector<vector<float>> CovidHospitalizationVect;
        vector<vector<float>> CovidMortalityVect;

        // COVID Infection Monthly probabilities
        float covidInfection[NUM_COVID_INFECT_FACTORS][AGE_YRS];
        /** Probability of hospitalization due to COVID */
        float covidHosp[NUM_COVID_HOSP_MORTALITY_FACTORS][AGE_YRS];
        /** COVID Mortality probability */
        float covidMortality[NUM_COVID_HOSP_MORTALITY_FACTORS][AGE_YRS];
    };
    
    class ESRDInputs {
    public:
        float ESRDFromRemission[AGE_YRS][NUM_GENDERS_SPECIFIED][RENAL_INV_STATE][DIABETES_STATE];
        float ESRDFromActive[AGE_YRS][NUM_GENDERS_SPECIFIED][RENAL_INV_STATE][DIABETES_STATE];
    };

    class ESRDMortalityInputs {
    public:
        float ESRDMortality[AGE_YRS][NUM_GENDERS_SPECIFIED][DIABETES_STATE];
    };

    class InfectionMortalityInputs {
    public:
        float infectionMortality[AGE_YRS][DIABETES_STATE];
    };

    class RemissionRelapseMortalityInputs {
    public:
        float remissionMortality[NUM_GENDERS_SPECIFIED][AGE_YRS][DIABETES_STATE];
        float inactiveDiseaseMortality[NUM_GENDERS_SPECIFIED][AGE_YRS][DIABETES_STATE];
        float activeMortality[NUM_GENDERS_SPECIFIED][AGE_YRS][DIABETES_STATE];
        /** Probability that death during the Active state was actually caused by AAV and not background death */
        float pAAV_death;
    };

    class DiabetesInputs {
    public:
        double diabetesFromRemission[AGE_YRS][NUM_GENDERS_SPECIFIED];
        double diabetesFromActive[NUM_STEROID_REGIMENS][NUM_GENDERS_SPECIFIED];
        string steroidRegimen;
    };


    void readInputs();

    /* public accessor functions that return const pointers to the input data classes */
    const CohortParamsInputs *getCohortParamsInputs();
    const RelapseInfectionInputs *getRelapseInfectioninputs();
    const CovidInputs *getCovidInputs();
    const ESRDInputs *getESRDInputs();
    const ESRDMortalityInputs *getESRDMortalityInputs();
    const InfectionMortalityInputs *getInfectionMortalityInputs();
    const RemissionRelapseMortalityInputs *getRemissionRelapseMortalityInputs();
    const DiabetesInputs *getDiabetesInputs();

    /* function for expanding vectors containing raw data into properly-stratified 2-d arrays */
    void expandFloatVect(vector<vector<float>> vect, float arr[][101], vector<int> buckets);


private:
    string inputFileName;
    FILE *inputFile;

    /* Classes for storing the input data */
    CohortParamsInputs cohortParamsInputs;
    RelapseInfectionInputs relapseInfectionInputs;
    CovidInputs covidInputs;
    ESRDInputs esrdInputs;
    ESRDMortalityInputs esrdMortalityInputs;
    InfectionMortalityInputs infectionMortalityInputs;
    RemissionRelapseMortalityInputs remissionRelapseMortalityInputs;
    DiabetesInputs diabetesInputs;

    /* Private functions for reading in the inputs, called by readInputs()*/
    void readCohortParamsInputs();
    void readRelapseInfectionInputs();
    void readCovidInputs();
    void readESRDInputs();
    void readESRDMortalityInputs();
    void readInfectionMortalityInputs();
    void readRemissionRelapseMortalityInputs();
    void readDiabetesInputs();
    bool readAndSkipPast(const char* searchStr, FILE* file);
    bool readAndSkipPast2(const char* searchStr1, const char *searchStr2, FILE *file);
};

/* getCohortParamsInputs returns a const pointer to CohortParamsInputs data class */
inline const SimContext::CohortParamsInputs* SimContext::getCohortParamsInputs() {
    return &cohortParamsInputs;
}
/* getRelapseInfectionInputs returns a const pointer to RelapseInfectionInputs data class*/
inline const SimContext::RelapseInfectionInputs* SimContext::getRelapseInfectioninputs() {
    return &relapseInfectionInputs;
}
/* getCovidInputs returns a const pointer to CovidInputs data class*/
inline const SimContext::CovidInputs* SimContext::getCovidInputs() {
    return &covidInputs;
}
/* getESRDInputs returns a const pointer to ESRDInputs data class*/
inline const SimContext::ESRDInputs* SimContext::getESRDInputs() {
    return &esrdInputs;
}
/* getESRDMortalityInputs returns a const pointer to ESRDMortalityInputs data class*/
inline const SimContext::ESRDMortalityInputs* SimContext::getESRDMortalityInputs() {
    return &esrdMortalityInputs;
}
/* getInfectionMortalityInputs returns a const pointer to InfectionMortalityInputs data class*/
inline const SimContext::InfectionMortalityInputs* SimContext::getInfectionMortalityInputs() {
    return &infectionMortalityInputs;
}
/* getRemissionRelapseMortalityInputs returns a const pointer to RemissionRelapseMortalityInputs data class*/
inline const SimContext::RemissionRelapseMortalityInputs* SimContext::getRemissionRelapseMortalityInputs() {
    return &remissionRelapseMortalityInputs;
}
/* getDiabetesInputs returns a const pointer to DiabetesInputs data class*/
inline const SimContext::DiabetesInputs* SimContext::getDiabetesInputs() {
    return &diabetesInputs;
}
