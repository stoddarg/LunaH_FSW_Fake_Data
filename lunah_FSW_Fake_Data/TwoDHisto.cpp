#include "stdafx.h"
#include "TwoDHisto.h"

//default constructor
TwoDHisto::TwoDHisto(int PMT_ID) 
	: m_above_oor(0),
	m_below_oor(0), 
	m_left_oor(0), 
	m_right_oor(0), 
	m_x_bin_number(0),
	m_y_bin_number(0),
	m_header_data{PMT_ID, 0, 0, 0, 0},
	m_twoDH_pmt{}
{
	m_x_binsize = static_cast<float>(energy_max_range / x_bins);
	m_y_binsize = static_cast<float>(psd_max_range / y_bins);
}

//default destructor
TwoDHisto::~TwoDHisto()
{
}


//getter function for the array index
//returns the bin number if it's within the accepted values
// otherwise returns 0x3FF (max value for the bit field reporting bin number
int TwoDHisto::get2DHXArrayIndex()
{
	int bin_number{ 0 };
	//test the Energy number
	if (0 <= m_x_bin_number)
		if (m_x_bin_number < (x_bins - 1))
			bin_number = m_x_bin_number;	//the x bin is in the range
		else
			bin_number = 0x3FF;	//outside the range, set to max value for 10 bits
	else
		bin_number = 0x3FF;

	return bin_number;
}


//getter function for the array index
//returns the bin number if it's within the accepted values
// otherwise returns 0x3FF (max value for the bit field reporting bin number
unsigned int TwoDHisto::get2DHYArrayIndex()
{
	unsigned int bin_number{ 0 };
	//test the Energy number
	if (0 <= m_y_bin_number)
		if (m_y_bin_number < (y_bins - 1))
		{
			//the x bin is in the range
			bin_number = m_y_bin_number;	
			bin_number = bin_number << 26;
			//mask off any bits that were shifted in during the previous operation
			bin_number &= 0xFC000000;
		}
		else
			bin_number = 0xFC000000;	//outside the range, set to max value for 10 bits
	else
		bin_number = 0xFC000000;

	return bin_number;
}


//main function for determining where the energy and psd values go into the 2DH
void TwoDHisto::tally_2DH(float energy_value, float psd_value)
{
	float energy_bin = 0.0;
	float psd_bin = 0.0;
	//calculate the energy and psd bin numbers
	energy_bin = energy_value / m_x_binsize;
	psd_bin = psd_value / m_y_binsize;
	//sort the bin numbers based on the defined ranges
	if (0 <= energy_bin)
	{
		if (energy_bin <= (x_bins - 1))
		{
			if (0 <= psd_bin)
			{
				if (psd_bin <= (y_bins - 1))
				{
					//cast the bin numbers as ints so that we can use them as array indices
					m_x_bin_number = static_cast<int>(energy_bin);
					m_y_bin_number = static_cast<int>(psd_bin);
					//increment the bin in the matrix
					++m_twoDH_pmt[m_x_bin_number][m_y_bin_number];
				}
				else
					++m_above_oor;
			}
			else
				++m_below_oor;
		}
		else
		{
			if (0 <= psd_bin)
			{
				if (psd_bin <= (y_bins - 1))
					++m_right_oor;
				else
					++m_above_oor;
			}
			else
				++m_below_oor;
		}
	}
	else
	{
		if (0 <= psd_bin)
		{
			if (psd_bin <= (y_bins - 1))
				++m_left_oor;
			else
				++m_above_oor;
		}
		else
			++m_below_oor;
	}

	return;
}