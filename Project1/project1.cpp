/*Chido Nguyen
nguychid@oregonstate.edu // 931506965
Project 1: OpenMP Numeric Integration
Approach: sum(TILES * Heights)
Tiles: Will be dependent on num of nodes ( area of tile).
Similar to calculus smaller the tiles the more accurate the answer etc.
Command Line Argument: ./Program #of threads #ofnodes
*/

#include <iostream>
#include <stdlib.h> // atoi 
#include <omp.h>
#include <math.h>
#include <iomanip> // setw setprecision
using std::cout;
using std::endl;
#define NUMTRIES 6
int NUMT;
int NUMNODES;

//2D restriction of X Y plane
#define XMIN	 0.
#define XMAX	 3.
#define YMIN	 0.
#define YMAX	 3.
//Z plane coordinates
#define TOPZ00  0.
#define TOPZ10  1.
#define TOPZ20  0.
#define TOPZ30  0.

#define TOPZ01  1.
#define TOPZ11  6.
#define TOPZ21  1.
#define TOPZ31  0.

#define TOPZ02  0.
#define TOPZ12  1.
#define TOPZ22  0.
#define TOPZ32  4.

#define TOPZ03  3.
#define TOPZ13  2.
#define TOPZ23  3.
#define TOPZ33  3.

#define BOTZ00  0.
#define BOTZ10  -3.
#define BOTZ20  0.
#define BOTZ30  0.

#define BOTZ01  -2.
#define BOTZ11  10.
#define BOTZ21  -2.
#define BOTZ31  0.

#define BOTZ02  0.
#define BOTZ12  -5.
#define BOTZ22  0.
#define BOTZ32  -6.

#define BOTZ03  -3.
#define BOTZ13   2.
#define BOTZ23  -8.
#define BOTZ33  -3.
//////////////////////////////////////
double Height( int iu, int iv )	// iu,iv = 0 .. NUMNODES-1
{
	double u = (double)iu / (double)(NUMNODES-1);
	double v = (double)iv / (double)(NUMNODES-1);

	// the basis functions:

	double bu0 = (1.-u) * (1.-u) * (1.-u);
	double bu1 = 3. * u * (1.-u) * (1.-u);
	double bu2 = 3. * u * u * (1.-u);
	double bu3 = u * u * u;

	double bv0 = (1.-v) * (1.-v) * (1.-v);
	double bv1 = 3. * v * (1.-v) * (1.-v);
	double bv2 = 3. * v * v * (1.-v);
	double bv3 = v * v * v;

	// finally, we get to compute something:


        double top =       bu0 * ( bv0*TOPZ00 + bv1*TOPZ01 + bv2*TOPZ02 + bv3*TOPZ03 )
                        + bu1 * ( bv0*TOPZ10 + bv1*TOPZ11 + bv2*TOPZ12 + bv3*TOPZ13 )
                        + bu2 * ( bv0*TOPZ20 + bv1*TOPZ21 + bv2*TOPZ22 + bv3*TOPZ23 )
                        + bu3 * ( bv0*TOPZ30 + bv1*TOPZ31 + bv2*TOPZ32 + bv3*TOPZ33 );

        double bot =       bu0 * ( bv0*BOTZ00 + bv1*BOTZ01 + bv2*BOTZ02 + bv3*BOTZ03 )
                        + bu1 * ( bv0*BOTZ10 + bv1*BOTZ11 + bv2*BOTZ12 + bv3*BOTZ13 )
                        + bu2 * ( bv0*BOTZ20 + bv1*BOTZ21 + bv2*BOTZ22 + bv3*BOTZ23 )
                        + bu3 * ( bv0*BOTZ30 + bv1*BOTZ31 + bv2*BOTZ32 + bv3*BOTZ33 );

        return top - bot;	// if the bottom surface sticks out above the top surface
				// then that contribution to the overall volume is negative
}
/*
nodeLocation: Tells us full tile / .25 / or .5 tile based on location
We know that all corners will have quarter 
All edges .5 
All else is full
Input: Node Location 2 int values
Return: 1.0 , .25, or .5 based on location
Algorithm: we can pinpoint the 4 corner coordinates (0,X) (X,0), (0,0), (X,X)
All edges will have 0 or X(NUMNODE) in one of the coordinate places
*/
double nodeLocation(int iu, int iv){
	if(iu == 0 || iu == (NUMNODES - 1 )){
		if(iv == 0 || iv == (NUMNODES -1)){
			return .25;
		}
		else
			return .5;
	}
	else if(iv == 0 || iv == (NUMNODES -1))
		return .5;
	else 
		return 1.;

}

int main(int argc, char *argv[]){
	#ifndef _OPENMP
		cerr << "OpenMP Not Supported" << endl;
		return 1;
	#endif
	double peakPerformance = 0 ;
	double avgPerformance = 0 ;
	double sumTries = 0;
	double areaTotal = 0.;
	// Load in our thread # and node # from command line //
	if(argc == 3 ){
		NUMT = atoi(argv[1]);
		NUMNODES = atoi(argv[2]);
	}
	else {
		cout << "Argument Count Error" << endl;
		return 1;
	}
	///////////////////////////////////////////////////////
	
	omp_set_num_threads( NUMT); // sets number of threads to use //
	
	double fullTileArea = ((XMAX - XMIN) / (double)(NUMNODES-1)) * ((YMAX - YMIN) / (double)(NUMNODES-1));
	for(int x = 0 ; x < NUMTRIES ; x++){
		double time_0 = omp_get_wtime();
		
		#pragma omp parallel for default(none), shared(NUMNODES,fullTileArea) ,reduction(+:areaTotal)
		for(int i = 0; i < NUMNODES*NUMNODES; i++){
			int iu = i % NUMNODES;
			int iv = i / NUMNODES;
			double TMP_AREA = fullTileArea * nodeLocation(iu,iv) * Height(iu,iv);
			areaTotal += TMP_AREA;
		}
		double time_1 = omp_get_wtime();
		double CalcSpeed = (NUMNODES*NUMNODES) / (time_1 - time_0) / 1000000;
		sumTries += CalcSpeed; 
		if(CalcSpeed > peakPerformance){
			peakPerformance = CalcSpeed;
		}
	}
	avgPerformance = sumTries / (double) NUMTRIES;
	areaTotal = areaTotal / 6.;
	cout << std::setw(8) << std::setprecision(8) << std::fixed<<"Thread(s): " << argv[1] << "	Number of Nodes: " << argv[2] << "	Average: " << avgPerformance << "	Peak: " << peakPerformance << "	Attempts: "<< NUMTRIES << "	Area: " << areaTotal << endl;
	
	return 0;
}