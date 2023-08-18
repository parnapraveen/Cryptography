#include <stdio.h>
#include <gmp.h>
#include "rsa.h"
#include "numtheory.h"
#include "randstate.h"
#include <unistd.h>


int main(int argc, char **argv){
        int opt = 0;
	int value_v = 0;
	char *pv_file = "rsa.priv";
	char *infile_file = NULL;
    	char *outfile_file = NULL;
	while ((opt = getopt(argc, argv, "i:o:n:vh"))!=-1){
		//input file
		if(opt=='i'){
			infile_file = optarg;
		}
		//output file
		else if(opt=='o'){
			outfile_file = optarg;
		}
		//private key file
		else if(opt=='n'){
			pv_file = optarg;
		}
		//verbose output
		else if(opt=='v'){
			value_v = 1;
		}
		//help/usage
		else if(opt=='h'){
                        fprintf(stderr,
"Usage: ./decrypt [options]\n"
"  ./decrypt decrypts an input file using the specified private key file,\n"
"  writing the result to the specified output file.\n"
"    -i <infile> : Read input from <infile>. Default: standard input.\n"
"    -o <outfile>: Write output to <outfile>. Default: standard output.\n"
"    -n <keyfile>: Private key is in <keyfile>. Default: rsa.priv.\n"
"    -v          : Enable verbose output.\n"
"    -h          : Display program synopsis and usage.\n");
				return 0;
			}
		//default state when something wrong is entered
		else{
			fprintf(stderr,
"Usage: ./decrypt [options]\n"
"  ./decrypt decrypts an input file using the specified private key file,\n"
"  writing the result to the specified output file.\n"
"    -i <infile> : Read input from <infile>. Default: standard input.\n"
"    -o <outfile>: Write output to <outfile>. Default: standard output.\n"
"    -n <keyfile>: Private key is in <keyfile>. Default: rsa.priv.\n"
"    -v          : Enable verbose output.\n"
"    -h          : Display program synopsis and usage.\n");
				return 1;
                	}
	}
	//initialize variables for reading the private key
        mpz_t d, n;
        mpz_init(n);
        mpz_init(d);

	//open the private key file with fopen as described in the assignment doc
	FILE *pvfile = fopen(pv_file, "r");
	//if the pbfile doesn't exist, or doesn't open properly, an error message gets printed
        if (pvfile == NULL) {
        	fprintf(stderr, "Couldn't open %s to read plaintext: No such file or directory.\n", pv_file);
                mpz_clears(d, n,  NULL);
                return 1;
        }


	//open the infile and set to stdin so that the encrypted file can be parsed by decrypt
	FILE *infile = stdin;
	if (infile_file != NULL) {
                if ((infile = fopen(infile_file, "r")) == NULL) {
                        fprintf(stderr, "Couldn't open %s to read plaintext: No such file or directory.\n", infile_file);
                        mpz_clears(d, n, NULL);
                        return 1;
                }
        }


	//set the outfile so that the decrypted text can be written into the outward stream
	FILE *outfile = stdout;
	//if the outfile doesn't open, give error message, otherwise open or create file and set it to write
        if (outfile_file != NULL) {
                if ((outfile = fopen(outfile_file, "w")) == NULL) {
                        fprintf(stderr, "Couldn't open %s to read plaintext: No such file or directory.\n", outfile_file);
                        mpz_clears(d, n, NULL);
                        return 1;
                }
        }

	//read the private key as the assignment document says
	rsa_read_priv(n,d,pvfile);
	
	//if the user wants verbose output, then print the following
	if(value_v==1){
		//the modulus
		gmp_printf("n (%i bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        	//the private key
		gmp_printf("d (%i bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
	}
	
	//decrypt the file next, as the assignment document says
	rsa_decrypt_file(infile, outfile, n, d);
	
	//close all files and clear variables
    	fclose(pvfile);
    	fclose(infile);
    	fclose(outfile);
    	mpz_clears(d, n, NULL);
    	return 0;
		
}

