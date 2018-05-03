/*
Chido Nguyen
nguychid@oregonstate.edu / 931506965
Project 2: OpenMP: N-Body Problems
Coarse vs Fine, and Static vs Dynamic
*/

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
using std::cout;
using std::endl;

//Constants//

const double G = 6.67300e-11; // units  = m^3 / (kg s^2)
const double EARTH_MASS = 5.9742e24;// kg
const double EARTH_DIAMETER = 12756000.32;   // meters
const double TIMESTEP = 1.0; // secs

#define NUMBODIES	100
#define NUMSTEPS	200

struct body{
	float mass;
	float x,y,z; //position
	float vx,vy,vz;	// velocity
	float fx,fy,fz;	// forces
	float xnew,ynew,znew;
	float vxnew,vynew,vznew;
};

typedef struct body Body;
Body Bodies[NUMBODIES];


//function prototypes//
float GetDistanceSquared(Body *, Body *);
float GetUnitVector(Body*, Body*, float*, float*,float*);
float Ranf(float,float);
int	Ranf(int,int);

//./program thread_count_variable
int main(int argc, char * argv[]){
	#ifndef _OPENMP
		cout << "OpenMP not available!" << endl;
		return 1;
	#endif
	
	omp_set_num_threads(atoi(argv[1])); // command line argument 
	int numProcessors = omp_get_num_procs();
	std::cerr << "Have " << numProcessors << " processors, and Using " << argv[1] << " processor(s)" << endl;
	
	for(int x = 0; x < NUMBODIES ; x++){
		Bodies[x].mass = EARTH_MASS * Ranf(0.5f,10.0f);
		Bodies[x].x = EARTH_DIAMETER * Ranf(-100.0f, 100.0f);
		Bodies[x].y = EARTH_DIAMETER * Ranf(-100.0f, 100.0f);
		Bodies[x].z = EARTH_DIAMETER * Ranf(-100.0f, 100.0f);
		Bodies[x].vx = Ranf(-100.0f, 100.0f);
		Bodies[x].vy = Ranf(-100.0f, 100.0f);
		Bodies[x].vz = Ranf(-100.0f, 100.0f);
	}

	double time0 = omp_get_wtime();
	for(int t = 0; t < NUMSTEPS ; t++){
		//coarse parallelism CHANGE OUT AS NEEDED TO DO TESTS
		//#pragma omp parallel for default(none), schedule(dynamic), shared(Bodies)
		for(int i = 0; i < NUMBODIES ; i++){
			float fx = 0.0;
			float fy = 0.0;
			float fz = 0.0;
			Body *bi = &Bodies[i];
			
			//fine grained CHANGE AS NEEDED TO DO TESTS
			#pragma omp parallel for default(none),schedule(dynamic),shared(Bodies,i,bi),reduction(+:fx,fy,fz)
			for(int j = 0; j < NUMBODIES ; j++){
				if( j == i)
					continue;
				Body *bj = &Bodies[j];
				
				float rsqd = GetDistanceSquared(bi,bj);
				
				if(rsqd > 0.0){
					float f = G * bi->mass * bj->mass / rsqd ;
					float ux,uy,uz;
					GetUnitVector(bi,bj,&ux, &uy, &uz);
					fx += f*ux;
					fy += f*uy;
					fz += f*uz;
				}
			} // end of j
			float ax = fx / Bodies[i].mass;
			float ay = fy / Bodies[i].mass;
			float az = fz / Bodies[i].mass;
			
			Bodies[i].xnew = Bodies[i].x + Bodies[i].vx*TIMESTEP + 0.5*ax*TIMESTEP*TIMESTEP;
			Bodies[i].ynew = Bodies[i].y + Bodies[i].vy*TIMESTEP + 0.5*ay*TIMESTEP*TIMESTEP;
			Bodies[i].znew = Bodies[i].z + Bodies[i].vz*TIMESTEP + 0.5*az*TIMESTEP*TIMESTEP;
			
			Bodies[i].vxnew = Bodies[i].vx + ax*TIMESTEP;
			Bodies[i].vynew = Bodies[i].vy + ay*TIMESTEP;
			Bodies[i].vznew = Bodies[i].vz + az*TIMESTEP;
		} // end of i
		
		//setup up state for next animation//
		
		for(int i = 0; i < NUMBODIES; i++){
			Bodies[i].x = Bodies[i].xnew;
			Bodies[i].y = Bodies[i].ynew;
			Bodies[i].z = Bodies[i].znew;
			Bodies[i].vx = Bodies[i].vxnew;
			Bodies[i].vy = Bodies[i].vynew;
			Bodies[i].vz = Bodies[i].vznew;
		}
	} // end of t 
	double time1 = omp_get_wtime();
	
	double performance = (float)(NUMBODIES * NUMBODIES * NUMSTEPS) / (time1-time0) / (1000000); // mega_astro_movements calculated per second
	//cout << "Performance " << atoi(argv[1]) <<endl 
	cout << performance << endl;
	
	return 0;
}

//functions //
float GetDistanceSquared(Body *bi, Body*bj){
	float dx = bi->x - bj->x;
	float dy = bi->y - bj->y;
	float dz = bi->z - bj->z;
	return dx*dx + dy*dy + dz*dz;
}

float GetUnitVector(Body* from, Body *to, float* ux, float *uy, float* uz){
	float dx = to->x - from->x;
	float dy = to->y - from->y;
	float dz = to->z - from->z;
	
	float d = sqrt(dx*dx + dy*dy + dz*dz);
	
	if(d > 0.0){
		dx /= d;
		dy /= d;
		dz /= d;
	}
	
	*ux = dx;
	*uy = dy;
	*uz = dz;
	
	return d;
}

float Ranf(float low, float high){
	float r = (float) rand();
	return ( low + r*(high-low) / (float)RAND_MAX );
}

int Ranf(int ilow, int ihigh){
	float low = (float)ilow;
	float high =  (float)ihigh +.9999f;
	return (int)( Ranf(low,high));
}