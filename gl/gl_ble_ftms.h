#ifndef GL_BLE_FTMS_H
#define GL_BLE_FTMS_H

#include <stdint.h>


//**********************************************************************
// FITNESS MACHINE SERVICE
#define FITNESS_MACHINE_SERVICE_UUID 	"00001826-0000-1000-8000-00805f9b34fb"

//*********
// Service Data AD type--how to specify??
// https://stackoverflow.com/questions/68106926/what-does-the-ble-scanrecords-getservicedata-contain
struct ftms_adv_struct{
	uint8_t service_data_ad_type;
	uint16_t ftms_uuid;
	uint8_t flags;
	uint16_t ftms_type;
};
#define FTMS_ADV_FLAGS_FITNESS_MACHINE_AVAILABLE (0x01)
#define FTMS_ADV_TYPE_TREADMILL_SUPPORTED		(0x01)
#define FTMS_ADV_TYPE_CROSS_TRAINER_SUPPORTED	(0x02)
#define FTMS_ADV_TYPE_STEP_CLIMBER_SUPPORTED		(0x04)
#define FTMS_ADV_TYPE_STAIR_CLIMBER_SUPPORTED	(0x08)
#define FTMS_ADV_TYPE_ROWER_SUPPORTED			(0x10)
#define FTMS_ADV_TYPE_INDOOR_BIKE_SUPPORTED		(0x20)


//*********
// Fitness Machine Feature
// Mandatory, read
#define FTMS_FC_UUID		"00002acc-0000-1000-8000-00805f9b34fb"
struct ftms_fc_struct{
	uint32_t feature;
	uint32_t target;
};
#define FTMS_FC_FEATURE_AVERAGE_SPEED_SUPPORTED		(0x00000001)
#define FTMS_FC_FEATURE_CADENCE_SUPPORTED			(0x00000002)
#define FTMS_FC_FEATURE_TOTAL_DISTANCE_SUPPORTED	(0x00000004)
#define FTMS_FC_FEATURE_INCLINATION_SUPPORTED		(0x00000008)
#define FTMS_FC_FEATURE_ELEVATION_GAIN_SUPPORTED	(0x00000010)
#define FTMS_FC_FEATURE_PACE_SUPPORTED				(0x00000020)
#define FTMS_FC_FEATURE_STEP_COUNT_SUPPORTED		(0x00000040)
#define FTMS_FC_FEATURE_RESISTANCE_LEVEL_SUPPORTED	(0x00000080)
#define FTMS_FC_FEATURE_STRIDE_COUNT_SUPPORTED		(0x00000100)
#define FTMS_FC_FEATURE_EXPENDED_ENERGY_SUPPORTED	(0x00000200)
#define FTMS_FC_FEATURE_HEART_RATE_SUPPORTED		(0x00000400)
#define FTMS_FC_FEATURE_MET_SUPPORTED				(0x00000800)
#define FTMS_FC_FEATURE_ELAPSED_TIME_SUPPORTED		(0x00001000)
#define FTMS_FC_FEATURE_REMAINING_TIME_SUPPORTED	(0x00002000)
#define FTMS_FC_FEATURE_POWER_MEASUREMENT_SUPPORTED	(0x00004000)
#define FTMS_FC_FEATURE_BELT_FORCE_POWER_SUPPORTED	(0x00008000)
#define FTMS_FC_FEATURE_USER_DATA_SUPPORTED			(0x00010000)
#define FTMS_FC_TARGET_SPEED_SETTING_SUPPORTED					(0x00000001)
#define FTMS_FC_TARGET_INCLINATION_SETTING_SUPPORTED			(0x00000002)
#define FTMS_FC_TARGET_RESISTANCE_SETTING_SUPPORTE				(0x00000004)
#define FTMS_FC_TARGET_POWER_SETTING_SUPPORTED					(0x00000008)
#define FTMS_FC_TARGET_HEART_RATE_SETTING_SUPPORTED				(0x00000010)
#define FTMS_FC_TARGET_EXP_ENERGY_CONFIG_SUPPORTED				(0x00000020)
#define FTMS_FC_TARGET_STEP_NUMBER_CONFIG_SUPPORTED				(0x00000040)
#define FTMS_FC_TARGET_STRIDE_NUMBER_CONFIG_SUPPORTED			(0x00000080)
#define FTMS_FC_TARGET_DISTANCE_CONFIG_SUPPORTED				(0x00000100)
#define FTMS_FC_TARGET_TRAINING_TIME_CONFIG_SUPPORTED			(0x00000200)
#define FTMS_FC_TARGET_TWO_HEART_RATE_ZONES_CONFIG_SUPPORTED	(0x00000400)
#define FTMS_FC_TARGET_THREE_HEART_RATE_ZONES_CONFIG_SUPPORTED	(0x00000800)
#define FTMS_FC_TARGET_FIVE_HEART_RATE_ZONES_CONFIG_SUPPORTED	(0x00001000)
#define FTMS_FC_TARGET_INDOOR_BIKE_SIM_PARAMS_SUPPORTED			(0x00002000)
#define FTMS_FC_TARGET_WHEEL_CIRCUMFERENCE_CONFIG_SUPPORTED		(0x00004000)
#define FTMS_FC_TARGET_SPIN_DOWN_CONTROL_SUPPORTED				(0x00008000)
#define FTMS_FC_TARGET_CADENCE_CONFIG_SUPPORTED					(0x00010000)


