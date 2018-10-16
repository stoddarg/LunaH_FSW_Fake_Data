# LunaH_FSW_Fake_Data

A minor log for myself to keep track of what is going on.

9-28-2018 - Have updated the CPS data product creation, fully implemented the CPS data packet creation; that is finished. Need to implement 2DH packet creation, then this must be documented and the files sent out to ASU.

Version 1.1 - all 3 data products have been produced; EVTS packets done

Version 1.2 - CPS packets done

Version 1.3 - add final data product files; created data packet with multi-packet production for each data product

Version 2.0 - finalized 2DH data packets, have folders for all data products and all data packets

Version 2.01 - updated this file

Version 2.1 - corrected a few mistakes:
				+ was writing floating point numbers in the data header incorrectly
				+ bl_samples changed to 38
				+ changed data products from little-endian to byte-by-byte
				+ added 5th 2DH as a total event histogram
			- large change is that instead of just using random structs to hold values, structs are built up from single bytes, rather than shorts/ints, etc and
			  can now be written directly from the file into the data packets
			- when I go through and re-generate the data files, there will be a 5th 2DH (total)
			- no changes to the ICD are necessary to properly read the data packet files
				+ further information is needed if someone wants to process the raw data files