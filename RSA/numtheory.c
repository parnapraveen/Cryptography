#include "numtheory.h"
#include <gmp.h>
#include "numtheory.h"
#include "randstate.h"
#include <stdbool.h>
#include <stdint.h>

gmp_randstate_t state;

void gcd(mpz_t d, mpz_t a, mpz_t b){
	mpz_t t, temp_a, temp_b;
	//initialize all variables
	mpz_init(t);
	mpz_init(temp_a);
	mpz_init(temp_b);
	//store a and b in temporary variables
	mpz_set(temp_a, a);
	mpz_set(temp_b, b);
	//while b!=0
	while(mpz_sgn(temp_b)!=0){
		//t <-- b
		mpz_set(t, temp_b);
		//b <-- a % b
		mpz_mod(temp_b, temp_a, temp_b);
		//a <-- t
		mpz_set(temp_a, t);
	}
	//return a (set the output parameter to d)
	mpz_set(d, temp_a);
	mpz_clears(t, temp_a, temp_b, NULL);
}

void mod_inverse(mpz_t o, mpz_t a, mpz_t n){
	mpz_t one, r, r_prime, t, t_prime, q, r_subq_mul_rprime, t_subq_mul_tprime, r_divide, r_temp, t_temp;
	//initialize all variables
	mpz_init(one);
	mpz_set_si(one,1);
	mpz_init(r);
	mpz_init(r_prime);
	mpz_init(t);
	mpz_init(t_prime);
	mpz_init(q);
	mpz_init(r_subq_mul_rprime);
	mpz_init(t_subq_mul_tprime);
	mpz_init(r_divide);
	mpz_init(r_temp);
	mpz_init(t_temp);
	
	//(r, r') <-- (n,a)
	mpz_set(r,n);
	mpz_set(r_prime, a);
	//(t,t') <-- (0,1)
	mpz_set_si(t, 0);
	mpz_set_si(t_prime, 1);
	//while r'!=0
	while(mpz_sgn(r_prime)!=0){
		//r/r'
		mpz_fdiv_q(r_divide, r, r_prime);
		//q <-- r/r'
		mpz_set(q, r_divide);
		//(r, r') <-- (r', r-q*r')
		//r_temp <-- r
		mpz_set(r_temp, r);
		//r <-- r'
		mpz_set(r, r_prime);
		//q*r' <-- r_subq_mul_rprime
		mpz_mul(r_subq_mul_rprime, q, r_prime);
		//r_subq_mul_rprime <-- r - q*r'
		mpz_sub(r_subq_mul_rprime, r_temp, r_subq_mul_rprime);
		//r' <-- r_subq_mul_rprime
		mpz_set(r_prime, r_subq_mul_rprime);
		//(t, t') <-- (t', t-q*t')
		//t_temp <-- t
		mpz_set(t_temp, t);
		//t <-- t'
		mpz_set(t, t_prime);
		//t_subq_mul_tprime <-- q*t'
		mpz_mul(t_subq_mul_tprime, q, t_prime);
		//t_subq_mul_tprime <-- t-q*t'
		mpz_sub(t_subq_mul_tprime, t_temp, t_subq_mul_tprime);
		//t' <-- t_subq_mul_tprime
		mpz_set(t_prime, t_subq_mul_tprime);
	}
	//if r>1
	if(mpz_cmp(r, one)>0){
		//return no inverse, or 0
		mpz_set_si(o, 0);
	}
	else{
		//if t<0
		if(mpz_sgn(t)<0){
			//t <-- t+n
			mpz_add(t,t,n);
		}
		//return t
		mpz_set(o,t);
	}
	mpz_clears(one, r, r_prime, t, t_prime, q, r_subq_mul_rprime, t_subq_mul_tprime, r_divide, r_temp, t_temp, NULL);

}

void pow_mod(mpz_t o, mpz_t a, mpz_t d, mpz_t n){
	//Initialize the variables necessary in type mpz
	mpz_t two, p, temp_d, o_temp;
	mpz_init(two);
	mpz_init(p);
	mpz_init(temp_d);
	mpz_init(o_temp);
	//when dividing by 2 later on, set a variable equal to two first
	mpz_set_si(two, 2);

	//set initial value of o to 1, which will get converted to the output value
	//v <-- 1
	mpz_set_si(o_temp,1); 
	//p <-- a
	mpz_set(p, a);
	mpz_set(temp_d, d);
	//begin loop that continues while the value of the exponent is greater than zero
	while (mpz_sgn(temp_d)>0){
		//use mpz built-in function to check if the value of d is odd
		if (mpz_odd_p(temp_d) !=0){
			//v = v*p % n
			mpz_mul(o_temp,o_temp,p);
			mpz_mod(o_temp,o_temp,n);
		}
		//p = p*p % n
		mpz_mul(p,p,p);
		mpz_mod(p,p,n);
		//d = d/2
		//Using mpz function for division of two mpz values and setting equal to an mpz variable
		mpz_fdiv_q(temp_d, temp_d, two);
	}
	//set the initial parameter value to the value of v_temp so that the "return value" gets set
	mpz_set(o, o_temp);
	mpz_clears(two, p, temp_d, o_temp, NULL);
}

