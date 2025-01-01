#ifndef GL_SIMVALS_H
#define GL_SIMVALS_H

#define SIM_BATTERY_LEVEL
#define SIM_BATTERY_HI (100)
#define SIM_BATTERY_LO (50)

#define SIM_HR
#define SIM_HR_HIGH (79)
#define SIM_HR_LOW (70)

// CPS uses gl_cps_mc_source, not SIM_CPS
#define SIM_CPS_INST_POWER_HI (49) // Keep low to not get active minutes when debugging
#define SIM_CPS_INST_POWER_LO (40)
#define SIM_CPS_CADENCE_HI (90)
#define SIM_CPS_CADENCE_LO (70)
//#define SIM_CPS_KPH_HI (30)
//#define SIM_CPS_KPH_LO (1)

// CSCS uses gl_cscs_mc_source, not SIM_CSCS
#define SIM_CSCS_INST_POWER_HI (49) // Keep low to not get active minutes when debugging
#define SIM_CSCS_INST_POWER_LO (40)
#define SIM_CSCS_CADENCE_HI (109)
#define SIM_CSCS_CADENCE_LO (100)
#define SIM_CSCS_KPH_HI (30)
#define SIM_CSCS_KPH_LO (1)


#endif

