/*
Copyright (c) 2015, Advanced Micro Devices, Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "CubeRAIDRepair.hh"
#include "DRAMDomain.hh"
#include "Settings.hh"

extern struct Settings settings;

CubeRAIDRepair::CubeRAIDRepair( string name, uint n_sym_correct, uint n_sym_detect, uint data_block_bits ) : RepairScheme( name )
, m_n_correct(n_sym_correct)
, m_n_detect(n_sym_detect)
, m_data_block_bits(data_block_bits)
{
	counter_prev=0;
	counter_now=0;
	m_log_block_bits = log2( m_data_block_bits );
}

void CubeRAIDRepair::repair( FaultDomain *fd, uint64_t &n_undetectable, uint64_t &n_uncorrectable )
{
	n_undetectable = n_uncorrectable = 0;
	// Repair this module.  Assume 8-bit symbols.

	list<FaultDomain*> *pChips = fd->getChildren();
	list<FaultDomain*>::iterator it0, it1;

	//Clear out the touched values for all chips
	for( it1 = pChips->begin(); it1 != pChips->end(); it1++ )
	{
		DRAMDomain *pDRAM1 = dynamic_cast<DRAMDomain*>((*it1));
		list<FaultRange*> *pRange3 = pDRAM1->getRanges();
		list<FaultRange*>::iterator itRange3;
		for( itRange3 = pRange3->begin(); itRange3 != pRange3->end(); itRange3++ )
		{
			FaultRange *fr3 = (*itRange3);
			fr3->touched=0;
		}
	}
	// Take each chip in turn.  For every fault range,
	// count the number of intersecting faults.
	// if count exceeds correction ability, fail.
	for( it0 = pChips->begin(); it0 != pChips->end(); it0++ )
	{

		DRAMDomain *pDRAM0 = dynamic_cast<DRAMDomain*>((*it0));
		list<FaultRange*> *pRange0 = pDRAM0->getRanges();

		// For each fault in first chip, query the other chips to see if they have
		// an intersecting fault range.
		list<FaultRange*>::iterator itRange0;
		for( itRange0 = pRange0->begin(); itRange0 != pRange0->end(); itRange0++ )
		{
			// Make a copy, otherwise fault is modified as a side-effect
			FaultRange *frOrg = (*itRange0); // The pointer to the fault location
			FaultRange frTemp = *(*itRange0);

			// round the FR size to that of a detection block (e.g. cache line)
			frTemp.fWildMask |= ((1 << m_log_block_bits)-1);

			uint32_t n_intersections = 0;
			if(frTemp.touched<frTemp.max_faults)
			{
				// for each other chip, count number of intersecting faults
				//it1 = it0;
				//it1++;
				for( it1 = pChips->begin(); it1 != pChips->end(); it1++ )
				{
					if( it0 == it1 ) continue;	// skip if we're looking at the first chip

					DRAMDomain *pDRAM1 = dynamic_cast<DRAMDomain*>((*it1));
					list<FaultRange*> *pRange1 = pDRAM1->getRanges();
					list<FaultRange*>::iterator itRange1;
					for( itRange1 = pRange1->begin(); itRange1 != pRange1->end(); itRange1++ )
					{
						FaultRange frTemp1 = *(*itRange1);

						// round the FR size to that of a detection block (e.g. cache line)
						frTemp1.fWildMask |= ((1 << m_log_block_bits)-1);

						if(frTemp1.touched < frTemp1.max_faults)
						{	
							if(frTemp.intersects(&frTemp1)) {
								// count the intersection
								n_intersections++;
								break;
							}
						}
					}
				}
			}

			// 1 intersection implies 2 overlapping faults
			if(n_intersections < m_n_correct)
			{
				// correctable
			}
			if( n_intersections >= m_n_correct)
			{
				// uncorrectable fault discovered
				n_uncorrectable += (n_intersections + 1 - m_n_correct);
				frOrg->transient_remove = false;

				if( !settings.continue_running ) return;
			}
			if( n_intersections >= m_n_detect) {
				n_undetectable += (n_intersections + 1 - m_n_detect);
			}
		}
	}
}

uint64_t CubeRAIDRepair::fill_repl(FaultDomain *fd)
{
	return 0;
}
void CubeRAIDRepair::printStats( void )
{
	RepairScheme::printStats();
}
void CubeRAIDRepair::clear_counters(void)
{
	counter_prev=0;
	counter_now=0;
}

void CubeRAIDRepair::resetStats( void )
{
	RepairScheme::resetStats();
}


