/*
 * FAULTSIM: A Fast, Configurable Memory Resilience Simulator
 * (c) 2013-2015 Advanced Micro Devices, Inc.
 *
 */

#include "ChipKillRepair.hh"
#include "DRAMDomain.hh"

ChipKillRepair::ChipKillRepair( string name, int n_sym_correct, int n_sym_detect ) : RepairScheme( name )
, m_n_correct(n_sym_correct)
, m_n_detect(n_sym_detect)
{
counter_prev=0;
counter_now=0;
}

void ChipKillRepair::repair( FaultDomain *fd, uint64_t &n_undetectable, uint64_t &n_uncorrectable )
{
	n_undetectable = n_uncorrectable = 0;
	// Repair this module.  Assume 8-bit symbols.

	list<FaultDomain*> *pChips = fd->getChildren();
	// make sure number of children is appropriate for level of ChipKill
	// i.e. 18 chips per chipkill
	assert( pChips->size() == (m_n_correct * 18) );
	list<FaultDomain*>::iterator it0, it1;
	
	//Clear out the touched values for all chips
	for( it1 = pChips->begin(); it1 != pChips->end(); it1++ )
	{
		DRAMDomain *pDRAM3 = dynamic_cast<DRAMDomain*>((*it1));
		list<FaultRange*> *pRange3 = pDRAM3->getRanges();
		list<FaultRange*>::iterator itRange3;
		for( itRange3 = pRange3->begin(); itRange3 != pRange3->end(); itRange3++ )
		{
			FaultRange *fr2 = (*itRange3);
			fr2->touched=0;
		}
	}
	uint32_t n_intersections;
	// Take each chip in turn.  For every fault range,
	// count the number of intersecting faults (rounded to an 8-bit range).
	// if count exceeds correction ability, fail.
	for( it0 = pChips->begin(); it0 != pChips->end(); it0++ )
	{
		DRAMDomain *pDRAM0 = dynamic_cast<DRAMDomain*>((*it0));
		list<FaultRange*> *pRange0 = pDRAM0->getRanges();

		// For each fault in first chip, query the second chip to see if it has
		// an intersecting fault range.
		list<FaultRange*>::iterator itRange0;
		for( itRange0 = pRange0->begin(); itRange0 != pRange0->end(); itRange0++ )
		{
			// tweak the query range to cover 8-bit block
			// Make a copy, otherwise fault is modified as a side-effect
			FaultRange *frOrg = (*itRange0); // The pointer to the fault location
			FaultRange frTemp = *(*itRange0);
			frTemp.fWildMask |= ((0x1<<3)-1);
		    n_intersections = 0;
			if(frTemp.touched<frTemp.max_faults)
			{
				// for each other chip, count number of intersecting faults
				for( it1 = pChips->begin(); it1 != pChips->end(); it1++ )
				{
                    DRAMDomain *pDRAM1 = dynamic_cast<DRAMDomain*>((*it1));
					list<FaultRange*> *pRange1 = pDRAM1->getRanges();
					list<FaultRange*>::iterator itRange1;
					for( itRange1 = pRange1->begin(); itRange1 != pRange1->end(); itRange1++ )
					{
						FaultRange *fr1 = (*itRange1);
						
                       // if (frTemp.Chip!=fr1->Chip)
                       // {
                          if((frTemp.intersects(fr1)) ) {  //&& (fr1->touched < fr1->max_faults)
					        	// count the intersection
                                n_intersections++;
		        				break;  
                                }
                       // }
                    }//end of Range loop
                } //end of Chip loop
			}
           if(n_intersections <= m_n_correct)
			{
				if(frOrg->fWildMask > m_n_correct)
				frOrg->transient_remove = false;
			}
			if( n_intersections >= m_n_correct) 
			{
				n_uncorrectable = (n_intersections - m_n_correct)+n_uncorrectable;
				frOrg->transient_remove = false;
			}
			if( n_intersections >= m_n_detect) {
				n_undetectable = (n_intersections - m_n_detect)+n_undetectable;
			}
            
		}
	}
//    if(n_intersections>2)
  //  n_undetectable++;
  //  else if (n_intersections==1)
  //  n_uncorrectable++;


}

uint64_t ChipKillRepair::fill_repl(FaultDomain *fd)
{
return 0;
}
void ChipKillRepair::printStats( void )
{
	RepairScheme::printStats();
}
void ChipKillRepair::clear_counters(void)
{
	counter_prev=0;
	counter_now=0;
}

void ChipKillRepair::resetStats( void )
{
	RepairScheme::resetStats();
}
