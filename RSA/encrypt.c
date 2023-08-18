#include <stdio.h>
#include <gmp.h>
#include "rsa.h"
#include "numtheory.h"
#include "randstate.h"
#include <unistd.h>

int main(int argc, char **argv){
        int opt = 0;
	int value_v = 0;
        char *infile_file = NULL;
        char *outfile_file = NULL;
	char *pb_file = "rsa.pub";
        while ((opt = getopt(argc, argv, "i:o:n:vh"))!=-1){
			//specifies the input file that needs to get encrypted
                        if(opt=='i'){
				infile_file = optarg;
			}
			//specifies the output file to encrypt
			else if(opt=='o'){
				outfile_file = optarg;
			}
			//provides the file with the public key
			else if(opt=='n'){
				pb_file = optarg;
			}
			//verbose output is enabled
			else if(opt=='v'){
				value_v = 1;
			}
			//program usage
			else if(opt=='h'){
                                fprintf(stderr,
"Usage: ./encrypt [options]\n"
"  ./encrypt encrypts an input file using the specified public key file,\n"
"  writing the result to the specified output file.\n"
"    -i <infile> : Read input from <infile>. Default: standard input.\n"
"    -o <outfile>: Write output to <outfile>. Default: standard output.\n"
"    -n <keyfile>: Public key is in <keyfile>. Default: rsa.pub.\n"
"    -v          : Enable verbose output.\n"
"    -h          : Display program synopsis and usage.\n");
				return 0;
			}
			//invalid input
			else{
				fprintf(stderr,
"Usage: ./encrypt [options]\n"
"  ./encrypt encrypts an input file using the specified public key file,\n"
"  writing the result to the specified output file.\n"
"    -i <infile> : Read input from <infile>. Default: standard input.\n"
"    -o <outfile>: Write output to <outfile>. Default: standard output.\n"
"    -n <keyfile>: Public key is in <keyfile>. Default: rsa.pub.\n"
"    -v          : Enable verbose output.\n"
"    -h          : Display program synopsis and usage.\n");
                                return 1;
			}
                }
		//initialize variables to pass into functions
        	mpz_t n, e, s, m;
        	mpz_inits(n, e, s, m,  NULL);
       		//initialize username
        	char username[] = "";

		//open the public key file from keygen
		FILE *pbfile = fopen(pb_file, "r");
		
		//if the pbfile doesn't exist, or doesn't open properly, an error message gets printed
		if (pbfile == NULL) {
        		fprintf(stderr, "Couldn't open %s to read plaintext: No such file or directory.\n", pb_file);
        		mpz_clears(n, e, s, NULL);
        		return 1;
    		}

		//open the infile and the outfile and assign the infile to stdin and the outfile to stdout
		//this is for the infile
		FILE *infile = stdin;
		
		//if the infile doesn't open, give an error message, otherwise open the file and set it to read
		if (infile_file != NULL) {
                        if ((infile = fopen(infile_file, "r")) == NULL) {
                                fprintf(stderr, "Couldn't open %s to read plaintext: No such file or directory.\n", infile_file);
                                mpz_clears(n, e, s, NULL);
                                return 1;
                        }
                }
		
		//this is for the outfile
    		FILE *outfile = stdout;
		//if the outfile doesn't open, give error message, otherwise open or create file and set it to write
		if (outfile_file != NULL) {
                        if ((outfile = fopen(outfile_file, "w")) == NULL) {
                                fprintf(stderr, "Couldn't open %s to read plaintext: No such file or directory.\n", outfile_file);
                                mpz_clears(n, e, s, NULL);
                                return 1;
                        }
                }

		mpz_t change_user;
		mpz_init(change_user);

		//read the public key from the file that is opened
                rsa_read_pub(n, e, s, username, pbfile);
		
		//when the user wants verbose output, print this
                if (value_v==1) {
                        //username
                        gmp_printf("user = %s\n", username);
                        //signature
                        gmp_printf("s (%i bits) = %Zd\n", mpz_sizeinbase(s, 2), s);
                        //modulus
                        gmp_printf("n (%i bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
                        //exponent
                        gmp_printf("e (%i bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
                }

		//use the mpz function set_str and set the base value to 62 because the printed username is always in lowercase
    		mpz_set_str(change_user, username, 62); 
		
		//this is to verify the signature using the rsa function for verify
    		if (!(rsa_verify(change_user, s, e, n))) {
        		fprintf(stderr, "Error: signature could not be verified\n");
			mpz_clears(n, e, s, change_user,  NULL);
                        fclose(pbfile);
                        fclose(infile);
                        fclose(outfile);
                        return 1;
    		}

		//when the file is verified, then we can ecrypt the file
        	rsa_encrypt_file(infile, outfile, n, e); 
    		//clear initialized variables and files
                mpz_clears(n, e, s, change_user, NULL);
                fclose(pbfile);
                fclose(infile);
                fclose(outfile);
                return 0;
}
