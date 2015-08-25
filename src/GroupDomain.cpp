/*
 * FAULTSIM: A Fast, Configurable Memory Resilience Simulator
 * (c) 2013-2015 Advanced Micro Devices, Inc.
 *
 */

#include "GroupDomain.hh"
#include <iostream>
#include <stdlib.h>

GroupDomain::GroupDomain( const char *name ) : FaultDomain( name )
{
	tsv_transientFIT=0;
	tsv_permanentFIT=0;
	tsv_n_faults_transientFIT_class=0;
	tsv_n_faults_permanentFIT_class=0;
}

