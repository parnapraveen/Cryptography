#include <stdio.h>
#include <gmp.h>
#include "rsa.h"
#include "numtheory.h"
#include "randstate.h"
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <inttypes.h>

int main(int argc, char **argv){
	int opt = 0;
	uint64_t seed = time(NULL);
	uint64_t iters = 50;
    	uint64_t bits = 1024;
    	int value_v = 0;
    	char *pb_file = "rsa.pub";
    	char *pv_file = "rsa.priv";
	int temp;
	int temp2;
	while ((opt = getopt(argc, argv, "b:i:n:d:s:vh"))!=-1){
		switch (opt){
			case 'b':
				temp = strtoul(optarg, NULL, 10);
				if(temp>0){
					bits = temp;
				}
				//bits = strtoul(optarg, NULL, 10);
				break;
			case 'i':
				temp2 = strtoul(optarg, NULL, 10);
				if(temp2>0){
					iters = temp2;
				}
				break;
			case 'n':
				pb_file = optarg;
				break;
			case 'd':
				pv_file = optarg;
				break;
			case 's':
				seed = strtoul(optarg, NULL, 10);
				break;
			case 'v':
				value_v = 1;
				break;
			case 'h':
				fprintf(stderr,
"Usage: ./keygen [options]\n"
"  ./keygen generates a public / private key pair, placing the keys into the public and private\n"
"  key files as specified below. The keys have a modulus (n) whose length is specified in\n"
"  the program options.\n"
"    -s <seed>   : Use <seed> as the random number seed. Default: time()\n"
"    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024\n"
"    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50\n"
"    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n"
"    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv\n"
"    -v          : Enable verbose output.\n"
"    -h          : Display program synopsis and usage.\n");
				return 0;
			default:
				fprintf(stderr,
"Usage: ./keygen [options]\n"
"  ./keygen generates a public / private key pair, placing the keys into the public and private\n"
"  key files as specified below. The keys have a modulus (n) whose length is specified in\n"
"  the program options.\n"
"    -s <seed>   : Use <seed> as the random number seed. Default: time()\n"
"    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024\n"
"    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50\n"
"    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub\n"
"    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv\n"
"    -v          : Enable verbose output.\n"
"    -h          : Display program synopsis and usage.\n");
				return 1;
			}
        	}

	//open files to the write mode
	FILE *pbfile = fopen(pb_file, "w");
	FILE *pvfile = fopen(pv_file, "w");

	//if either of the files have nothing then print error message
    	if (pvfile == NULL) {
        	chmod(pv_file, 0600);
        	pvfile = fopen(pv_file, "w");
    	}

    	if (pbfile == NULL) {
        	fprintf(stderr, "Error: failed to open pbfile.\n");
        	return 1;
    	}

    	if (pvfile == NULL) {
        	fprintf(stderr, "Error: failed to open pvfile.\n");
        	return 1;
    	}

    	// Initialize the random state for the seed
	randstate_init(seed);
	//printf("%"PRIu64"\n", seed);

	//initializing all variables needed
    	mpz_t s, p, q, n, e, d, m;
    	mpz_inits(s, p, q, n, e, d, m, NULL);
	
	// Make the public and private keys
    	rsa_make_pub(p, q, n, e, bits, iters);
    	//gmp_printf("n first: %Zd \n", n);
	rsa_make_priv(d, e, p, q);
	
	/*
	gmp_printf("p: %Zd \n", p);
	gmp_printf("q: %Zd \n", q);
	gmp_printf("n: %Zd \n", n);
	gmp_printf("e: %Zd \n", e);
	printf("%s%"PRIu64"\n", "bits: ", bits);
	printf("%s%"PRIu64"\n", "iters: ", iters);
	*/

	// get username
    	char *username = getenv("USER");

	// Username should become mpz_t to base 62
    	mpz_set_str(m, username, 62);
    	//Then, compute the signature of the username
	rsa_sign(s, m, d, n); 

    	// write the public and private key to their files
    	rsa_write_pub(n, e, s, username, pbfile);
    	rsa_write_priv(n, d, pvfile);
	
	// If verbose output is enabled print the following:
    	if (value_v==1) {
		//username
        	gmp_printf("username: %s\n", username); 
		//the signature
        	gmp_printf("user signature (%i bits): %Zd\n", mpz_sizeinbase(s, 2), s);
        	//large prime p
		gmp_printf("p (%i bits): %Zd\n", mpz_sizeinbase(p, 2), p);
        	//large prime q
		gmp_printf("q (%i bits): %Zd\n", mpz_sizeinbase(q, 2), q);
        	//modulus n
		gmp_printf("n - modulus (%i bits): %Zd\n", mpz_sizeinbase(n, 2), n);
        	//exponent e
		gmp_printf("e - public exponent (%i bits): %Zd\n", mpz_sizeinbase(e, 2), e);
        	//private key d
		gmp_printf("d - private exponent (%i bits): %Zd\n", mpz_sizeinbase(d, 2), d);
    	}
	//close the files
    	fclose(pbfile);
    	fclose(pvfile);
	//clear the randstate
    	randstate_clear();
	//clear variables
    	mpz_clears(s, p, q, n, e, d, m, NULL);
    	return 0;
}
