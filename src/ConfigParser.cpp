/*
 * FAULTSIM: A Fast, Configurable Memory Resilience Simulator
 * (c) 2013-2015 Advanced Micro Devices, Inc.
 *
 */

#include<iostream>
extern struct Settings settings;

#include "ConfigParser.hh"
#include <stdlib.h>
#include <stdio.h>
//#include "roxml.h"
#include <string.h>
#include "Settings.hh"
#include <stdint.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

void parser(char *ininame)
{
	boost::property_tree::ptree pt;
	boost::property_tree::ini_parser::read_ini( ininame, pt );

	settings.sim_mode = pt.get<int>("Sim.sim_mode");
	settings.interval_s = pt.get<uint64_t>("Sim.interval_s");
	settings.scrub_s = pt.get<uint64_t>("Sim.scrub_s");
	settings.max_s = pt.get<uint64_t>("Sim.max_s");
	settings.n_sims = pt.get<uint64_t>("Sim.n_sims");
	settings.continue_running = pt.get<bool>("Sim.continue_running");
	settings.verbose = pt.get<int>("Sim.verbose");
	settings.debug = pt.get<int>("Sim.debug");
	settings.output_bucket_s = pt.get<uint64_t>("Sim.output_bucket_s");

	settings.organization = pt.get<int>("Org.organization");
	settings.chips_per_rank = pt.get<int>("Org.chips_per_rank");
	settings.chip_bus_bits = pt.get<int>("Org.chip_bus_bits");
	settings.ranks = pt.get<int>("Org.ranks");
	settings.banks = pt.get<int>("Org.banks");
	settings.rows = pt.get<int>("Org.rows");
	settings.cols = pt.get<int>("Org.cols");
	settings.cube_model = pt.get<int>("Org.cube_model");
	settings.cube_addr_dec_depth = pt.get<int>("Org.cube_addr_dec_depth");
	settings.cube_ecc_tsv = pt.get<int>("Org.cube_ecc_tsv");
	settings.cube_redun_tsv = pt.get<int>("Org.cube_redun_tsv");
	settings.data_block_bits = pt.get<int>("Org.data_block_bits");

	settings.faultmode = pt.get<int>("Fault.faultmode");
	settings.enable_permanent = pt.get<int>("Fault.enable_permanent");
	settings.enable_transient = pt.get<int>("Fault.enable_transient");
	settings.enable_tsv = pt.get<int>("Fault.enable_tsv");
	settings.fit_factor = pt.get<double>("Fault.fit_factor");
	settings.tsv_fit = pt.get<double>("Fault.tsv_fit");

	settings.repairmode = pt.get<int>("ECC.repairmode");
}
