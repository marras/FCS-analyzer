#include "correlator.h"
// #include "log.h"

#define MAX_M 30 //max delay = 2^31 // NOTE observe the size of int!!

CorrelatorPAT :: CorrelatorPAT (const vector<int>& channel/*, const double step_size, const int num_steps, 
	 const int _steps_per_bin*/) { //PAT = photon arrival times (dl. wektora = liczba fotonow w traj.)
	delay.clear();
	corr.resize(MAX_M);
	
	for (vector<int>::const_iterator i = channel.begin(); i != channel.end(); i++) {

	    static int iter = 0;
	  
	   for (uint d=0;d<delay.size();d++) { 
		delay[d] += *i; //co ja chce miec w tym delay??!!
// 		printf ("d=%d, delay[d]=%d\n",d,delay[d]);
		for (int m = 0; m < MAX_M; m++) {
		    if ((1 << m) >= delay[d]) {corr[m] ++; break;} //TODO optimize!
		}
	    }
	    delay.push_back(0);
	    if (iter%100 == 0) printf ("iter:=%d\n",iter);
	    iter++;
	}
	
	double avg_intensity = (double)channel.size()/(double)delay[0];
	
	for (int i=0;i<corr.size();i++) printf ("%d %lf \n",i,(double)corr[i]/avg_intensity);
}

CorrelatorPAT :: ~CorrelatorPAT () {
}

// double CorrelatorPAT :: Calculate_correlation (double time) {
// 
// 	int k = int (time / bin_size);		//k - delay (bin size units)
// 	double sum = 0;
// 
// 	assert (k>=0);
// 
// 	for (int i=0;i<B-k;i++) {
// 		if (photons[i]<0 || photons[i+k]<0) {	//DEBUG
// 			printf ("ERROR! ph[%d]= %d, ph[%d] = %d!\n",i,photons[i],i+k,photons[i+k]);
// 		}
// 
// 		assert (photons[i]>=0);
// 		assert (photons[i+k]>=0);
//  		sum += photons[i]*photons[i+k];
// 	}
//  	double correlation = sum/((B - k)*(avg_intensity*avg_intensity)) - 1;	//calculate the value of the correlation
// 	return correlation;
// }
