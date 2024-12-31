#ifndef K2CPS_H
#define K2CPS_H

// Make M3i speed mimic Kona Rove ST
#define K2CPS_KONA_TIRE_650B_CIRCUMFERENCE_KM (1.920/1000.0)
#define K2CPS_KONA_GEAR_RATIO (2.0) // Range from 3.64 to 0.95; pick 2


//**********************************************************************
// Advertising data
#define K2CPS_ADV_MANUFACTURER_ID (0xFFFF)

//**********************************************************************
// Device Information Service
#define K2CPS_DIS_MODEL_NUMBER  "k2cps"
#define K2CPS_DIS_SERIAL_NUMBER "001"
#define K2CPS_DIS_HARDWARE_REV  "0.8"
#define K2CPS_DIS_FIRMWARE_REV  "0.8"
#define K2CPS_DIS_SOFTWARE_REV  "0.8"
#define K2CPS_DIS_MANUFACTURER  "Innovaq"

//**********************************************************************
// Battery Service (placeholder; notify interval not dynamically changed based on battery level)
#define K2CPS_BS_BLC_NOTIFY_MS (10000)

//**********************************************************************
// Cycling Power Service
#define K2CPS_CPS_FC_FLAGS (CPS_FC_CRANK_REVOLUTION_DATA_SUPPORTED | CPS_FC_WHEEL_REVOLUTION_DATA_SUPPORTED | CPS_FC_DISTRIBUTED_SYS_SUPPORT_FALSE)
#define K2CPS_CPS_SLC_FLAGS (CPS_SLC_SPIDER)

#endif

