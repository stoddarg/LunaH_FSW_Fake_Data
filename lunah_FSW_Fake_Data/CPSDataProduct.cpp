#include "stdafx.h"
#include "CPSDataProduct.h"

CPSDataProduct::CPSDataProduct(int ecut_low, int ecut_high, int ecut_w_low, int ecut_w_high, double psd_low, double psd_high, double psd_w_low, double psd_w_high)
	: m_ecut_low(ecut_low),
	m_ecut_high(ecut_high),
	m_ecut_wide_low(ecut_w_low),
	m_ecut_wide_high(ecut_w_high),
	m_psd_cut_low(psd_low),
	m_psd_cut_high(psd_high),
	m_psd_wide_low(psd_w_low),
	m_psd_wide_high(psd_w_high),
	m_cps_data{ 0xAA, 0, 0, 0, 0, 0, 0 }
{
	//initialize member variables:
	m_recorded_time = 0;
}


CPSDataProduct::~CPSDataProduct()
{
}


//this function will check each tally to see if the inputs are within the cuts
void CPSDataProduct::cpsUpdateTallies(float energy, float psd)
{
	//check to see if the values are counted in any of the 4 tallies
	//is event greater than 10 MeV?
	if (energy > 100000)	//make the value here whatever 10 MeV is
		m_cps_data.num_events_over_threshold++;

	//does the event fit into the no PSD cut?
	if (energy > m_ecut_low)
		if (energy < m_ecut_high)
			m_cps_data.neutrons_NoPSD++;

	//check for neutrons in the primary cuts
	if (energy > m_ecut_low)
		if (energy < m_ecut_high)
			if (psd > m_psd_cut_low)
				if (psd < m_psd_cut_high)
					m_cps_data.neutrons_wPSD++;

	//check for neutrons in the secondary cuts
	//this number should be larger than the neutrons w/psd cut because these cuts are wider, but
	//will still catch all the neutrons from above
	if (energy > m_ecut_wide_low)
		if (energy < m_ecut_wide_high)
			if (psd > m_psd_wide_low)
				if (psd < m_psd_wide_high)
					m_cps_data.neutrons_wide_cut++;

	return;
}

//this function takes in the time from the event and checks to see if it is time to report a CPS event
//the return value of this function will indicate whether writeCPSEvent should be called
bool CPSDataProduct::cpsCheckTime(unsigned int time)
{
	bool mybool = false;
	float event_time = 0.0;
	float recorded_time_s = 0.0;

	event_time = convert_to_seconds(time);
	recorded_time_s = convert_to_seconds(m_recorded_time);
	//is the current time from the event more than one second past the previous recorded time?
	if (event_time >= (recorded_time_s + 1.0))
	{
		mybool = true;
		//set the current recorded time as the FPGA Time of First Event in 1s Interval
		m_cps_data.time = m_recorded_time;
		//update the recorded time, the next time to report an event is 1s after this time
		m_recorded_time = time;	
	}
	else
		mybool = false;

	recorded_time_s = convert_to_seconds(m_recorded_time);
	
	return mybool;
}