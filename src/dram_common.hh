/*
 * FAULTSIM: A Fast, Configurable Memory Resilience Simulator
 * (c) 2013-2015 Advanced Micro Devices, Inc.
 *
 */

#ifndef DRAM_COMMON_HH_
#define DRAM_COMMON_HH_

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/variate_generator.hpp>
#include <ctime>
#include <sys/time.h>

using namespace std;

#define DRAM_1BIT 0
#define DRAM_1WORD 1
#define DRAM_1COL 2
#define DRAM_1ROW 3
#define DRAM_1BANK 4
#define DRAM_NBANK 5
#define DRAM_NRANK 6
#define DRAM_MAX 7

// 64-bit random doubles for determining if failure happened
typedef boost::mt19937_64                     ENG;    // Mersenne Twister
typedef boost::random::uniform_real_distribution<double> DIST;
typedef boost::random::variate_generator<ENG,DIST> GEN;    // Variate generator

#endif /* DRAM_COMMON_HH_ */
