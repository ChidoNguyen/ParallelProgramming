/*Chido Nguyen
931506965
nguychid@oregonstate.edu
Project 5: Analyzing SIMD vs Non-SIMD performances.
*/

#include <iostream>
#include <omp.h>
#include <time.h>
#include "simd.p5.h"
using namespace std;
#define NUMTRIES 10

float a[ARR],b[ARR],c[ARR];

/*Non simd approach, linear multiplying in a sense */
void NonSimdMul(float *a, float *b, float *c, int len){
	
	for(int i = 0 ; i < len ; i++)
		c[i]  = a[i]*b[i];

}

/*non simd approach, multiplying_sum */
float NonSimdMulSum(float* a, float* b, int len){
	float tmp_sum =0;
	for(int i = 0; i < len ; i++){
		tmp_sum+=a[i]+b[i];
	}
	return tmp_sum;
}

int main(int argc, char* argv[]){
	#ifndef _OPENMP
		cerr << "OpenMP not found." <<endl;
	#endif
	srand(time(0));
	double tmp_size = ARR;
	float peak = 0;
	float perf = 0;
	float avg = 0;
	//loop//
	int j = 0;
	//4 runs avg//
	
	//SIMD multiply//
	for(j = 0; j < NUMTRIES; j++){
		for(int i= 0 ; i < ARR; i++){
			a[i]=rand();
			b[i]=rand();
		}
		double time0 = omp_get_wtime();
		SimdMul(a,b,c,(int)ARR);
		double time1 = omp_get_wtime();
		perf = (float)ARR /(time1-time0) / 1000000.;
		if(perf > peak)
			peak = perf;
		avg += perf;
	}
	avg/=NUMTRIES;
	cout <<"SIMD MUL Peak then Avg" << endl; 
	cout << peak << endl;
	cout << avg << endl;
	//NonSimd Mul
	peak = 0;
	perf = 0;
	avg = 0;
	for(j = 0; j < NUMTRIES; j++){
		for(int i= 0 ; i < ARR; i++){
			a[i]=rand();
			b[i]=rand();
		}
		double time0 = omp_get_wtime();
		NonSimdMul(a,b,c,(int)ARR);
		double time1 = omp_get_wtime();

		perf = (float)ARR /(time1-time0 )/ 1000000.;
		if(perf > peak)
			peak = perf;
		avg += perf;
	}
	avg/=NUMTRIES;
	cout <<"NON_SIMD MUL Peak then Avg" << endl; 
	cout << peak << endl;
	cout << avg << endl;
	//SIMD mul sum//
	peak = 0;
	perf = 0;
	avg = 0;
	for(j = 0; j < NUMTRIES; j++){
		for(int i= 0 ; i < ARR; i++){
			a[i]=rand();
			b[i]=rand();
		}
		double time0 = omp_get_wtime();
		float tmp = SimdMulSum(a,b,(int)ARR);
		double time1 = omp_get_wtime();

		perf = (float)ARR /(time1-time0 )/ 1000000.;
		if(perf > peak)
			peak = perf;
		avg += perf;
	}
	avg/=NUMTRIES;
	cout <<"SIMD MUL_SUM Peak then Avg" << endl; 
	cout << peak << endl;
	cout << avg << endl;
	
	// NON simd mul sum//
	peak = 0;
	perf = 0;
	avg = 0;
	for(j = 0; j < NUMTRIES; j++){
		for(int i= 0 ; i < ARR; i++){
			a[i]=rand();
			b[i]=rand();
		}
		double time0 = omp_get_wtime();
		float tmp = NonSimdMulSum(a,b,(int)ARR);
		double time1 = omp_get_wtime();

		perf = (float)ARR /(time1-time0 )/ 1000000.;
		if(perf > peak)
			peak = perf;
		avg += perf;
	}
	avg/=NUMTRIES;
	cout <<"NON_SIMD MUL_SUM Peak then Avg" << endl; 
	cout << peak << endl;
	cout << avg << endl;
	
	return 0;
}