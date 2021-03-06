// Project0.cpp : Chido Nguyen; 931506965; CS475
// Basic Intro to openMP usages/setups
//

#include <iostream>
#include <omp.h>
#include <math.h>
#include <iomanip>
using std::cout;
using std::endl;

#define NUMT1		1 // Added 2 NUMT to run both 1 and 4 thread data
#define NUMT4		4 // # of threads
#define ARRAYSIZE	65536
#define NUMTRIES	50

float A[ARRAYSIZE];
float B[ARRAYSIZE];
float C[ARRAYSIZE];

int main()
{
#ifndef _OPENMP
	std::cout << "OpenMP not supported" << std::endl;
	return 1;
#endif
	//			ONE THREAD			//
	omp_set_num_threads(NUMT1);
	std::cerr << "Using " << NUMT1 << " threads" << endl;

	double maxMegaMults1 = 0.;
	double sumMegaMults1 = 0.;

	for (int x = 0; x < NUMTRIES; x++) {
		double time0 = omp_get_wtime();

		#pragma omp parallel for
		for (int y = 0; y < ARRAYSIZE; y++) {
			C[y] = A[y] * B[y];
		}

		double time1 = omp_get_wtime();
		double megaMults1 = (double)ARRAYSIZE / (time1 - time0) / 1000000;
		sumMegaMults1 += megaMults1;
		if (megaMults1 > maxMegaMults1)
			maxMegaMults1 = megaMults1;
	}

	double avgMegaMults1 = sumMegaMults1 / (double)NUMTRIES;
	cout << std::fixed << std::setw(8) << std::setprecision(2) << "Peak Performance = " << maxMegaMults1 << " MegaMults/Sec" << endl;
	cout << std::fixed << std::setw(8) << std::setprecision(2) << "Average Performance = " << avgMegaMults1 << " MegaMults/Sec" << endl;
	
	//			FOUR THREADS			//
	omp_set_num_threads(NUMT4);
	std::cerr << "Using " << NUMT4 << " threads" << endl;

	double maxMegaMults4 = 0.;
	double sumMegaMults4 = 0.;

	for (int x = 0; x < NUMTRIES; x++) {
		double time4_0 = omp_get_wtime();

		#pragma omp parallel for
		for (int y = 0; y < ARRAYSIZE; y++) {
			C[y] = A[y] * B[y];
		}

		double time4_1 = omp_get_wtime();
		double megaMults4 = (double)ARRAYSIZE / (time4_1 - time4_0) / 1000000;
		sumMegaMults4 += megaMults4;
		if (megaMults4 > maxMegaMults4)
			maxMegaMults4 = megaMults4;
	}

	double avgMegaMults4 = sumMegaMults4 / (double)NUMTRIES;
	cout << std::fixed << std::setw(8) << std::setprecision(2) << "Peak Performance = " << maxMegaMults4 << " MegaMults/Sec" << endl;
	cout << std::fixed << std::setw(8) << std::setprecision(2) << "Average Performance = " << avgMegaMults4 << " MegaMults/Sec" << endl;
	
	//Calculations//
	
	// Performance = size / time / 1000000 or TIME = ARRAYSIZE / (performance *1,000,000) //
	double time_exec_1 = (double) ARRAYSIZE / (avgMegaMults1 * 1000000);
	double time_exect_4 = (double) ARRAYSIZE / (avgMegaMults4 * 1000000);
	double Speed_Up = time_exec_1 / time_exect_4;
	
	cout << std::fixed << std::setw(8) << std::setprecision(4) << "Speedup from 1 to 4 is about " << Speed_Up << endl;
	
	// Parallel Fraction (4/3) * ( 1- 1/S) //
	
	double Fp = (4./3.) * (1. - (1./Speed_Up));
	
	cout << std::fixed << std::setw(8) << std::setprecision(4) << "Parallel fraction is : " << Fp << endl;
	
	return 0;
}

