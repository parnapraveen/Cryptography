#include "randstate.h"
#include <stdint.h>
#include <gmp.h>

//gmp_randstate_t state;

void randstate_init(uint64_t seed){
	//use the gmp randinit mt function to initialize the the state of the Mersenne Twister algorithm
	gmp_randinit_mt(state);
	//use the gmp randseed ui function with the state and initialize the seed value into the state 
	gmp_randseed_ui(state, seed);
}

void randstate_clear(void){
	//use gmp function to clear the random state
	gmp_randclear(state);
}
