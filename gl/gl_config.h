#ifndef GL_CONFIG_H
#define GL_CONFIG_H

// GENERIC DEFINES--customize for specific applications

//**********************************************************************
// Advertising data
#define GYMLINK_ADV_MANUFACTURER_ID_TESTING (0xFFFF)
#define GYMLINK_ADV_MANUFACTURER_ID_INNOVA (0x0c51) //  "INNOVA S.R.L" 5 person investment firm in Italy
#define GYMLINK_ADV_MANUFACTURER_ID_ZIMI (0x08ce) // Acquired by Xioami in 2021; probably went away

//**********************************************************************
// Device Information Service
#define GYMLINK_DIS_MODEL_NUMBER  "GymLink"
#define GYMLINK_DIS_SERIAL_NUMBER "#1"
#define GYMLINK_DIS_HARDWARE_REV  "0.01"
#define GYMLINK_DIS_FIRMWARE_REV  "0.02"
#define GYMLINK_DIS_SOFTWARE_REV  "0.03"
#define GYMLINK_DIS_MANUFACTURER  "Innovaq"
//#define GYMLINK_DIS_OUI_ZIMI (0x6cd3ee) // Also 74:A3:4A
//const char *gymlink_dis_oui_zimi = "0x6cd3ee";


//**********************************************************************
// Battery Service
#define GYMLINK_BS_BLC_NOTIFY_MS (10000)

//**********************************************************************
// Heart Rate Service
#define GYMLINK_HRS_BSLC_FLAGS (HRS_BSLC_CHEST)
#define GYMLINK_HRS_MC_NOTIFY_MS (1000)

//**********************************************************************
// Cycle Power Service
#define CPS_FC_ALL \
	CPS_FC_PEDAL_POWER_BALANCE_SUPPORTED |\
	CPS_FC_ACC_TORQUE_SUPPORTED |\
	CPS_FC_WHEEL_REVOLUTION_DATA_SUPPORTED |\
	CPS_FC_CRANK_REVOLUTION_DATA_SUPPORTED |\
	CPS_FC_EXTREME_MAGNITUDES_SUPPORTED |\
	CPS_FC_EXTREME_ANGLES_SUPPORTED |\
	CPS_FC_TOP_AND_BOTTOM_DEAD_SPOT_ANGLES_SUPPORTED |\
	CPS_FC_ACCUMULATED_ENERGY_SUPPORTED |\
	CPS_FC_OFFSET_COMPENSATION_INDICATOR_SUPPORTED |\
	CPS_FC_OFFSET_COMPENSATION_SUPPORTED |\
	CPS_FC_CPS_CPMC_CONTENT_MASKING_SUPPORTED |\
	CPS_FC_MULTIPLE_SENSOR_LOCATIONS_SUPPORTED |\
	CPS_FC_CRANK_LENGTH_ADJUSTMENT_SUPPORTED |\
	CPS_FC_CHAIN_LENGTH_ADJUSTMENT_SUPPORTED |\
	CPS_FC_SPAN_LENGTH_ADJUSTMENT_SUPPORTED |\
	CPS_FC_SENSOR_MEASUREMENT_CONTEXT |\
	CPS_FC_INSTANTANEOUS_MEASUREMENT_DIRECTION_SUPPORTED |\
	CPS_FC_FACTORY_CALIBRATION_DATE_SUPPORTED |\
	CPS_FC_ENHANCED_OFFSET_COMPENSATION_SUPPORTED |\
	CPS_FC_DISTRIBUTED_SYSTEM_SUPPORT_FALSE )
// If support WHEEL_REVOLUTION_DATA, mandatory to support support Cycling Power Control Point p.17
// 12/9/24 -- If support WHEEL_REVOLUTION_DATA, after CPC indication is confirmed on server, Fenix8 disconnects (???)
// Garmin Fenix8 will write, receive indication, and confirm receipt, but then disconnects after confirming the indication
//#define GYMLINK_CPS_FC_FLAGS (CPS_FC_ALL) // Use to fuzz opcodes Garmin will send to server
#define GYMLINK_CPS_FC_FLAGS (CPS_FC_CRANK_REVOLUTION_DATA_SUPPORTED | CPS_FC_WHEEL_REVOLUTION_DATA_SUPPORTED | CPS_FC_DISTRIBUTED_SYS_SUPPORT_FALSE)
#define GYMLINK_CPS_SLC_FLAGS (CPS_SLC_SPIDER) // Pelomon says Peloton uses LEFT_CRANK
#define GYMLINK_CPS_MC_NOTIFY_MS (500) // BLE spec must be <= 1000 (increments of 1Hz update)

//**********************************************************************
// Cycling Speed and Cadence Service
#define GYMLINK_CSCS_FC_FLAGS (CSCS_FC_WHEEL_REVS_SUPPORTED | CSCS_FC_CRANK_REVS_SUPPORTED)
#define GYMLINK_CSCS_SLC_FLAGS (CSCS_SLC_SPIDER) // Pelomon says Peloton uses LEFT_CRANK
#define GYMLINK_CSCS_MC_NOTIFY_MS (5000)
#define GYMLINK_CSCS_CPC_NOTIFY_MS (500)

//**********************************************************************
// Running Speed and Cadence Service
#define GYMLINK_RSCS_RSCFC_FLAGS (0)
#define GYMLINK_RSCS_SLC_FLAGS (RSCS_SLC_TOP_OF_SHOE)
#define GYMLINK_RSCS_MC_NOTIFY_MS (500)

//**********************************************************************
// Fitness Machine Service

//**********************************************************************
// M3i
#define GYMLINK_M3I_BEACON_NAME "M3"


#endif

