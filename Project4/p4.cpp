/*Chido Nguyen
nguychid@oregonstate.edu
931506965
Project 4: Parallel for convenience 
*/
#include <iostream>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
using namespace std;
//GLOBAL VARIABLES//
//System States//
int		NowYear;		//2017-2022
int		NowMonth;		//0-11

float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight;		// grain height in inches
int		NowNumDeer;		// number of deer in current population 

int		REDWEDDING;			// My Agent
unsigned int seed = 0;
//Constants Variables//
//grain grown = inches//
//temperature are in F//
// precip are in inches //
const float GRAIN_GROWS_PER_MONTH =			8.0;
const float ONE_DEER_EATS_PER_MONTH =		0.5;

const float AVG_PRECIP_PER_MONTH =			6.0;	// average
const float AMP_PRECIP_PER_MONTH =			6.0;	// plus or minus
const float RANDOM_PRECIP =					2.0;	// plus or minus noise

const float AVG_TEMP =						50.0;	// average
const float AMP_TEMP =						20.0;	// plus or minus
const float RANDOM_TEMP =					10.0;	// plus or minus noise

const float MIDTEMP =						40.0;
const float MIDPRECIP =						10.0;

//////////////////////////////////////////////////////
//Randomizers//
float Ranf( unsigned int *seedp, float low, float high){
	float r  = (float)rand_r(seedp);
	return (low + r*(high-low)/ float(RAND_MAX));
}

int Ranf(  unsigned int *seedp, int ilow, int ihigh){
	float low = (float) ilow;
	float high = (float) ihigh;
	return (int)(Ranf(seedp,low,high));
}

float SQR(float x){
	return x*x;
}

//setup_tmp_precip will calculate the NowPrcip and NowTemp for the current month//

void setup_tmp_precip(){
	float ang = (  30.*(float)NowMonth + 15.  ) * ( M_PI / 180. );
	float temp = AVG_TEMP - AMP_TEMP * cos( ang );
	//unsigned int seed = 0;
	NowTemp = temp + Ranf( &seed, -RANDOM_TEMP, RANDOM_TEMP );

	float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin( ang );
	NowPrecip = precip + Ranf( &seed,  -RANDOM_PRECIP, RANDOM_PRECIP );
	if( NowPrecip < 0. )
		NowPrecip = 0.;
}

//GrainDeer calculates sustainability of Deer and grain height//
//Capacity : Depends on number of inches in grain height. Height = Deer nothing happens; Height > Deer  +1 population ; Height < Deer - 1 population
void GrainDeer(){
	while(NowYear < 2023){
		//Calc//
		int tmp_GD;
		if(NowNumDeer == NowHeight)
			tmp_GD = NowNumDeer;
		else if(NowNumDeer > NowHeight)
			tmp_GD = NowNumDeer -1;
		else// (NowNumDeer < NowHeight)
			tmp_GD = NowNumDeer + 1;
		
		#pragma omp barrier
		
		//update//
		NowNumDeer = tmp_GD;
		
		#pragma omp barrier
		//red wedding
		#pragma omp barrier
		//watch() running
		
		#pragma omp barrier
	}
}

//Grain: will calculate if grain grew in height or decreased based on weather and being eaten by Deers
/* my logic here is that we want to do this :
 NowHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
 NowHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;
 But we dont want to touch global variables yet so I consider it a simple
 x+=y and x-=z or x = x + y and x =  x - z; do some math we get
 x = x +y - z; or x += (y-z) y - z being our growth factor and eat factor
 */
void Grain(){
	//Calc//
	while(NowYear < 2023){
		float tempFactor = exp( -SQR( (NowTemp - MIDTEMP)/10.));
		float precipFactor = exp( -SQR( (NowPrecip - MIDPRECIP)/10.));
		float growth_factor = tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
		float eat_factor = (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;

		float tmp = growth_factor - eat_factor;
		#pragma omp barrier

		NowHeight += tmp;
		if(NowHeight < 0)
			NowHeight = 0;

		#pragma omp barrier
		//red wedding
		#pragma omp barrier
		//Watcher

		#pragma omp barrier
	}
	
	
}

// Watcher: Prints info such as year/ month ; tmp; precip; heigh;deer
void Watcher(){
	while(NowYear < 2023){
		//calc//
		#pragma omp barrier
		//update//
		#pragma omp barrier
		//red wedding
		#pragma omp barrier
		cout<< NowYear << "	" << (NowMonth+1) << endl;
		cout << "Temperature: \n" << NowTemp << endl;
		cout << "Precip: \n" << NowPrecip << endl;
		cout << "Grain Height: \n" << NowHeight << endl;
		cout << "Deer Count: \n" << NowNumDeer << endl;
		cout << "Red Wedding: \n" << REDWEDDING << endl;
		//Update stuff//
		if(NowMonth < 11)
			NowMonth++;
		else{
			NowMonth =0;
			NowYear++;
		}
		setup_tmp_precip();
		#pragma omp barrier
	}
}

// MY OWN LITTLE INTERACTION //
// GRRM style //
// if even i.e. modulos 0 then Deers get lucky and dont get slaughtered to make into pies and double in population while Grain is reduced by 1/4.
// Else unlucky odd deers halved and grain grow by factor of 2//
void RED_WEDDING(){
	while(NowYear < 2023){
		//Calc//
		int tmp = rand_r(&seed);
		tmp %=2;
		#pragma omp barrier
		//Update//
		#pragma omp barrier
		REDWEDDING = tmp;
		if(tmp == 0){
			NowNumDeer *=2;
			NowHeight *= .25;
		}
		else{
			NowNumDeer /=2;
			NowHeight *=2;
		}
		#pragma omp barrier
		
		//Watcher
		#pragma omp barrier
	}
}

int main(int argc, char* argv[]){
	//Starting values//
	NowMonth = 0;
	NowYear = 2017;
	NowNumDeer = 1;
	NowHeight = 1.;
	//////////////////////////

	setup_tmp_precip(); // initial setup_tmp_precip
	
	omp_set_num_threads(4); // change to 4 later
	#pragma omp parallel sections private(seed)
	{
		#pragma omp section
		{
			GrainDeer();

		}
		#pragma omp section
		{
			Grain();
		}
		#pragma omp section
		{
			Watcher();
		}
		 #pragma omp section
		{
			RED_WEDDING();	// your own
		} 
	}
	return 0;
}