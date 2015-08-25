/*
 * FAULTSIM: A Fast, Configurable Memory Resilience Simulator
 * (c) 2013-2015 Advanced Micro Devices, Inc.
 *
 */

#include "BCHRepair.hh"
#include "DRAMDomain.hh"

BCHRepair::BCHRepair( string name, int n_correct, int n_detect, uint64_t deviceBitWidth ) : RepairScheme( name )
, m_n_correct(n_correct)
, m_n_detect(n_detect)
, m_bitwidth(deviceBitWidth)
{
counter_prev=0;
counter_now=0;
}

void BCHRepair::repair( FaultDomain *fd, uint64_t &n_undetectable, uint64_t &n_uncorrectable )
{
	n_undetectable = n_uncorrectable = 0;

	// Repair up to N bit faults in a single row
	// Similar to ChipKill except that only 1 bit can be bad across
	// all devices, instead of 1 symbol being bad.
	uint bit_shift=0;
	uint loopcount_locations=0;
	uint ii=0;
	list<FaultDomain*> *pChips = fd->getChildren();
	//assert( pChips->size() == (m_n_repair * 18) );

	list<FaultDomain*>::iterator it0, it1;
	for(it1 =pChips->begin(); it1 !=pChips->end(); it1++)
	{
		DRAMDomain *pDRAM3 = dynamic_cast<DRAMDomain*>((*it1));
		list<FaultRange*> *pRange3 = pDRAM3->getRanges();
		list<FaultRange*>::iterator itRange3;
		for(itRange3 = pRange3->begin(); itRange3 !=pRange3->end(); itRange3++)
		{
			FaultRange *fr3 =(*itRange3);
			fr3->touched=0;
		}
	}
	// Take each chip in turn.  For every fault range, compare with all chips including itself, any intersection of fault range is treated as a fault
	// if count exceeds correction ability, fail.
	for( it0 = pChips->begin(); it0 != pChips->end(); it0++ )
	{
		DRAMDomain *pDRAM0 = dynamic_cast<DRAMDomain*>((*it0));
		list<FaultRange*> *pRange0 = pDRAM0->getRanges();

		// For each fault in first chip, query the second chip to see if it has
		// an intersecting fault range, touched variable tells us about the location being already addressed or not
		list<FaultRange*>::iterator itRange0;
		for( itRange0 = pRange0->begin(); itRange0 != pRange0->end(); itRange0++ )
		{
			FaultRange *frOrg = (*itRange0); // The pointer to the fault location
			FaultRange frTemp = *(*itRange0); //This is a fault location of a chip	
					
			uint32_t n_intersections = 0;
			
			if(frTemp.touched < frTemp.max_faults)
			{
				if(m_n_correct==1) // Depending on the scheme, we will need to group the bits
				{
					bit_shift=2;	//SECDED will give ECC every 8 byte granularity, group by 4 locations in the fault range per chip
				}
				else if(m_n_correct == 3)
				{
					bit_shift=4;	//3EC4ED will give ECC every 32 byte granularity, group by 16 locations in the fault range per chip
				}
				else if (m_n_correct == 6)
				{
					bit_shift=5;	//6EC7ED will give ECC every 64 byte granularity, group by 32 locations in the fault range per chip
				} else {
					assert(0);
				}
				
				//Clear the last few bits to accomodate the address range
				frTemp.fAddr = frTemp.fAddr >> bit_shift;
				frTemp.fAddr = frTemp.fAddr << bit_shift;
				frTemp.fWildMask = frTemp.fWildMask >> bit_shift;
				frTemp.fWildMask = frTemp.fWildMask << bit_shift;
				loopcount_locations = 1 << bit_shift; // This gives me the number of loops for the addresses near the fault range to iterate

				for(ii=0;ii<loopcount_locations;ii++)
				{
					// for each other chip including the current one, count number of intersecting faults
					for( it1 = pChips->begin(); it1 != pChips->end(); it1++ )
					{
						DRAMDomain *pDRAM1 = dynamic_cast<DRAMDomain*>((*it1));
						list<FaultRange*> *pRange1 = pDRAM1->getRanges();
						list<FaultRange*>::iterator itRange1;
						for( itRange1 = pRange1->begin(); itRange1 != pRange1->end(); itRange1++ )
						{
							FaultRange *fr1 = (*itRange1);
							if((frTemp.intersects(fr1)) && (fr1->touched < fr1->max_faults)) {
							// count the intersection
							n_intersections++;
							// immediately move on to the next chip, we don't care about other ranges
							break;
							}
						}
					}
					frTemp.fAddr = frTemp.fAddr + 1;
				}

				if(n_intersections <= m_n_correct)
				{
					// correctable
				}
				if(n_intersections > m_n_correct)
				{
					n_uncorrectable = (n_intersections - m_n_correct)+n_uncorrectable;
					frOrg->transient_remove = false;
					return;
				}
				if(n_intersections > m_n_detect)
				{
					n_undetectable = (n_intersections - m_n_detect)+n_undetectable;
					return;
				}
			}
		}
	}
}
uint64_t BCHRepair::fill_repl(FaultDomain *fd)
{
return 0;
}
void BCHRepair::printStats( void )
{
	RepairScheme::printStats();
}

void BCHRepair::clear_counters(void)
{
	counter_prev=0;
	counter_now=0;
}

void BCHRepair::resetStats( void )
{
	RepairScheme::resetStats();
}


