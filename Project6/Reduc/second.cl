kernel
void
ArrayMultReduce( global const float *dA, global const float *dB, local float *prods, global float *dC )
{
	int gid = get_global_id( 0 ); // 0 = X dimension , this returns which index in the x dimension array we're in
	int numItems = get_local_size(0); // Number of items per work group
	int tnum = get_local_id(0); // thread number in this work group 0-> numItems-1

	int wgnum = get_group_id(0);// which work group are you in

	prods[tnum] = dA[gid] * dB[gid];

	for(int offset = 1; offset < numItems; offset *= 2 ){
		int mask = offset*2 -1 ;
		barrier(CLK_LOCAL_MEM_FENCE); // wait for comp
		if( (tnum & mask) == 0){
			prods[tnum] += prods[tnum+offset];
		}
	}
	barrier(CLK_LOCAL_MEM_FENCE);
	if(tnum ==0)
		dC[wgnum] = prods[0];
}
