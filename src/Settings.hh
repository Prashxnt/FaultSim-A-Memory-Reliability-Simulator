/*
Copyright (c) 2015, Advanced Micro Devices, Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

class Settings
{
public:
	// Simulator settings
	int sim_mode;		 // Interval or Event based
	std::string output_file;      // Output results file
	uint64_t interval_s; // Simulation interval (seconds)
	uint64_t scrub_s;    // Scrubbing interval (seconds)
	uint64_t max_s;      // Simulation total duration (seconds)
	uint64_t n_sims;    // Number of simulations to run total
	bool continue_running; // Continue simulations after the first uncorrectable error
	uint test_mode;			// TODO document
	int verbose;			// Enable or disable runtime output
	bool debug; 			// TODO document
	uint64_t output_bucket_s; // Seconds per output histogram bucket

	// Memory system physical configuration
	int organization;	// Which topology to simulate e.g. DIMM or 3D stack
	// Settings for all DRAMs
	uint chips_per_rank, chip_bus_bits, ranks, banks, rows, cols;

	// Settings for 3D stacks
	uint cube_model;				// TODO document
	uint64_t cube_addr_dec_depth;	// TODO document
	uint64_t cube_ecc_tsv;			// TODO document
	uint64_t cube_redun_tsv;		// TODO document
	uint64_t data_block_bits; 		// Symbol size (in Bits) for RAID-like parity

	// Fault models
	int faultmode;      	// Fault injection model (uniform random bit errors, Jaguar FIT rates etc.)
	double fit_factor;  	// Base FIT rate scaling factor for memory arrays
	double tsv_fit;     	// FIT rate for TSVs
	bool enable_tsv;		// Enable TSV fault injection
	bool enable_transient;	// Enable transient fault injection
	bool enable_permanent;	// Enable permanent fault injection

	// ECC configuration
	int repairmode;     // Type of ECC to apply
};
