#include <stdio.h>
#include <gmp.h>
#include "rsa.h"
#include "numtheory.h"
#include "randstate.h"
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>

//helper function for finding least common denominator
void lcm(mpz_t d, mpz_t a, mpz_t b){
	//initialize the needed variables 
	mpz_t mul_num, abs_numerator, numerator, denominator, divided;
	mpz_inits(mul_num, abs_numerator, numerator, denominator, divided, NULL);

	//Carmichael's generalization of Euler's theorem
	//|(p-1)(q-1)|/gcd(p-1, q-1)
	mpz_mul(mul_num, a, b);
	mpz_abs(abs_numerator, mul_num);
	//use the absolute value function from the gmp library to make sure the numerator positive
	mpz_set(numerator, abs_numerator);
	gcd(denominator, a, b);
	mpz_fdiv_q(divided, numerator, denominator);
	mpz_set(d, divided);
	mpz_clears(mul_num, abs_numerator, numerator, denominator, divided, NULL);
}

void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters){
	//Declaring all the variables necessary 
	mpz_t bits_p, bits_q, total_b, mpz_up, mpz_low, range, rand_bit, one, lambda, random_num, gcd_value, p_sub, q_sub;
	//Initializing all the variables above using the one function inits instead of individually
	mpz_inits(bits_p, bits_q, total_b, mpz_up, mpz_low, range, rand_bit, one, lambda, random_num, gcd_value, p_sub, q_sub, NULL);
	//According to given algorithm, the upper range is 3*nbits/4 and the lower range is nbits/4
	//The final number of bits is given so the total bits variable is set to the parameter value for bits
	mpz_set_ui(one, 1);
	mpz_set_ui(total_b, nbits);
	size_t upper = (3*nbits)/4;
	size_t lower = nbits/4;
	
	//set the upper limit to the int upper
	mpz_set_ui(mpz_up, upper);
	//set the lower limit to the int lower
	mpz_set_ui(mpz_low, lower);
	//the range of values to be passed into the random function is the upper limit minus the lower limit
	mpz_sub(range, mpz_up, mpz_low);
	//use the random function to store a random integer into rand_bit
	mpz_urandomm(rand_bit, state, range);
	//add the random value to the bits associated with p
	mpz_add(bits_p, rand_bit, mpz_low);
	//subtract the bits associated with b from the total to get the remaining amount that will be added to q
	mpz_sub(bits_q, total_b, bits_p);
	//in the assignment doc the value of parameter n is the product of p and q
	
	//mpz_mul(n, p, q);
	//gmp_printf("n in function: %Zd \n", n);
	
	//create the two large prime numbers and store in p and q parameters from the function
	make_prime(p, mpz_get_ui(bits_p), iters);
	make_prime(q, mpz_get_ui(bits_q), iters);
	mpz_mul(n, p, q);
	//gmp_printf("n in function: %Zd \n", n);

	//calculate lcm and gcm using p-1 and q-1
	//initializing variables for these:
	mpz_sub_ui(p_sub, p, 1);
	mpz_sub_ui(q_sub, q, 1);

	//gmp_printf("p_sub: %Zd \n", p_sub);
        //gmp_printf("q_sub: %Zd \n", q_sub);

	lcm(lambda, p_sub, q_sub);
	//gmp_printf("lcm: %Zd \n", lambda);

	while(true){
		mpz_urandomb(random_num, state, nbits);
		gcd(gcd_value, random_num, lambda);
		//check whether lambda and random number are coprime by checking that their gcd is 1
		if(mpz_cmp(gcd_value, one)==0){
			break;
		}
	}
	mpz_set(e, random_num);
	
	mpz_clears(bits_p, bits_q, total_b, mpz_up, mpz_low, range, rand_bit, one, lambda, random_num, gcd_value, p_sub, q_sub);
}

void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile){
	//Declare and initiate temporary variables for the values of n, e, and s
	mpz_t n_temp, e_temp, s_temp;
	mpz_inits(n_temp, e_temp, s_temp, NULL);
	//set the temporary variables equal to the parameter values that are equal to them
	mpz_set(n_temp, n);
	mpz_set(e_temp, e);
	mpz_set(s_temp, s);
	//print each value and then the username
	//use gmp function that helps format and print the values in the files
	gmp_fprintf(pbfile, "%Zx\n", n);
	gmp_fprintf(pbfile, "%Zx\n", e);
	gmp_fprintf(pbfile, "%Zx\n", s);
	gmp_fprintf(pbfile, "%s\n", username);
	//clear all values
	mpz_clears(n_temp, e_temp, s_temp, NULL);
}

