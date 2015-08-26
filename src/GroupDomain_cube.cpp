/*
Copyright (c) 2015, Advanced Micro Devices, Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "GroupDomain_cube.hh"
#include <iostream>
#include <stdlib.h>
#include <ctime>
#include <sys/time.h>
#include "Settings.hh"

extern struct Settings settings;

GroupDomain_cube::GroupDomain_cube( const char *name, uint cube_model_t, uint64_t chips_t, uint64_t banks_t, uint64_t burst_size_t, uint64_t cube_addr_dec_depth_t, uint64_t cube_ecc_tsv_t, uint64_t cube_redun_tsv_t, bool enable_tsv_t) : GroupDomain( name)
, dist(0,1)
, gen(eng,dist)
{
	//Register Cube Model
	cube_model_enable = cube_model_t;
	cube_addr_dec_depth=cube_addr_dec_depth_t; //Address Decoding Depth
	//Check if TSVs need to be enabled for Fault Modelling
	enable_tsv=enable_tsv_t;
	//Params to figure out the number of TSVs in the chip
	chips=chips_t;
	banks=banks_t;
	burst_size = burst_size_t;
	/**************************************************/
	//Total number of TSVs in each category	
	cube_ecc_tsv=cube_ecc_tsv_t;
	cube_redun_tsv=cube_redun_tsv_t;
	/*Assuming 32Bytes of DATA, we get 32*8 = 256 data bits out. If DDR is used, then we have 512 data bits out for horizontal channel config. Assuming 16Bytes of DATA, we get 16*8 = 128 data bits out. There are ~20 (16 Data Value + 4 ECC - may be) TSVs for Data per bank. If DDR is used and in 8 bursts, we will get 256 bits out for vertical channel config*/
	cube_data_tsv=burst_size / 2; // (DDR)
	/**************************************************/
	//Compute the number of Address TSVs required for each depth
	// DR DEBUG: this is wrong - also get chip size from the DRAMDomains
	total_addr_tsv = 0;

	/*
	//if(cube_model_enable==1)
	//	total_addr_tsv=chip_size>>10; // As 512 bits are drawn every access and each ADDR TSV is usually shared by 2 WLs and 2 Columns (due to size of TSV)

	//Compute the total Addr TSVs required in case of complete decode
	uint64_t org_tsv_count=total_addr_tsv;
	uint64_t complete_decode_tsv=0;
	asm ( "\tbsr %1, %0\n"
			: "=r"(complete_decode_tsv)
			  : "r" (org_tsv_count)
	);
	if(cube_addr_dec_depth > complete_decode_tsv)
	{
		cube_addr_dec_depth=complete_decode_tsv;
	}
	//Read the number of Address TSVs based on the pre-decoding
	uint64_t pre_counter=0;
	for(pre_counter=0; pre_counter< cube_addr_dec_depth ; pre_counter++){
		total_addr_tsv=total_addr_tsv/2;
	}
	total_addr_tsv=total_addr_tsv+pre_counter;

	*/

	if(cube_model_enable==1) //Horizontal Channels
	{
		//The total TSVs per bank will be equal to this number divided by number of banks + ecc per bank + data burst TSVs+ ecc for data + redundant tsv
		total_tsv = (total_addr_tsv+cube_ecc_tsv+cube_redun_tsv+cube_data_tsv)*chips;

		tsv_bitmap= new bool[(total_addr_tsv+cube_ecc_tsv+cube_redun_tsv+cube_data_tsv)*chips]();
		tsv_info= new uint64_t[(total_addr_tsv+cube_ecc_tsv+cube_redun_tsv+cube_data_tsv)*chips]();
		tsv_shared_accross_chips=false;
	}
	else{	//Vertical Channels
		total_tsv = (total_addr_tsv+cube_redun_tsv)*chips+(cube_ecc_tsv+cube_data_tsv)*banks;
		tsv_bitmap= new bool[(total_addr_tsv+cube_redun_tsv)*chips+(cube_ecc_tsv+cube_data_tsv)*banks]();
		tsv_info= new uint64_t[(total_addr_tsv+cube_redun_tsv)*chips+(cube_ecc_tsv+cube_data_tsv)*banks]();
		tsv_shared_accross_chips=true;
	}
	/**************************************************/
	struct timeval tv;
	gettimeofday (&tv, NULL);
	gen.engine().seed(tv.tv_sec * 1000000 + (tv.tv_usec));

	if( settings.verbose )
	{
		cout << "# -------------------------------------------------------------------\n";
		cout << "# GroupDomain_cube(" << m_name << ")\n";
		cout << "# cube_addr_dec_depth " << cube_addr_dec_depth << "\n";
		cout << "# enable_tsv " << enable_tsv << "\n";
		cout << "# chips " << chips << "\n";
		cout << "# banks " << banks << "\n";
		cout << "# burst_size " << burst_size << "\n";
		cout << "# cube_ecc_tsv " << cube_ecc_tsv << "\n";
		cout << "# cube_redun_tsv " << cube_redun_tsv << "\n";
		cout << "# cube_data_tsv " << cube_data_tsv << "\n";
		cout << "# total_addr_tsv " << total_addr_tsv << "\n";
		cout << "# total_tsv " << total_tsv << "\n";
		cout << "# cube_addr_dec_depth " << cube_addr_dec_depth << "\n";
		cout << "# -------------------------------------------------------------------\n";
	}
}

