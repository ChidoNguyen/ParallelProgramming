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
	//int pad[NUMPAD];
}Array[NUMT]; // change 4 if we want to do more than 4 coures


int main(int argc, char* argv[]){
	#ifndef _OPENMP
		cout << "OpenMP not available!" << endl;
		return 1;
	#endif
	

	int BIGNUMBER = 1500000000;

	omp_set_num_threads(NUMT);

	double time0 = omp_get_wtime();
	float temp;
	#pragma omp parallel for default(none),shared(Array,BIGNUMBER),private(temp)
	for (int i = 0; i < 4; ++i)		// CHANGE 4 out as needed to match ARRAY[X]
	{
		temp = Array[i].value;
		for (int j = 0; j < BIGNUMBER; ++j)
		{
			temp += 2;
		}
		Array[i].value = temp;
	}
	double time1 = omp_get_wtime();
	double exec_time = (time1-time0);
	float performance = BIGNUMBER / exec_time / 1000000;
	cout << performance << endl;
	return 0;
}
