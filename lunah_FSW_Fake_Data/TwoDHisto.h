#pragma once

#define X_BINS				260
#define Y_BINS				30
#define energy_max_range	300000
#define psd_max_range		2.0

struct header_2dh
{
	int PMT_ID;
	unsigned int left_oor;
	unsigned int right_oor;
	unsigned int below_oor;
	unsigned int above_oor;
};

class TwoDHisto
{
private:
	unsigned int m_above_oor;	//above the PSD plot bins and out-of-range (oor)
	unsigned int m_below_oor;	//below the PSD plot bins and out-of-range (oor)
	unsigned int m_left_oor;	//to the left of the PSD plot bins and out-of-range (oor)
	unsigned int m_right_oor;	//to the right of the PSD plot bins and out-of-range (oor)
	int m_x_bin_number;
	int m_y_bin_number;
	float m_x_binsize;
	float m_y_binsize;
	
	//struct to hold header info
	header_2dh m_header_data;

	//array to hold the two-dimensional histogram tallies
	unsigned short m_twoDH_pmt[X_BINS][Y_BINS];

public:
	//constructors
	TwoDHisto(int PMT_ID);
	~TwoDHisto();

	//functions
	void tally_2DH(float energy_bin, float psd_bin);

	int get2DHXArrayIndex();
	unsigned int get2DHYArrayIndex();

	float get2DHEnergyBinSize() { return static_cast<float>(energy_max_range / X_BINS); }
	float get2DHPSDBinSize() { return static_cast<float>(psd_max_range / Y_BINS); }

	header_2dh * get2DHHeader() 
	{
		m_header_data.left_oor = m_left_oor;
		m_header_data.right_oor = m_right_oor;
		m_header_data.below_oor = m_below_oor;
		m_header_data.above_oor = m_above_oor;
		return &m_header_data; 
	}
	
	unsigned short * get2DHArray() { return &(m_twoDH_pmt[0][0]); };
};