//*********
// Treadmill Data
// Optional, notify
#define FTMS_TDC_UUID		"00002acd-0000-1000-8000-00805f9b34fb"
struct ftms_tdc_struct{
	uint16_t flags;
	uint16_t instantaneous_speed; // 0.01km/hr (always except when MORE_DATA==1)
	uint16_t average_speed; // 0.01km/hr
	uint8_t  total_distance[3]; // m (0.001km)
	int16_t  ramp_inclination; // 0.1% (0x7fff if not available)
	int16_t  ramp_angle; // 0.1 degrees (0x7ff if not available)
	uint16_t pos_elevation_gain; // 0.1m
	uint16_t neg_elevation_gain; // 0.1m
	uint8_t  instantaneous_pace; // 0.1 k/min
	uint8_t  average_pace; // 0.1 k/min
	uint16_t energy_total; // kCals (0x7fff if not available)
	uint16_t energy_per_hour; // kCals (0x7fff if not available)
	uint8_t  energy_per_min; // kCals (0x7fff if not available)
	uint8_t  heart_rate; // BPM
	uint8_t	 met; // 0.1
	uint16_t elapsed_time; // s
	uint16_t remaining_time; // s
	int16_t  belt_force; // N (0x7fff if not available)
	int16_t  belt_power; // Watts (0x7fff if not available)
};
// Link says instantaneous_pace and average_pace are now uint16_t (no evidence)
// https://stackoverflow.com/questions/79100470/bluetooth-data-from-fitness-machines-are-not-matching-the-standard-from-bluetoot
#define FTMS_TDC_MORE_DATA						(0x0001)
#define FTMS_TDC_AVERAGE_SPEED_PRESENT			(0x0002)
#define FTMS_TDC_TOTAL_DISTANCE_PRESENT			(0x0004)
#define FTMS_TDC_RAMP_INCLINATION_ANGLE_PRESENT	(0x0008)
#define FTMS_TDC_ELEVATION_GAIN_PRESENT			(0x0010)
#define FTMS_TDC_INSTANTANEOUS_PACE_PRESENT		(0x0020)
#define FTMS_TDC_AVERAGE_PACE_PRESENT			(0x0040)
#define FTMS_TDC_ENERGY_PRESENT					(0x0080)
#define FTMS_TDC_HEART_RATE_PRESENT				(0x0100)
#define FTMS_TDC_MET_PRESENT					(0x0200)
#define FTMS_TDC_ELAPSED_TIME_PRESENT			(0x0400)
#define FTMS_TDC_REMAINING_TIME_PRESENT			(0x0800)
#define FTMS_TDC_BELT_FORCE_POWER_PRESENT		(0x1000)
#define FTMS_TDC_SPEED_SCALE_KMPH (0.01)
#define FTMS_TDC_SPEED_SCALE_MPH (FTMS_TDC_SPEED_SCALE_KMPH * (1.0/1.60934))
#define FTMS_TDC_DISTANCE_SCALE_KM (0.001)
#define FTMS_TDC_DISTANCE_SCALE_MILES (FTMS_TDC_DISTANCE_SCALE_KM * (1.0/1.60934))
#define FTMS_TDC_RAMP_INCLINATION_PERCENT (0.1)
#define FTMS_TDC_POS_ELEVATION_GAIN_M (0.1)
#define FTMS_TDC_POS_ELEVATION_GAIN_FT (FTMS_TDC_POS_ELEVATION_GAIN_M * 3.2084)
#define FTMS_TDC_ELAPSED_TIME_S (1)


