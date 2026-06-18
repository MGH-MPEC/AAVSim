#pragma once 

#include "include.h"

extern int monthsSinceLastBCellRise;
extern bool anca_rise_this_month;
extern bool bcell_rise_this_month;
extern bool rtx_missed_this_month;
extern bool covid_hospitalization_this_month;
extern bool esrd_this_month;
extern bool diabetes_this_month;
extern int monthsActive;
extern int monthsRelapse;

bool probOfHappening(float prob);

float getProbDeath(Patient* patient, SimContext* simContext);

float RollNextStateGivenProbs(float pInactive, float pActive, float pSevere, float pDeath, float pMinor,
	float pCovid, float pMinorFromActive);

int RollAncaOrBCellRise(float probANCA, float probBCell, float nothing);

int tailoredUpdateState(Patient* patient, SimContext* simContext, StateUpdater* stateUpdater);

int fixedUpdateState(Patient* patient, SimContext *simContext, StateUpdater *stateUpdater);

int bCellUpdateState(int tx, Patient *patient, SimContext *simContext, StateUpdater *stateUpdater);