bool is_prime(mpz_t n, uint64_t iters){
	mpz_t three, four, temp_n, temp_mod, two, zero, one, n_sub, r, s_sub, n_subtwo, a, y, j, s;
	//initialize a variable for n-1
	mpz_init(n_sub);
	mpz_sub_ui(n_sub, n, 1);
	
	mpz_init(r);

	//initialize variables for finding n-1=(2^s)*r
        mpz_init(temp_n);
        mpz_init(temp_mod);
	mpz_init(s);
	//constant comparison numbers
        mpz_init(two);
        mpz_init(zero);
        mpz_init(one);
	mpz_init(four);
	mpz_init(three);
	mpz_set_si(two, 2);
        mpz_set_si(one, 1);
        mpz_set_si(zero, 0);
	mpz_set_si(four, 4);
	mpz_set_si(three, 3);
	
	if (mpz_cmp(n, two) == 0) {
        	mpz_clears(three, four, temp_n, temp_mod, two, zero, one, n_sub, r, s_sub, n_subtwo, a, y, j, s, NULL);
        	return true;
    	}
   	 if (mpz_cmp(n, three) == 0) {
        	mpz_clears(three, four, temp_n, temp_mod, two, zero, one, n_sub, r, s_sub, n_subtwo, a, y, j, s, NULL);
        	return true;
    	}
    	if (mpz_cmp(n, one) == 0) {
        	mpz_clears(three, four, temp_n, temp_mod, two, zero, one, n_sub, r, s_sub, n_subtwo, a, y, j, s, NULL);
        	return false;
    	}

    	if (mpz_cmp(n, zero) == 0) {
        	mpz_clears(three, four, temp_n, temp_mod, two, zero, one, n_sub, r, s_sub, n_subtwo, a, y, j, s, NULL);
        	return false;
    	}
	if (mpz_cmp(n, four) == 0) {
                mpz_clears(three, four, temp_n, temp_mod, two, zero, one, n_sub, r, s_sub, n_subtwo, a, y, j, s, NULL);
                return true;
        }

	//finding totient (n-1=(2^s)*r)
	//temp_n = n-1
        mpz_set(temp_n, n_sub);
	//temp_mod = temp_n %2
        mpz_mod(temp_mod, temp_n, two);
        //while temp_mod == 0	
        while(mpz_cmp(temp_mod, zero)==0){
		//s+=1
                mpz_add_ui(s, s, 1);
		//temp_n/=2
                mpz_fdiv_q(temp_n, temp_n, two);
		//temp_mod = temp_n % 2
                mpz_mod(temp_mod, temp_n, two); 
        }
	//set r = temp_n
        mpz_set(r, temp_n);
	//end of finding totient

	//values needed within function
	//initialize a variable for s-1
	mpz_init(s_sub);
	mpz_sub_ui(s_sub, s, 1);

	//initialize a variable for n-2
	mpz_init(n_subtwo);
	mpz_sub_ui(n_subtwo, n, 2);

	mpz_inits(a,y,j, NULL);
	mpz_set(a, zero);
	//for i <-- 1 to the number of total iterations
	for(uint64_t i = 1; i<iters; i+=1){
		//random number from 0 to n-2
		mpz_urandomm(a, state, n_subtwo);
		//make sure number is greater than 2
		
		//mpz_mod(a,a,n_sub);
		mpz_add_ui(a,a,2);

		//set variable y equal to power-mod(a,r,n)
		pow_mod(y, a, r, n);
		//gmp_printf("powmod: %Zd \n", y);
		
		//if y isn't 1 and y isn't equal to n-1
		if ((mpz_cmp(y, one)!=0) && (mpz_cmp(y, n_sub)!=0)){
			//set j equal to 1
			mpz_set_ui(j, 1);
			//mpz_sub_ui(s, s, 1);
			
			//while j<=s-1 and y!=n-1
			while((mpz_cmp(j, s_sub)<=0) && (mpz_cmp(y, n_sub)!=0)){
				//set y equal to power-mod(y,2,n)
				pow_mod(y, y, two, n);
				//gmp_printf("powmod2: %Zd \n", y);

				//if y equals 1
				if(mpz_cmp(y, one) == 0){
					//return false and clear variables
					mpz_clears(three, four, temp_n, temp_mod, two, zero, one, n_sub, r, s_sub, n_subtwo, a, y, j, s, NULL);
					return false;
				}
				//j equals j+1
				mpz_add_ui(j, j, 1);
			}
			//if y!= n-1
			if (mpz_cmp(y, n_sub)!=0){
				//return false and clear variables
				mpz_clears(three, four, temp_n, temp_mod, two, zero, one, n_sub, r, s_sub, n_subtwo, a, y, j, s, NULL);
				return false;
			}
		}
	}
	//return true because the value is prime, clear variables
	mpz_clears(three, four, temp_n, temp_mod, two, zero, one, n_sub, r, s_sub, n_subtwo, a, y, j, s, NULL);
	return true;

}

void make_prime(mpz_t p, uint64_t bits, uint64_t iters){
	//specified number of bits so there needs to be two variables to store the bits and the actual value
	mpz_t correct_bits, base_two_power;
	
	mpz_init(correct_bits);
	mpz_init(base_two_power);
	//the values are in base 2, so setting the exponent to the number of bits gives the right number of values
	mpz_ui_pow_ui(base_two_power, 2, bits);
	//once value is prime, the function stops
	int made = 0;
	while (made==0){
		mpz_urandomb(correct_bits, state, bits);
		mpz_add(correct_bits, correct_bits, base_two_power);
		//since the current value that is made prime is not the actual one we're looking for, increment to the next prime value
		mpz_nextprime(correct_bits, correct_bits);
		//run isprime function the number of iters from the parameter until it results in true
		if(is_prime(correct_bits, iters)){
			mpz_set(p, correct_bits);
			made = 1;
		}
	}
	//clear variables
	mpz_clears(correct_bits, base_two_power, NULL);
}
