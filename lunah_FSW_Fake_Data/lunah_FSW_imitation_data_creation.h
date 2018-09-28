#pragma once

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iomanip>
#include "DateClass.h"
#include "CPSDataProduct.h"
#include "TwoDHisto.h"

//define some values which may need to change later
#define bl_samples			0
#define short_samples		73
#define long_samples		169
#define full_samples		1551
#define ecut_low			20000
#define ecut_high			95000
#define psd_cut_low			0.20
#define psd_cut_high		0.35
#define wide_ecut_low		10000
#define wide_ecut_high		110000
#define wide_psd_cut_low	0.16
#define wide_psd_cut_high	0.40
#define ecal_slope			1
#define ecal_int			0
#define threshold			8900
#define hv0					200
#define hv1					200
#define hv2					200
#define hv3					200
#define CPS_MAX_EVENTS		139
#define CPS_HEADER_SIZE		81
#define CPS_EVENT_SIZE		14
#define CPS_DP_EVENT_SIZE	20
#define EVTS_MAX_EVENTS		244
#define EVTS_HEADER_SIZE	81
#define EVTS_EVENT_SIZE		8
#define TwoDH_HEADER_SIZE	69
#define SYNC_MARKER_SIZE	4
#define CHECKSUM_SIZE		4

//struct definitions
struct raw_event
{
	unsigned int event_id;
	unsigned int time;
	unsigned int total_count;
	unsigned int eventNum_HitID;
	unsigned int baselineInt;
	unsigned int shortInt;
	unsigned int longInt;
	unsigned int fullInt;
};

struct header_info
{
	unsigned long long real_time;
	int first_event_time;
	int baseline_int;
	int short_int;
	int long_int;
	int full_int;
	short orbit_number;
	short run_number;
	unsigned char file_type;
};

struct first_event_EVTS_dp
{
	unsigned char event_id_0 = 0xDD;
	unsigned char event_id_1 = 0xDD;
	unsigned char event_id_2 = 0xDD;
	unsigned char event_id_3 = 0xDD;
	unsigned int null_time;
};

struct temp_EVTS_dp
{
	unsigned char event_id_0 = 0xEE;
	char analog_board_temp;
	char digital_board_temp;
	char module_temp;
	unsigned char event_id_1 = 0xEE;
	unsigned char event_id_2 = 0xEE;
	unsigned char event_id_3 = 0xEE;
	unsigned char event_id_4 = 0xEE;
};

struct event_EVTS_dp
{
	unsigned int evtID_hitID_totEvts_E = 0xFF000000;
	unsigned int PSD_time;
};

//function prototypes
float convert_to_seconds(float time);
