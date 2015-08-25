/*
 * FAULTSIM: A Fast, Configurable Memory Resilience Simulator
 * (c) 2013-2015 Advanced Micro Devices, Inc.
 *
 */

#include "RepairScheme.hh"

RepairScheme::RepairScheme( string name )
{
	m_name = name;

	resetStats();
}

uint64_t RepairScheme::fill_repl (FaultDomain *fd)
{
	return 1;
}
void RepairScheme::printStats( void )
{
}

void RepairScheme::resetStats( void )
{
}

string RepairScheme::getName( void )
{
	return m_name;
}