//*********
// Indoor Bike Data
// Optional, notify
#define FTMS_IBDC_UUID		"00002ad2-0000-1000-8000-00805f9b34fb"
struct ftms_ibdc_struct{
	uint16_t flags;
	uint16_t instantaneous_speed; // 0.01km/hr (always except when MORE_DATA==1)
	uint16_t average_speed; // 0.01km/hr
	uint16_t instantaneous_cadence; // 0.5 rpm
	uint16_t average_cadence; //0.5 rpm
	uint8_t   total_distance[3]; // m
	int16_t  resistance_level; // dimensionless
	int16_t  instantaneous_power; // 1 W
	int16_t  average_power; // 1W
	uint16_t energy_total; // 1 kCal
	uint16_t energy_per_hour; // 1 kCal
	uint8_t  energy_per_min; // 1 kCal
	uint8_t  heart_rate; // BPM
	uint8_t	 met; // 0.1
	uint16_t elapsed_time; // s
	uint16_t remaining_time; // s
};
// Link says resistance is now uint8_t (no evidence)
// https://stackoverflow.com/questions/79100470/bluetooth-data-from-fitness-machines-are-not-matching-the-standard-from-bluetoot
#define FTMS_IBDC_MORE_DATA						(0x0001)
#define FTMS_IBDC_INSTANTANEOUS_CADENCE_PRESENT	(0x0002)
#define FTMS_IBDC_AVERAGE_SPEED_PRESENT			(0x0004)
#define FTMS_IBDC_AVERAGE_CADENCE_PRESENT		(0x0008)
#define FTMS_IBDC_TOTAL_DISTANCE_PRESENT		(0x0010)
#define FTMS_IBDC_RESISTANCE_LEVEL_PRESENT		(0x0020)
#define FTMS_IBDC_INSTANTANEOUS_POWER_PRESENT	(0x0040)
#define FTMS_IBDC_AVERAGE_POWER_PRESENT			(0x0080)
#define FTMS_IBDC_EXP_ENERGY_PRESENT			(0x0100)
#define FTMS_IBDC_HEART_RATE_PRESENT			(0x0200)
#define FTMS_IBDC_MET_PRESENT					(0x0400)
#define FTMS_IBDC_ELAPSED_TIME_PRESENT			(0x0800)
#define FTMS_IBDC_REMAINING_TIME_PRESENT		(0x1000)
#define FTMS_IBDC_SPEED_KMPH (0.01)
#define FTMS_IBDC_SPEED_MPH (FTMS_IBD_SPEED_KMPH * (1.0/1609.34)
#define FTMS_IBDC_DISTANCE_KM (1.0)
#define FTMS_IBDC_DISTANCE_MILES (FTME_IBD_DISTANCE_KM * (1.0/1.60934)
#define FTMS_IBDC_CADENCE_RPM (0.5)


//*********
// Cross Trainer Data
// Optional, notify
#define FTMS_CTDC_UUID

//*********
// Step Climber Data
// Optional, notify
#define FTMS_STPCDC_UUID

//*********
// Stair Climber Data
// Optional, notify
#define FTMS_STRCDC_UUID

//*********
// Rower Data
// Optional, notify
#define FTMS_RDC_UUID


//*********
// Training Status
// Optional, read, notify
#define FTMS_TSC_UUID

//*********
// Supported Speed Range
// Mandatory, read if speed target supported
#define FTMS_SSRC_UUID

//*********
// Supported Inclination Range
// Mandatory, read if inclination target supported
#define FTMS_SIRC_UUID

//*********
// Supported Resistance Level Range
// Mandatory, read if resistance target supported
#define FTMS_SRLRC_UUID

//*********
// Supported Power Range
// Mandatory, read if power target supported
#define FTMS_SPRC_UUID

//*********
// Supported Heart Rate Range
// Mandatory, read if heart rate target supported
#define FTMS_SHRRC_UUID

//*********
// Fitness Machine Control Point
// Optional, write, indicate
#define FTMS_CPC_UUID

//*********
// Fitness Machine Status
// Mandatory, notify if fitness machine control point supported
#define FTMS_SC_UUID


//**********************************************************************
// FTMS service structure
struct ftms_struct{
	struct ftms_adv_struct adv;
	struct ftms_fc_struct fc;
	struct ftms_tdc_struct tdc;
	struct ftms_ibdc_struct ibdc;
};


//**********************************************************************
int gl_ble_ftms_init( Application *application, struct ftms_struct *p_ftms );

GByteArray* gl_ble_ftms_tdc_struct_to_gba( struct ftms_tdc_struct *p_tdc );

int gl_ble_ftms_tdc_gba_to_struct( const GByteArray *byteArray, struct ftms_tdc_struct *p_tdc );

int gl_ble_ftms_tdc_receive_notify( const GByteArray *byteArray,  struct ftms_tdc_struct *p_tdc );

void gl_ble_ftms_free( struct ftms_struct *p_ftms );

gboolean gl_ble_ftms_tdc_simulate( gpointer gptr );

int gl_ble_ftms_ibdc_gba_to_struct( const GByteArray *byteArray, struct ftms_ibdc_struct *p_ibdc );

int gl_ble_ftms_ibdc_receive_notify( const GByteArray *byteArray,  struct ftms_ibdc_struct *p_ibdc );








#endif