void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile){
	//use fscanf to read the stream from a file
	gmp_fscanf(pbfile, "%Zx\n", n);
        gmp_fscanf(pbfile, "%Zx\n", e);
        gmp_fscanf(pbfile, "%Zx\n", s);
        gmp_fscanf(pbfile, "%s\n", username);
}

void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q){
	//declare and initiate variables for p-1, q-1, and lcm
	mpz_t p_sub, q_sub, low;
	mpz_inits(p_sub, q_sub, low, NULL);
	//gmp_printf("make_priv p: %Zd \n", p);
        //gmp_printf("make_priv q: %Zd \n", q);
	//calculate p-1 and q-1 and store in the variabls
	mpz_sub_ui(p_sub, p, 1);
	mpz_sub_ui(q_sub, q, 1);
	//gmp_printf("make_priv p_sub: %Zd \n", p_sub);
	//gmp_printf("make_priv q_sub: %Zd \n", q_sub);
	//calculate lcm, lambda(n)
	lcm(low, p_sub, q_sub);
	
	//gmp_printf("make priv low: %Zd \n", low);
	
	//calculate mod_inverse and store in d
	mod_inverse(d, e, low);
	mpz_clears(p_sub, q_sub, low, NULL);
}

void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile){
	 //Create temporary variables for the values of n and d
	 mpz_t n_temp, d_temp;
	 mpz_inits(n_temp, d_temp, NULL);
	 //set them equal to the parameter values
	 mpz_set(n_temp, n);
	 mpz_set(d_temp, d);
	 //print the values as hexstrings
	 gmp_fprintf(pvfile, "%Zx\n", n_temp);
	 gmp_fprintf(pvfile, "%Zx\n", d_temp);
	 //clear the values
	 mpz_clears(n_temp, d_temp, NULL);
}

void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile){
	//scan the values of n and d from the file
	gmp_fscanf(pvfile, "%Zx\n", n);
    	gmp_fscanf(pvfile, "%Zx\n", d);
}

void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n){
	//using formula from assignment doc, plugged in the variables into the pow_mod function
	pow_mod(c, m, e, n);
}

void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e){
	//initialize variables needed
        mpz_t log_t, int_size, temp, k_val, j_val, m_val, c_val, one, eight;
        mpz_inits(log_t, int_size, temp, k_val, j_val, m_val, c_val, NULL);
        mpz_init(one);
        mpz_init(eight);
        //int i;
        //set the values of the constants
        mpz_set_ui(one, 1);
        mpz_set_ui(eight, 8);
        //initialize variable to keep track of when the block ends and for final check of if the entire file is read
        int length_block;
        //initialize size of each value in block to 8 bit by default
        uint8_t k_size = 8;
        //begin calculation of [logbase2(n)-1/8] rounded down
        //initialize logbasetwo value
        mpz_set_ui(int_size, mpz_sizeinbase(n,2));
        //subtract one
        mpz_sub(temp, int_size, one);
        //divide by eight
        mpz_fdiv_q(log_t, temp, eight);
        unsigned long n_val=0;
        n_val = mpz_get_ui(log_t);
	mpz_set_ui(k_val, n_val);
	mpz_sub(k_val, k_val, one);

        //in order to initialize the array with all the output values, you need the size in the type int, so after solving for the block size, we have to cast this as an integer.
        k_size = (int)mpz_get_ui(k_val);
        //mpz_sub(int_k_val, int_k_val, one);

        //initialize array for the entire output and allocate memory
        uint8_t *all = (uint8_t*)calloc(k_size+1, sizeof(uint8_t));
        //initialize array for each block size and allocate memory
        char *block = (char*)calloc(k_size, sizeof(char));

        //continue reading the file until the end, which will be null
        if(infile!=NULL){
                //continue reading each block while the size of the block is less than the first index
                while(1){
                        //set the zeroth byte to 0xFF because this is the workaround byte on the output file
                        all[0] = 0xFF;
                        //fread function takes in a pointer block of memory and reads the data given through the stream that is pointed at. The second parameter is for size in bytes of each element that needs to be read. The third represents the number of elements with the size bytes and the last is the file name which is the strem itself.
                        size_t j_val = fread(block, sizeof(char), k_size, infile);
			
			//loop that goes to the end of the block and appends appropriate values from the file to the array representing the block. This is shifted one because of the zeroth byte
                        length_block = (int)j_val;	
                        int i = 0;
                        while(i<(length_block)){
                              all[i+1] = block[i];
                              i+=1;
                        }

                        //the mpz import function takes in the array of word dats, the amount of words, each in size bytes, and the assignment doc says to set the endian as 1 and nails to 0
                        //the size of the prepended value will be used for size
			mpz_import(m_val, j_val+1, 1, sizeof(all[0]), 1, 0, all);
                        //encrypt the value at m_val
			rsa_encrypt(c_val, m_val, e, n);
			//print the values into outfile
                        gmp_fprintf(outfile, "%Zx\n", c_val);
			//if the end of the block has been reached, break the while loop to continue onto the next block
                        if(length_block<k_size){break;}
                }
        }
	//clear initialized variables and free the arrays
        mpz_clears(log_t, int_size, temp, k_val, j_val, m_val, c_val, one, eight, NULL);
        free(all);
        free(block);
}

