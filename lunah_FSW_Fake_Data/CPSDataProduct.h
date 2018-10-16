#pragma once

//the size of this struct is 20 bytes
struct event_CPS_dp
{
	unsigned char event_id = 0xAA;
	unsigned char neutrons_wPSD_MSB;
	unsigned char neutrons_wPSD_LSB;
	unsigned char neutrons_wide_cut_MSB;
	unsigned char neutrons_wide_cut_LSB;
	unsigned char neutrons_NoPSD_MSB;
	unsigned char neutrons_NoPSD_LSB;
	unsigned char num_events_over_threshold_MSB;
	unsigned char num_events_over_threshold_LSB;
	unsigned char time_MSB;
	unsigned char time_LSB3;
	unsigned char time_LSB2;
	unsigned char time_LSB1;
	char module_temp;
};

class CPSDataProduct
{
private:
	unsigned int m_time;
	unsigned int m_recorded_time;
	int m_ecut_low;
	int m_ecut_high;
	int m_ecut_wide_low;
	int m_ecut_wide_high;
	double m_psd_cut_low;
	double m_psd_cut_high;
	double m_psd_wide_low;
	double m_psd_wide_high;

	//holder variables which will be split in two to put into chars
	unsigned short m_neutrons_wPSD;
	unsigned short m_neutrons_wide_cut;
	unsigned short m_neutrons_NoPSD;
	unsigned short m_num_events_over_threshold;

	event_CPS_dp m_cps_data;
	
public:
	CPSDataProduct(int ecut_low, int ecut_high, int ecut_w_low, int ecut_w_high, double psd_low, double psd_high, double psd_w_low, double psd_w_high);
	~CPSDataProduct();

	void cpsUpdateTallies(float energy, float psd);
	bool cpsCheckTime(unsigned int time);

	void cpsSetModuleTemp(char temp) { m_cps_data.module_temp = temp; }
	void cpsSetRecordedTime(unsigned int time) { m_recorded_time = time; }
	void cpsSetFirstEventTime(unsigned int time) { m_time = time; }
	
	event_CPS_dp * cpsGetEvent() { 
		//when we return the struct, we need to convert the time correctly
		m_cps_data.time_MSB = static_cast<unsigned char>(m_time >> 24);
		m_cps_data.time_LSB3 = static_cast<unsigned char>(m_time >> 16);
		m_cps_data.time_LSB2 = static_cast<unsigned char>(m_time >> 8);
		m_cps_data.time_LSB1 = static_cast<unsigned char>(m_time);
		return &m_cps_data; 
	};
	
	float convert_to_seconds(unsigned int time) { return (time * static_cast<float>(0.000262144)); }
};