int GroupDomain_cube::update( uint test_mode_t )
{
	int newfault = 0;
	uint64_t location=0;
	//Check if TSVs are enabled
	if(enable_tsv)
	{	
		// determine whether any faults happened.
		// if so, record them.
		double random = gen();
		if( random <= tsv_transientFIT) {
			// only record un-correctable faults for overall simulation success determination
			tsv_n_faults_transientFIT_class++;
			newfault = 1;
			//Record the fault and update the info for TSV
			location = eng()%total_tsv;
			if(tsv_bitmap[location]==false)
			{
				tsv_bitmap[location]=true;
				tsv_info[location]=1;
			}
		}
		random = gen();
		if( random <= tsv_permanentFIT) {
			// only record un-correctable faults for overall simulation success determination
			tsv_n_faults_permanentFIT_class++;
			newfault = 1;
			//Record the fault in a tsv and update its info
			location = eng()%total_tsv;
			if(tsv_bitmap[location]==false)
			{
				tsv_bitmap[location]=true;
				tsv_info[location]=2;
			}
		}
	}
	FaultDomain::update(test_mode_t);
	
	return newfault;
}

void GroupDomain_cube::setFIT( int faultClass, bool isTransient, double FIT )
{
	assert(0);
}
void GroupDomain_cube::setFIT_TSV(bool tsv_isTransient, double FIT_TSV )
{
	FaultDomain::setFIT_TSV(tsv_isTransient, FIT_TSV );
}
void GroupDomain_cube::init( uint64_t interval, uint64_t max_s, double fit_factor )
{
	m_interval = interval;
	m_sim_seconds = max_s;
	m_fit_factor = fit_factor;

	double sec_per_hour = 60 * 60;
	double interval_factor = (interval / sec_per_hour) / 1000000000.0;
	tsv_transientFIT = (double)1.0 - exp( -tsv_transientFIT * fit_factor * interval_factor );
	tsv_permanentFIT = (double)1.0 - exp( -tsv_permanentFIT * fit_factor * interval_factor );
	assert( tsv_transientFIT >= 0 );
	assert( tsv_transientFIT <= 1 );
	assert( tsv_permanentFIT >= 0 );
	assert( tsv_permanentFIT <= 1 );

	FaultDomain::init( interval, max_s, fit_factor );
}

void GroupDomain_cube::generateRanges( int faultClass )
{

}