void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n){
	//using formula, use pow_mod again to decrypt
	pow_mod(m, c, d, n);
}


void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d){	
	mpz_t log_t, int_size, temp, k_val, m_val, c_val;
	mpz_t one, eight;
        mpz_inits(log_t, int_size, temp, k_val, m_val, c_val, NULL);
        mpz_init(one);
        mpz_init(eight);
        //set the values of the constants
        mpz_set_ui(one, 1);
        mpz_set_ui(eight, 8);
        //initialize variable to keep track of when the block ends and for final check of if the entire file is read
        //initialize size of each value in block to 1 bit by default
        uint8_t k_size = 1;
        //begin calculation of [logbase2(n)-1/8] rounded down
        //initialize logbasetwo value
        mpz_set_ui(int_size, mpz_sizeinbase(n,2));
        //subtract one
        mpz_sub(temp, int_size, one);
        //divide by eight
        mpz_fdiv_q(log_t, temp, eight);
        unsigned long n_val=0;
        n_val = mpz_get_ui(log_t);
        //value of the block
        mpz_set_ui(k_val, n_val);
        //in order to initialize the array with all the output values, you need the size in the type int, so after solving for the block size, we have to cast this as an integer.
        k_size = (int)mpz_get_ui(k_val);
	//allocate space for the output array
	uint8_t *all = (uint8_t*)calloc(k_size, sizeof(uint8_t));
	
	//to the end of the file
	if(infile!=NULL){
		//keep reading the blocks of text
		while(1){
			//assignment doc says to scan hexstring and save it in mpz_t c_val
			//use the mpz function for scanning files in order to scan over the section/block of hextring
			int scanned = gmp_fscanf(infile, "%Zx\n", c_val);
			//when the end of the file is reached break
			if(scanned==EOF){
				break;
			}

			//assignment doc says to then decrypt the c_val using function before this
			size_t j_val;
			//j has to be an integer val when passed into fwrite, so declare variable for that
			int j_integer_val;
			rsa_decrypt(m_val, c_val, d, n);
			//mpz_export function has 6 parameters. The first one will be the arr passed in, the second one will be the size of the integer because the total count will be multiplied by this size to get the count, then it will be the order which the assignment says to set as 1, then the endian parameter which the assignment also says to set as 1 and then 0 for the nails parameter. This will all be stored in the last value which is m_val
			mpz_export(all, &j_val, 1, sizeof(all[0]), 1, 0, m_val);
			j_integer_val = (int)j_val;
			//the fwrite function has 4 parameters, one for the pointer array, another for the size in bytes of each element that will get written, the number of elements, and the stream which will be the file itself. In this case, set the first one to the array all+1 because the first value is 0xFF that was prepended in encrypt, the second to the size of an 8 bit number, the first the the value of j-1 because the assignment says it starts from index 1 of the block, and the last is the outfile
			fwrite(all+1, sizeof(uint8_t), j_integer_val-1, outfile);
		}
	}
	//free the array and clear variables
	free(all);
	mpz_clears(log_t, int_size, temp, k_val, m_val, c_val, one, eight);
}


void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n){
	//gmp_printf("rsa_sign m: %Zd \n", m);
	//gmp_printf("rsa_sign d: %Zd \n", d);
	//gmp_printf("rsa_sign n: %Zd \n", n);
	pow_mod(s,m,d,n);
}

bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n){
	//use a temporary variable to store pow_mod and initialize
	mpz_t temp;
    	mpz_init(temp);
	//use pow_mod to get the inverse value from signing
   	pow_mod(temp, s, e, n);
	//compare the pow_mod value to the message and if they're equal return true
    	if (mpz_cmp(temp, m) == 0) {
		//clear the temporary variable and return true because the message equals the pow_mod
        	mpz_clear(temp);
        	return true;
    	} else {
		//clear the temporary variable and return false because the message doesn't equal the pow_mod result
        	mpz_clear(temp);
        	return false;
    	}
}

