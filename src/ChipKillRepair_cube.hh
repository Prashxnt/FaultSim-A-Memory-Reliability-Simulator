/*
Copyright (c) 2015, Advanced Micro Devices, Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef CHIPKILLREPAIR_CUBE_HH_
#define CHIPKILLREPAIR_CUBE_HH_

#include "RepairScheme.hh"

class ChipKillRepair_cube : public RepairScheme
{
public:
	ChipKillRepair_cube( string name, int n_sym_correct, int n_sym_detect , FaultDomain *fd);

	void repair( FaultDomain *fd, uint64_t &n_undetectable, uint64_t &n_uncorrectable );
	uint64_t fill_repl ( FaultDomain *fd );
	void printStats( void );
	void resetStats( void );
	void clear_counters( void );
	void repair_hc(FaultDomain *fd, uint64_t &n_undetect, uint64_t &n_uncorrect);
	void repair_vc(FaultDomain *fd, uint64_t &n_undetect, uint64_t &n_uncorrect);
	int64_t getbank_number(FaultRange fr_number);
private:
	uint64_t m_n_correct, m_n_detect;
	uint64_t counter_prev, counter_now;
	uint32_t logBits, logCols, logRows, banks;
};


#endif /* CHIPKILLREPAIR_CUBE_HH_ */
