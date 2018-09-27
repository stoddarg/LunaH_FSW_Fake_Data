#pragma once

//the size of this struct is 20 bytes
struct event_CPS_dp
{
	unsigned char event_id = 0xAA;
	unsigned short neutrons_wPSD;
	unsigned short neutrons_wide_cut;
	unsigned short neutrons_NoPSD;
	unsigned short num_events_over_threshold;
	unsigned int time;
	char module_temp;
};

class CPSDataProduct
{
private:
	unsigned int m_recorded_time;
	int m_ecut_low;
	int m_ecut_high;
	int m_ecut_wide_low;
	int m_ecut_wide_high;
	double m_psd_cut_low;
	double m_psd_cut_high;
	double m_psd_wide_low;
	double m_psd_wide_high;

	event_CPS_dp m_cps_data;
	
public:
	CPSDataProduct(int ecut_low, int ecut_high, int ecut_w_low, int ecut_w_high, double psd_low, double psd_high, double psd_w_low, double psd_w_high);
	~CPSDataProduct();

	void cpsUpdateTallies(float energy, float psd);
	bool cpsCheckTime(unsigned int time);

	void cpsSetModuleTemp(char temp) { m_cps_data.module_temp = temp; }
	void cpsSetRecordedTime(unsigned int time) { m_recorded_time = time; }
	void cpsSetFirstEventTime(unsigned int time) { m_cps_data.time = time; }
	
	event_CPS_dp * cpsGetEvent() { return &m_cps_data; };
	
	float convert_to_seconds(float time) { return (time * static_cast<float>(0.000262144)); }
};

