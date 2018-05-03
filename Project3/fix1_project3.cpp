/* Chido Nguyen
Project 3: False Sharing 
931506965
*/

#include <iostream>
#include <omp.h>
#include <math.h>
#include <stdlib.h>
#include <climits>

using namespace std;



struct s{
	float value;
	int pad[NUMPAD];
}Array[NUMT]; // change 4 if we want to do more than 4 coures


int main(int argc, char* argv[]){
	#ifndef _OPENMP
		cout << "OpenMP not available!" << endl;
		return 1;
	#endif
	
	//int NUMT = 4;
	int BIGNUMBER = 1500000000;

	/* if(argc > 1){
		NUMT = atoi(argv[1]);
	} */

	omp_set_num_threads(NUMT);

	double time0 = omp_get_wtime();
	#pragma omp parallel for
	for (int i = 0; i < 4; ++i)		// CHANGE 4 out as needed to match ARRAY[X]
	{
		for (int j = 0; j < BIGNUMBER; ++j)
		{
			Array[i].value += 2;
		}
	}
	double time1 = omp_get_wtime();
	double exec_time = (time1-time0);
	float performance = BIGNUMBER / exec_time / 1000000;
	cout << performance << endl;
	return 0;
}
