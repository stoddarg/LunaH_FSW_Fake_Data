// LunaH_text_transposer.cpp : Defines the entry point for the console application.
/* I want to read in a data file, process it into data products, then save it to a file.
* Then read from that file based on the header of the saved data products to create output
* Mini-NS data product packets.
*
* Settings: Mini-NS Detector 0
* reset request = 0x00
*
*/

#include "stdafx.h"
#include "lunah_FSW_imitation_data_creation.h"

//function definitions
float convert_to_seconds(float time) { return (time * static_cast<float>(0.000262144)); }

int main()
{

	DateClass tomorrow{ 2018, 9, 12 };
	tomorrow.setDate(2019, 3, 8);
	tomorrow.print();

	//Variables
	std::string infile = "";
	std::string outfile = "";
	int i{};
	int event_counter{};
	unsigned short s_holder{};
	int i_holder{};
	float f_holder{};
	int length{};
	int bytes_to_read{};
	int sequence_count{};
	int packet_size{};
	int iterator{};
	int rmd_checksum_simple{0};
	int rmd_checksum_Fletcher{0};
	unsigned short bct_checksum{0};
	long long int temp1(0);
	long long int temp2(0);
	int pmt_id_holder(0);
	int temp_holder(0);
	int total_events_holder(0);
	int point_outside(0);
	float f_time_holder(0.0);
	float si(0.0); float li(0.0); float fi(0.0);
	float bl1(0.0);	float bl2(0.0); float bl3(0.0); float bl4(0.0); float bl_avg(0.0);
	float psd(0.0); float energy(0.0);
	int x_bin_number(0);
	unsigned int y_bin_number(0);
	unsigned int time_holder{};
	float xbinsize(0.0);	//PSD plot bins 
	float ybinsize(0.0);	//PSD plot bins

	//Structs
	raw_event read_event = {};
	header_info header_info{};
	first_event_EVTS_dp first_event{ 0xDD ,0xDD ,0xDD ,0xDD ,0 };
	event_EVTS_dp data_event = {};
	temp_EVTS_dp temp_event = {};
	CPSDataProduct CPSData(ecut_low, ecut_high, wide_ecut_low, wide_ecut_high, psd_cut_low, psd_cut_high, wide_psd_cut_low, wide_psd_cut_high);
	TwoDHisto twoDH_pmt0(0);
	TwoDHisto twoDH_pmt1(1);
	TwoDHisto twoDH_pmt2(2);
	TwoDHisto twoDH_pmt3(3);
	TwoDHisto twoDH_pmt_total(4);	//no PMT ID

	//Arrays
	char getline_buff[100] = "";
	unsigned char struct_to_char_write_buffer[sizeof(first_event_EVTS_dp)] = "";
	unsigned char float_holder_buff[sizeof(float)] = "";
	unsigned char byte_reorder_buff[8] = "";
	unsigned char header_info_holder[100] = "";
	unsigned char event_holder_buff[100] = "";
	unsigned char packet_array[2040] = "";

	//Files
	std::ifstream infileStream; // Open stream for reading
	std::ofstream ofs_evts_file;
	std::ofstream ofs_cps_file;
	std::ofstream ofs_2dh_file0;
	std::ofstream ofs_2dh_file1;
	std::ofstream ofs_2dh_file2;
	std::ofstream ofs_2dh_file3;
	std::ofstream ofs_2dh_file_total;
	std::ofstream ofs_data_packet;

	//Make sure the user has written the correct filename in:
	//K:\users\GStoddard\Miscellany\LunaH FSW Imitation Data Product Packet Creation\lunah_FSW_Fake_Data\lunah_FSW_Fake_Data
	printf("This program is using this file path as the base string:\nK:\\users\\GStoddard\\Miscellany\\LunaH FSW Imitation Data Product Packet Creation\\lunah_FSW_Fake_Data\\lunah_FSW_Fake_Data\\\n");
	std::string work_dir = "K:\\users\\GStoddard\\Miscellany\\LunaH FSW Imitation Data Product Packet Creation\\lunah_FSW_Fake_Data\\lunah_FSW_Fake_Data\\";

	//prints to see sizes:
	printf("The header_info is: %d bytes\n", sizeof(header_info));
	printf("The 2dh_header is: %d bytes\n", sizeof(header_2dh));
	printf("The 2dh size is: %d bytes\n", sizeof(unsigned short) * X_BINS * Y_BINS);

	 //Comment/Uncomment this line to start/stop producing data products (and line 440)

	//begin by opening the data file
	//infile = "K:\\Research Files\\NASA\\C16-018 LunaH-Map\\Hardware Development\\Assembly Testing and Verification\\Digital Board FM\\XQ1\\XQ1_25C_START\\DATA_SOURCE_CS1_1555_06_27_18_25C_XQ1_translated.txt";
	infile = work_dir + "test_file_large.txt";
	infileStream.open(infile, std::ios::in);	//open the file

	//open the file for the EVTS data product
	outfile = work_dir + "data_products_evts.bin";
	ofs_evts_file.open(outfile, std::ios::binary | std::ios::app);	//open the file in binary, append mode
	//open the file for the CPS data product
	outfile = "";
	outfile = work_dir + "data_products_cps.bin";
	ofs_cps_file.open(outfile, std::ios::binary | std::ios::app);
	//open the file for the 2DH data product files (one for each histogram)
	outfile = "";
	outfile = work_dir + "data_products_2dh_0.bin";
	ofs_2dh_file0.open(outfile, std::ios::binary | std::ios::app);
	outfile = "";
	outfile = work_dir + "data_products_2dh_1.bin";
	ofs_2dh_file1.open(outfile, std::ios::binary | std::ios::app);
	outfile = "";
	outfile = work_dir + "data_products_2dh_2.bin";
	ofs_2dh_file2.open(outfile, std::ios::binary | std::ios::app);
	outfile = "";
	outfile = work_dir + "data_products_2dh_3.bin";
	ofs_2dh_file3.open(outfile, std::ios::binary | std::ios::app);
	outfile = "";
	outfile = work_dir + "data_products_2dh_total.bin";
	ofs_2dh_file_total.open(outfile, std::ios::binary | std::ios::app);

	//create in the header information that would get included in a normal data file
	header_info.real_time = 1234567;
	header_info.first_event_time = 2345678;
	header_info.baseline_int = bl_samples;
	header_info.short_int = short_samples;
	header_info.long_int = long_samples;
	header_info.full_int = full_samples;
	header_info.orbit_number = 1;
	header_info.run_number = 1;
	header_info.file_type = 0x77;	//see table 7 for APIDs in the v9.0.2 of ICD
	//write the header into each file
	int num = sizeof(header_info);
	ofs_evts_file.write(reinterpret_cast<char *>(&header_info), sizeof(header_info));
	header_info.file_type = 0x55;	//see table 7 for APIDs in the v9.0.2 of ICD
	ofs_cps_file.write(reinterpret_cast<char *>(&header_info), sizeof(header_info));
	header_info.file_type = 0x88;	//see table 7 for APIDs in the v9.0.2 of ICD
	ofs_2dh_file0.write(reinterpret_cast<char *>(&header_info), sizeof(header_info));
	ofs_2dh_file1.write(reinterpret_cast<char *>(&header_info), sizeof(header_info));
	ofs_2dh_file2.write(reinterpret_cast<char *>(&header_info), sizeof(header_info));
	ofs_2dh_file3.write(reinterpret_cast<char *>(&header_info), sizeof(header_info));
	ofs_2dh_file_total.write(reinterpret_cast<char *>(&header_info), sizeof(header_info));

	//this loop handles creation of data products
	while (infileStream.eof() == false)	//while the files we are utilizing are available
	{
		//zero out structs to use them again
		data_event = {};
		read_event = {};
		//read in one line at a time, trying to find one of three patterns:
		// for 2147594759, we have a false event (should just be one of these)
		// for 892,270,675 we have a temp event
		// for 111 111 we have a data event
		infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
		sscanf_s(getline_buff, "%u", &read_event.event_id);

		//test for which type of event we are looking at
		switch (read_event.event_id)
		{
		case 2147594759:
			//this is a false event
			infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
			sscanf_s(getline_buff, "%u", &read_event.event_id);
			if (read_event.event_id == 2147594759)
			{
				std::cout << "this is a false event\n";
				infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
				sscanf_s(getline_buff, "%u", &read_event.time);
			}
			else
			{
				std::cout << "a fake false event\n";
				break;
			}

			//we found a false event, take in the information from it, make a data product and save it
			//This should be the first type of event in each packet.
			//fill in the first event struct
			//get the first time
			time_holder = read_event.time >> 30;			//grabs the top 2 bits of the time
			first_event.null_time_msb = 0x00 | time_holder;	//sets the top 6 bits as 0, keeps the 2 MSBs of time
			time_holder = read_event.time >> 22;	first_event.time_lsb3 = time_holder;
			time_holder = read_event.time >> 14;	first_event.time_lsb2 = time_holder;
			time_holder = read_event.time >> 6;		first_event.time_lsb1 = time_holder;
			//save the first FPGA time to the CPS data
			//f_time_holder = convert_to_seconds(read_event.time);
			CPSData.cpsSetRecordedTime(read_event.time);

			//write to the binary data product file
			ofs_evts_file.write(reinterpret_cast<char *>(&first_event), sizeof(first_event_EVTS_dp));
			break;
		case 892270675:
			//this is a temp event
			infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
			sscanf_s(getline_buff, "%lli", &temp1);
			infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
			sscanf_s(getline_buff, "%lli", &temp2);
			//create the temp data product
			temp_event.event_id_0 = 0xEE;
			temp_event.event_id_1 = 0xEE;
			temp_event.event_id_2 = 0xEE;
			temp_event.event_id_3 = 0xEE;
			temp_event.event_id_4 = 0xEE;
			//use placeholders for the actual temperatures, there are not temp reads in this file
			temp_event.analog_board_temp = 24;
			temp_event.digital_board_temp = 25;
			temp_event.module_temp = 23;
			//write out to the file
			ofs_evts_file.write(reinterpret_cast<char *>(&temp_event), sizeof(temp_EVTS_dp));

			printf("this is a temp event\n");
			break;
		case 111111:
			//this is a data event
			infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
			sscanf_s(getline_buff, "%u", &read_event.time);
			if (read_event.time == 775274708)	//check to see if the value is the pop-up number; if so, read the next two values to clear the associated numbers, then read the actual value
			{
				infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
				infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
				infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
				if (read_event.time == 111111)
					infileStream.getline(getline_buff, sizeof(getline_buff), '\n');	//in case there is a second event identifier after the first one, read past it to get the actual value
				sscanf_s(getline_buff, "%u", &read_event.time);
			}
			if (read_event.time == 111111)
				infileStream.getline(getline_buff, sizeof(getline_buff), '\n');	//in case there is a second event identifier after the first one, read past it to get the actual value
			infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
			sscanf_s(getline_buff, "%u", &read_event.total_count);
			if (read_event.total_count == 775274708)
			{
				infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
				infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
				infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
				sscanf_s(getline_buff, "%u", &read_event.total_count);
			}
			infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
			sscanf_s(getline_buff, "%u", &read_event.eventNum_HitID);
			if (read_event.eventNum_HitID == 775274708)
			{
				infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
				infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
				infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
				sscanf_s(getline_buff, "%u", &read_event.eventNum_HitID);
			}
			infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
			sscanf_s(getline_buff, "%u", &read_event.baselineInt);
			if (read_event.baselineInt == 775274708)
			{
				infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
				infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
				infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
				sscanf_s(getline_buff, "%u", &read_event.baselineInt);
			}
			infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
			sscanf_s(getline_buff, "%u", &read_event.shortInt);
			if (read_event.shortInt == 775274708)
			{
				infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
				infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
				infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
				sscanf_s(getline_buff, "%u", &read_event.shortInt);
			}
			infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
			sscanf_s(getline_buff, "%u", &read_event.longInt);
			if (read_event.longInt == 775274708)
			{
				infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
				infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
				infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
				sscanf_s(getline_buff, "%u", &read_event.longInt);
			}
			infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
			sscanf_s(getline_buff, "%u", &read_event.fullInt);
			if (read_event.fullInt == 775274708)
			{
				infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
				infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
				infileStream.getline(getline_buff, sizeof(getline_buff), '\n');
				sscanf_s(getline_buff, "%u", &read_event.fullInt);
			}

			//before processing the event, check the time to see if we need to report CPS data yet
			if (CPSData.cpsCheckTime(read_event.time) == true)
			{
				//when checkTime evaluates to true, the function also updates the various times needed
				//set the temperature
				CPSData.cpsSetModuleTemp(23);
				//use reinterpret cast to have C++ allow us to pass this pointer
				ofs_cps_file.write(reinterpret_cast<char *>(CPSData.cpsGetEvent()), sizeof(event_CPS_dp));
			}

			//begin processing the event now that it is all read in
			//assume that every time we see a 111111, the next values are all good
			//put the event ID on
			data_event.evt_id = 0xFF;	//the event ID is 0xFF in the MSB 8 bits
			//get the hit ID
			pmt_id_holder = read_event.eventNum_HitID & 0xF;	//mask the top 28 bits off to get the 4 LSBs
			switch (pmt_id_holder)
			{
			case 1:
				data_event.pmtID_totalEVTsMSB |= 0x00;	//PMT 0
				break;
			case 2:
				data_event.pmtID_totalEVTsMSB |= 0x40;	//PMT 1
				break;
			case 4:
				data_event.pmtID_totalEVTsMSB |= 0x80;	//PMT 2
				break;
			case 8:
				data_event.pmtID_totalEVTsMSB |= 0xC0;	//PMT 3
				break;
			default:
				std::cout << "PMT Hit ID either 0 or multiple IDs.\n";
				break;
			}
			//get the total number of events so far
			total_events_holder = read_event.total_count & 0xFFF;
			data_event.pmtID_totalEVTsMSB |= total_events_holder >> 6;
			data_event.totalEVTsLSB_energyBinMSB |= total_events_holder << 2;
			//zero out values from last event
			si = 0.0;	li = 0.0;	fi = 0.0;	psd = 0.0;	energy = 0.0;
			//we have the data event read in now
			//calculate baseline moving average
			bl4 = bl3; bl3 = bl2; bl2 = bl1;
			bl1 = read_event.baselineInt / (static_cast<float>(16.0) * 38);
			if (bl4 == 0.0)
				bl_avg = bl1;
			else
				bl_avg = (bl4 + bl3 + bl2 + bl1) / static_cast<float>(4.0);
			//find integrals
			si = read_event.shortInt / static_cast<float>(16.0) - (bl_avg * short_samples);
			li = read_event.longInt / static_cast<float>(16.0) - (bl_avg * long_samples);
			fi = read_event.fullInt / static_cast<float>(16.0) - (bl_avg * full_samples);
			energy = fi;
			//calculate PSD
			if (si != 0)
				if (li != 0)
					psd = si / (li - si);
			//calculate the energy and PSD bins
			//here is where we will pass in the energy and psd to the 2DH class
			switch (pmt_id_holder)
			{
			case 1:
				//PMT 0 //use the first 2dh class object
				//add the energy and PSD to the 2DH array
				twoDH_pmt0.tally_2DH(energy, psd);
				//then get back the bin numbers for x, y
				x_bin_number = twoDH_pmt0.get2DHXArrayIndex();
				//write in the bin number to the different fields
				data_event.totalEVTsLSB_energyBinMSB |= static_cast<unsigned char>(x_bin_number >> 8);
				data_event.energyBinLSB |= static_cast<unsigned char>(x_bin_number);
				y_bin_number = twoDH_pmt0.get2DHYArrayIndex();
				data_event.PSDBin_timeMSB |= y_bin_number << 2;
				break;
			case 2:
				//PMT 1
				twoDH_pmt1.tally_2DH(energy, psd);
				//then get back the bin numbers for x, y
				x_bin_number = twoDH_pmt1.get2DHXArrayIndex();
				//write in the bin number to the different fields
				data_event.totalEVTsLSB_energyBinMSB |= static_cast<unsigned char>(x_bin_number >> 8);
				data_event.energyBinLSB |= static_cast<unsigned char>(x_bin_number);
				y_bin_number = twoDH_pmt1.get2DHYArrayIndex();
				data_event.PSDBin_timeMSB |= y_bin_number << 2;
				break;
			case 4:
				//PMT 2
				twoDH_pmt2.tally_2DH(energy, psd);
				//then get back the bin numbers for x, y
				x_bin_number = twoDH_pmt2.get2DHXArrayIndex();
				//write in the bin number to the different fields
				data_event.totalEVTsLSB_energyBinMSB |= static_cast<unsigned char>(x_bin_number >> 8);
				data_event.energyBinLSB |= static_cast<unsigned char>(x_bin_number);
				y_bin_number = twoDH_pmt2.get2DHYArrayIndex();
				data_event.PSDBin_timeMSB |= y_bin_number << 2;
				break;
			case 8:
				//PMT 3
				twoDH_pmt3.tally_2DH(energy, psd);
				//then get back the bin numbers for x, y
				x_bin_number = twoDH_pmt3.get2DHXArrayIndex();
				//write in the bin number to the different fields
				data_event.totalEVTsLSB_energyBinMSB |= static_cast<unsigned char>(x_bin_number >> 8);
				data_event.energyBinLSB |= static_cast<unsigned char>(x_bin_number);
				y_bin_number = twoDH_pmt3.get2DHYArrayIndex();
				data_event.PSDBin_timeMSB |= y_bin_number << 2;
				break;
			default:
				//No PMT ID or MULTIPLE PMT IDs
				std::cout << "PMT Hit ID either 0 or multiple IDs.\n";
				break;
			}
			
			//As long as there is an event, record the 2DH info in the total 2dh
			twoDH_pmt_total.tally_2DH(energy, psd);

			//put the time into the struct
			//mask off the top few bits of the 32-bit time
			time_holder = read_event.time & 0x3FFFFFF;
			data_event.PSDBin_timeMSB |= static_cast<unsigned char>(time_holder >> 24);
			data_event.timeLSB3 = static_cast<unsigned char>(time_holder >> 16);
			data_event.timeLSB2 = static_cast<unsigned char>(time_holder >> 8);
			data_event.timeLSB1 = static_cast<unsigned char>(time_holder);
			//we have put together the entire EVT struct, save it to the file
			// write to the binary data product file
			ofs_evts_file.write(reinterpret_cast<char *>(&data_event), sizeof(event_EVTS_dp));

			//update the tallies for the CPS data
			CPSData.cpsUpdateTallies(fi, psd);

			printf("this is data event %d\n", event_counter++);
			break;
		default:
			//none of these identifiers was found
			printf("not an event\n");
			memset(getline_buff, '0', sizeof(getline_buff));	//reset the scan buffer
			break;
		}
	}

	//finished reading the file, output the 2DH data products to file
	//save the 4 out-of-range values and the PMT ID as a header to the binary file
	//so, the format will be:
	//header info from above (default values, system properties, etc)
	//post-run header (out-of-range values, PMT ID)
	//2-D histogram bins
	ofs_2dh_file0.write(reinterpret_cast<char *>(twoDH_pmt0.get2DHHeader()), sizeof(header_2dh));
	ofs_2dh_file1.write(reinterpret_cast<char *>(twoDH_pmt1.get2DHHeader()), sizeof(header_2dh));
	ofs_2dh_file2.write(reinterpret_cast<char *>(twoDH_pmt2.get2DHHeader()), sizeof(header_2dh));
	ofs_2dh_file3.write(reinterpret_cast<char *>(twoDH_pmt3.get2DHHeader()), sizeof(header_2dh));
	ofs_2dh_file_total.write(reinterpret_cast<char *>(twoDH_pmt_total.get2DHHeader()), sizeof(header_2dh));
	//now write in the histogram bins
	int size_of_2dh_array = sizeof(unsigned short) * X_BINS * Y_BINS;
	ofs_2dh_file0.write(reinterpret_cast<char *>(twoDH_pmt0.get2DHArray()), size_of_2dh_array);
	ofs_2dh_file1.write(reinterpret_cast<char *>(twoDH_pmt1.get2DHArray()), size_of_2dh_array);
	ofs_2dh_file2.write(reinterpret_cast<char *>(twoDH_pmt2.get2DHArray()), size_of_2dh_array);
	ofs_2dh_file3.write(reinterpret_cast<char *>(twoDH_pmt3.get2DHArray()), size_of_2dh_array);
	ofs_2dh_file_total.write(reinterpret_cast<char *>(twoDH_pmt_total.get2DHArray()), size_of_2dh_array);
	//close out of the streams for now, so we are tidy
	ofs_evts_file.close();
	ofs_cps_file.close();
	ofs_2dh_file0.close();
	ofs_2dh_file1.close();
	ofs_2dh_file2.close();
	ofs_2dh_file3.close();
	ofs_2dh_file_total.close();
	infileStream.close();

	  //Comment/uncomment this line to start/stop producing data files each time

	/***** Create the data packets to be sent out from the Mini-NS ****/
//	system("pause");
	int data_bytes{};
	int iter{};
	int write_out{};
	event_CPS_dp holder_CPS_event{};
	header_2dh holder_2DH_header{};
	//open the file for reading
	//***** UNCOMMENT FOR EVTS FILE FOR READING *****
//	infile = work_dir + "data_products_evts.bin";
//	infileStream.open(infile, std::ios::binary);	//open the file
//	infileStream.seekg(0, infileStream.end);
//	length = static_cast<int>(infileStream.tellg());
//	infileStream.seekg(0, infileStream.beg);
	//***** UNCOMMENT FOR CPS FILE FOR READING ***** 
//	infile = work_dir + "data_products_cps.bin";
//	infileStream.open(infile, std::ios::binary);	//open the file
//	infileStream.seekg(0, infileStream.end);
//	length = static_cast<int>(infileStream.tellg());
//	infileStream.seekg(0, infileStream.beg);
	//***** UNCOMMENT FOR 2DH FILE FOR READING ***** 
//	infile = work_dir + "data_products_2dh_0.bin";
//	infileStream.open(infile, std::ios::binary);	//open the file
//	infileStream.seekg(0, infileStream.end);
//	length = static_cast<int>(infileStream.tellg());
//	infileStream.seekg(0, infileStream.beg);
	//***** UNCOMMENT FOR 2DH FILE FOR READING ***** 
//	infile = work_dir + "data_products_2dh_1.bin";
//	infileStream.open(infile, std::ios::binary);	//open the file
//	infileStream.seekg(0, infileStream.end);
//	length = static_cast<int>(infileStream.tellg());
//	infileStream.seekg(0, infileStream.beg);
	//***** UNCOMMENT FOR 2DH FILE FOR READING ***** 
//	infile = work_dir + "data_products_2dh_2.bin";
//	infileStream.open(infile, std::ios::binary);	//open the file
//	infileStream.seekg(0, infileStream.end);
//	length = static_cast<int>(infileStream.tellg());
//	infileStream.seekg(0, infileStream.beg);
	//***** UNCOMMENT FOR 2DH FILE FOR READING ***** 
	infile = work_dir + "data_products_2dh_3.bin";
	infileStream.open(infile, std::ios::binary);	//open the file
	infileStream.seekg(0, infileStream.end);
	length = static_cast<int>(infileStream.tellg());
	infileStream.seekg(0, infileStream.beg);
	//***** UNCOMMENT FOR 2DH FILE FOR READING ***** 
//	infile = work_dir + "data_products_2dh_total.bin";
//	infileStream.open(infile, std::ios::binary);	//open the file
//	infileStream.seekg(0, infileStream.end);
//	length = static_cast<int>(infileStream.tellg());
//	infileStream.seekg(0, infileStream.beg);
	
	//read in data from the data products binary file
	//create a packet based on the data read in
	//open the file to write to:
	//outfile = work_dir + "EVTS_data_packet_" + std::to_string(sequence_count) + ".bin";
	//outfile = work_dir + "CPS_data_packet_" + std::to_string(sequence_count) + ".bin";
	//outfile = work_dir + "2DH_0_data_packet_" + std::to_string(sequence_count) + ".bin";
	//outfile = work_dir + "2DH_1_data_packet_" + std::to_string(sequence_count) + ".bin";
	//outfile = work_dir + "2DH_2_data_packet_" + std::to_string(sequence_count) + ".bin";
	outfile = work_dir + "2DH_3_data_packet_" + std::to_string(sequence_count) + ".bin";
	//outfile = work_dir + "2DH_total_data_packet_" + std::to_string(sequence_count) + ".bin";
	ofs_data_packet.open(outfile, std::ios::binary);	//open the file in binary, append mode

	//read in the header of the file
	//specifically, look for the file type to determine what we will be writing out/make sure that we have the correct file
	struct header_info file_header_info {};
	infileStream.read((char *)header_info_holder, sizeof(header_info));	//sizeof header info is 40 bytes
	//subtract off the header that was read in
	length -= sizeof(file_header_info);
	//for the 2dh data, we have a secondary header to read:
	//comment this for other data types
	//get and write the Out-of-range values:
	infileStream.read(reinterpret_cast<char *>(&holder_2DH_header), sizeof(holder_2DH_header));
	length -= sizeof(holder_2DH_header);

	//copy in the real time from the header
	memcpy(&file_header_info, header_info_holder, sizeof(unsigned long long));
	memcpy(&(file_header_info.first_event_time), &(header_info_holder[8]), sizeof(unsigned int));
	file_header_info.file_type = header_info_holder[32];	//should be 0x55, 0x77, 0x88 for CPS, EVTS, 2DH

	//finish putting together the packet headers here
	//then just memset the values which need to be replaced each time, rather than rewriting the whole thing
	packet_array[0] = 0x35;
	packet_array[1] = 0x2E;
	packet_array[2] = 0xF8;
	packet_array[3] = 0x53;
	packet_array[4] = 0x0A;
	packet_array[5] = file_header_info.file_type;
	//need to get file size to see if this will be unsegmented or not
	packet_array[6] = 0xC0;	//this is the group flags plus the sequence count msb //call this unsegmented for now
	packet_array[7] = 0x00;	//this is the first packet until we get through
	packet_array[10] = 0x00; //no reset request atm
	//fill in the secondary header	//these are shared header values
	i_holder = ecut_low;			memcpy(&(packet_array[11]), &i_holder, sizeof(i_holder));
	i_holder = ecut_high;			memcpy(&(packet_array[15]), &i_holder, sizeof(i_holder));
	f_holder = psd_cut_low;			memcpy(&(packet_array[19]), &f_holder, sizeof(float));
	f_holder = psd_cut_high;		memcpy(&(packet_array[23]), &f_holder, sizeof(float));
	i_holder = wide_ecut_low;		memcpy(&(packet_array[27]), &i_holder, sizeof(i_holder));
	i_holder = wide_ecut_high;		memcpy(&(packet_array[31]), &i_holder, sizeof(i_holder));
	f_holder = wide_psd_cut_low;	memcpy(&(packet_array[35]), &f_holder, sizeof(float));
	f_holder = wide_psd_cut_high;	memcpy(&(packet_array[39]), &f_holder, sizeof(float));
	i_holder = ecal_slope;			memcpy(&(packet_array[43]), &i_holder, sizeof(i_holder));
	i_holder = ecal_int;			memcpy(&(packet_array[47]), &i_holder, sizeof(i_holder));
	s_holder = threshold;			memcpy(&(packet_array[51]), &s_holder, sizeof(s_holder));
	s_holder = bl_samples;			memcpy(&(packet_array[53]), &s_holder, sizeof(s_holder));
	s_holder = short_samples;		memcpy(&(packet_array[55]), &s_holder, sizeof(s_holder));
	s_holder = long_samples;		memcpy(&(packet_array[57]), &s_holder, sizeof(s_holder));
	s_holder = full_samples;		memcpy(&(packet_array[59]), &s_holder, sizeof(s_holder));
	s_holder = hv0;					memcpy(&(packet_array[61]), &s_holder, sizeof(s_holder));
	s_holder = hv1;					memcpy(&(packet_array[63]), &s_holder, sizeof(s_holder));
	s_holder = hv2;					memcpy(&(packet_array[65]), &s_holder, sizeof(s_holder));
	s_holder = hv3;					memcpy(&(packet_array[67]), &s_holder, sizeof(s_holder));

	//each loop should create one packet //that packet should be saved at the end of the loop
	bool loop_var{ true };
	while (loop_var == true)
	{
		printf("making the packet here\n");
		//fill in the secondary header because this is a 0x77 (EVTS) file
		switch (file_header_info.file_type)
		{
		case 0x55:	//this is a CPS file
			printf("bytes left to read: %d\n", length);
			//write in the Real Time from the spacecraft
			memcpy(&(packet_array[69]), &(file_header_info.real_time), sizeof(file_header_info.real_time));
			memcpy(&(packet_array[77]), &(file_header_info.first_event_time), sizeof(file_header_info.first_event_time));

			//use length to see how many bytes there are to read
			//each CPS packet can have CPS_MAX_EVENTS number of events per packet
			if (length >= CPS_MAX_EVENTS * CPS_DP_EVENT_SIZE)
			{
				bytes_to_read = CPS_MAX_EVENTS * CPS_DP_EVENT_SIZE;
				if (sequence_count == 0)
					packet_array[6] = 0x40;	//first packet
				else
					packet_array[6] = 0x00;	//intermediate packet
			}
			else
			{
				bytes_to_read = length;
				if (sequence_count == 0)
					packet_array[6] = 0xC0;	//unsegmented packet
				else
					packet_array[6] = 0x80;	//last packet
			}
			printf("Group flag: 0x%X\n", packet_array[6]);
			//write in the sequence count
			packet_array[6] |= (sequence_count & 0x3F00) >> 8;
			packet_array[7] = sequence_count & 0xFF;
			printf("Sequence count: %d or: %d\n", packet_array[7], sequence_count);
			//can just read the data to our array, as it is in byte-by-byte order
			infileStream.read(reinterpret_cast<char *>(packet_array + CPS_HEADER_SIZE), bytes_to_read);
			//calculate the size of the packet
			data_bytes = iter * CPS_EVENT_SIZE;
			packet_size = CPS_HEADER_SIZE + data_bytes + CHECKSUM_SIZE;
			packet_array[8] = (packet_size & 0xFF00) >> 8;
			packet_array[9] = packet_size & 0xFF;
			printf("The size of this packet is: %d (MSB) and %d (LSB)\n", packet_array[8], packet_array[9]);
			//put the CPS data in
			//infileStream.read(reinterpret_cast<char *>(packet_array + CPS_HEADER_SIZE), bytes_to_read);
			//calculate checksums
			//RMD Checksums //two 8-bit checksums of all bytes in the packet after the sync marker
			//BCT Checksums //16-bit sum of all bytes after the sync marker
			iterator = 0;//reset loop var
			while (iterator < (packet_size - SYNC_MARKER_SIZE - CHECKSUM_SIZE))	//including any bytes after sync marker and before checksums
			{
				rmd_checksum_simple = (rmd_checksum_simple + packet_array[SYNC_MARKER_SIZE + iterator]) % 255;	//simple
				rmd_checksum_Fletcher = (rmd_checksum_Fletcher + rmd_checksum_simple) % 255;	//Fletcher builds off of simple
				bct_checksum += packet_array[SYNC_MARKER_SIZE + iterator];
				iterator++;
			}
			//write in the checksum values
			packet_array[CPS_HEADER_SIZE + data_bytes] = rmd_checksum_simple;
			packet_array[CPS_HEADER_SIZE + data_bytes + 1] = rmd_checksum_Fletcher;
			packet_array[CPS_HEADER_SIZE + data_bytes + 2] = bct_checksum >> 8;
			packet_array[CPS_HEADER_SIZE + data_bytes + 3] = static_cast<unsigned char>(bct_checksum);
			//write in the packet_array
			ofs_data_packet.write(reinterpret_cast<char *>(packet_array), packet_size);
			sequence_count++;	//increase the sequence count 
			printf("printed %d (header) + %d (data) bytes\n", 81, bytes_to_read);

			break;
		case 0x77: //this is a data file (EVTS)
			printf("bytes left to read: %d\n", length);
			//Write packet specific bytes
			//write in the Real Time from the spacecraft
			memcpy(&(packet_array[69]), &(file_header_info.real_time), sizeof(file_header_info.real_time));
			memcpy(&(packet_array[77]), &(file_header_info.first_event_time), sizeof(file_header_info.first_event_time));
			//how many bytes left to read?
			if (length >= EVTS_MAX_EVENTS * EVTS_EVENT_SIZE)
			{
				bytes_to_read = EVTS_MAX_EVENTS * EVTS_EVENT_SIZE;
				if (sequence_count == 0)
					packet_array[6] = 0x40;	//first packet
				else
					packet_array[6] = 0x00;	//intermediate packet
			}
			else
			{
				bytes_to_read = length;
				if (sequence_count == 0)
					packet_array[6] = 0xC0;	//unsegmented packet
				else
					packet_array[6] = 0x80;	//last pcket
			}
			printf("Group flag: 0x%X\n", packet_array[6]);
			//write in the sequence count
			packet_array[6] |= (sequence_count & 0x3F00) >> 8;
			packet_array[7] = sequence_count & 0xFF;
			printf("Sequence count: %d or: %d\n", packet_array[7], sequence_count);
			//calculate the size of the packet length
			packet_size = EVTS_HEADER_SIZE + bytes_to_read + CHECKSUM_SIZE;
			packet_array[8] = (packet_size & 0xFF00) >> 8;
			packet_array[9] = packet_size & 0xFF;
			printf("The size of this packet is: %d (MSB) and %d (LSB)\n", packet_array[8], packet_array[9]);
			//read in the event from the data file
			infileStream.read((char *)(packet_array + EVTS_HEADER_SIZE), bytes_to_read);
			//subtract off the bytes that were read
			length -= bytes_to_read;
			//calculate and record the 4 checksums
			//RMD Checksums //two 8-bit checksums of all bytes in the packet after the sync marker
			//BCT Checksums //16-bit sum of all bytes after the sync marker
			iterator = 0;//reset loop var
			while (iterator < (packet_size - SYNC_MARKER_SIZE - CHECKSUM_SIZE))	//including any bytes after sync marker and before checksums
			{
				rmd_checksum_simple = (rmd_checksum_simple + packet_array[SYNC_MARKER_SIZE + iterator]) % 255;	//simple
				rmd_checksum_Fletcher = (rmd_checksum_Fletcher + rmd_checksum_simple) % 255;	//Fletcher builds off of simple
				bct_checksum += packet_array[SYNC_MARKER_SIZE + iterator];
				iterator++;
			}
			//write in the checksum values
			packet_array[EVTS_HEADER_SIZE + bytes_to_read] = rmd_checksum_simple;
			packet_array[EVTS_HEADER_SIZE + bytes_to_read + 1] = rmd_checksum_Fletcher;
			packet_array[EVTS_HEADER_SIZE + bytes_to_read + 2] = bct_checksum >> 8;
			packet_array[EVTS_HEADER_SIZE + bytes_to_read + 3] = static_cast<unsigned char>(bct_checksum);
			//write in the packet_array
			ofs_data_packet.write(reinterpret_cast<char *>(packet_array), packet_size);
			sequence_count++;	//increase the sequence count 
			break;
		case 0x88:	//this is a 2DH file
			printf("bytes left to read: %d\n", length);
			//write in the packet specific Mini-NS Data Header
			//write these values into the packet
			memcpy(&(packet_array[69]), &(holder_2DH_header.left_oor), sizeof(holder_2DH_header.left_oor));
			memcpy(&(packet_array[73]), &(holder_2DH_header.right_oor), sizeof(holder_2DH_header.right_oor));
			memcpy(&(packet_array[77]), &(holder_2DH_header.below_oor), sizeof(holder_2DH_header.below_oor));
			memcpy(&(packet_array[81]), &(holder_2DH_header.above_oor), sizeof(holder_2DH_header.above_oor));
			//use length to see how many bytes there are to read
			//each 2DH packet can have 2DH_MAX_EVENTS number of events per packet
			if (length >= TWODH_MAX_EVENTS * TWODH_EVENT_SIZE)
			{
				bytes_to_read = TWODH_MAX_EVENTS * TWODH_EVENT_SIZE;
				if (sequence_count == 0)
					packet_array[6] = 0x40;	//first packet
				else if (sequence_count == 7)
					packet_array[6] = 0x80;	//last packet
				else
					packet_array[6] = 0x00;	//intermediate packet
			}
			else
			{
				bytes_to_read = length;
				if (sequence_count == 0)
					packet_array[6] = 0xC0;	//unsegmented packet
				else
					packet_array[6] = 0x80;	//last packet
			}
			printf("Group flag: 0x%X\n", packet_array[6]);
			packet_array[6] |= (sequence_count & 0x3F00) >> 8;
			packet_array[7] = sequence_count & 0xFF;
			printf("Sequence count: %d or: %d\n", packet_array[7], sequence_count);
			packet_size = TWODH_HEADER_SIZE + bytes_to_read + CHECKSUM_SIZE;
			packet_array[8] = (packet_size & 0xFF00) >> 8;
			packet_array[9] = packet_size & 0xFF;
			printf("The size of this packet is: %d (MSB) and %d (LSB)\n", packet_array[8], packet_array[9]);
			//read in the event from the data file
			infileStream.read(reinterpret_cast<char *>(packet_array + TWODH_HEADER_SIZE), bytes_to_read);
			//subtract off the bytes that were read
			length -= bytes_to_read;
			//write in the PMT ID
			packet_array[TWODH_HEADER_SIZE + bytes_to_read - 1] = holder_2DH_header.PMT_ID;
			//calculate and record the 4 checksums
			//RMD Checksums //two 8-bit checksums of all bytes in the packet after the sync marker
			//BCT Checksums //16-bit sum of all bytes after the sync marker
			iterator = 0;//reset loop var
			while (iterator < (packet_size - SYNC_MARKER_SIZE - CHECKSUM_SIZE))	//including any bytes after sync marker and before checksums
			{
				rmd_checksum_simple = (rmd_checksum_simple + packet_array[SYNC_MARKER_SIZE + iterator]) % 255;	//simple
				rmd_checksum_Fletcher = (rmd_checksum_Fletcher + rmd_checksum_simple) % 255;	//Fletcher builds off of simple
				bct_checksum += packet_array[SYNC_MARKER_SIZE + iterator];
				iterator++;
			}
			//write in the checksum values
			packet_array[TWODH_HEADER_SIZE + bytes_to_read] = rmd_checksum_simple;
			packet_array[TWODH_HEADER_SIZE + bytes_to_read + 1] = rmd_checksum_Fletcher;
			packet_array[TWODH_HEADER_SIZE + bytes_to_read + 2] = bct_checksum >> 8;
			packet_array[TWODH_HEADER_SIZE + bytes_to_read + 3] = static_cast<unsigned char>(bct_checksum);
			//write in the packet_array
			ofs_data_packet.write(reinterpret_cast<char *>(packet_array), packet_size);
			sequence_count++;	//increase the sequence count 
			break;
		default:
			//no file type
			printf("wrong or no file type found\n");
			break;
		}

		//do the cleanup from this process
		//erase the parts of the packet array which are not shared between the data products
		memset(&(packet_array[69]), '0', sizeof(packet_array) - 69);
		
		//determine if we should loop again (to create another packet) or to exit the loop
		int group_flag = packet_array[6] >> 6;
		switch (group_flag)
		{
		case 0:
			//current packet is an intermediate packet, need to keep looping
			loop_var = true;
			//close the current data packet file, it is written and finished
			ofs_data_packet.close();
			//update the file name for the file we wish to create
			//outfile = work_dir + "EVTS_data_packet_" + std::to_string(sequence_count) + ".bin";
			//outfile = work_dir + "CPS_data_packet_" + std::to_string(sequence_count) + ".bin";
			//outfile = work_dir + "2DH_0_data_packet_" + std::to_string(sequence_count) + ".bin";
			//outfile = work_dir + "2DH_1_data_packet_" + std::to_string(sequence_count) + ".bin";
			//outfile = work_dir + "2DH_2_data_packet_" + std::to_string(sequence_count) + ".bin";
			outfile = work_dir + "2DH_3_data_packet_" + std::to_string(sequence_count) + ".bin";
			//outfile = work_dir + "2DH_total_data_packet" + std::to_string(sequence_count) + ".bin";
			ofs_data_packet.open(outfile, std::ios::binary);	//open the file in binary, append mode
			break;
		case 1:
			//current packet was the first packet, need to keep looping
			loop_var = true;
			//close the current data packet file, it is written and finished
			ofs_data_packet.close();
			//update the file name for the file we wish to create
			//outfile = work_dir + "EVTS_data_packet_" + std::to_string(sequence_count) + ".bin";
			//outfile = work_dir + "CPS_data_packet_" + std::to_string(sequence_count) + ".bin";
			//outfile = work_dir + "2DH_0_data_packet_" + std::to_string(sequence_count) + ".bin";
			//outfile = work_dir + "2DH_1_data_packet_" + std::to_string(sequence_count) + ".bin";
			//outfile = work_dir + "2DH_2_data_packet_" + std::to_string(sequence_count) + ".bin";
			outfile = work_dir + "2DH_3_data_packet_" + std::to_string(sequence_count) + ".bin";
			//outfile = work_dir + "2DH_total_data_packet_" + std::to_string(sequence_count) + ".bin";
			ofs_data_packet.open(outfile, std::ios::binary);	//open the file in binary, append mode
			break;
		case 2:
			//current packet was the last packet, we are done
			loop_var = false;
			break;
		case 3:
			//current packet is unsegmented, we are done
			loop_var = false;
			break;
		default:
			printf("NO group flag found! Check the CCSDS Header for errors\n");
			loop_var = false;
			break;
		}
	}

	infileStream.close();
	ofs_data_packet.close();

	printf("finished going through the file\n");

	system("pause");
	return 0;
}

