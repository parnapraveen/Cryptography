# Public Key Cryptography

## Building
Run the first command in order to make encrypt.c, decrypt.c, and keygen.c executable
...
make
...

## Running
In order to run keygen, use
...
./keygen [- ]
...

The value in the brackets should be substituted by:
<br>
-b: specifies the minimum bits needed for the public modulus n(default: 1024).
<br>
-i: specifies the number of Miller-Rabin iterations for testing primes (default: 50).
<br>
-n pbfile: specifies the public key file (default: rsa.pub).
<br>
-d pvfile: specifies the private key file (default: rsa.priv).
<br>
-s: specifies the random seed for the random state initialization (default: the seconds since the
UNIX epoch, given by time(NULL)).
<br>
-v: enables verbose output.
<br>
-h: displays program synopsis and usage.
<br>

In order to run encrypt, use
...
./encrypt [- ]
...

The value in the brackets should be substituted by:
<br>
-i: specifies the input file to encrypt (default: stdin).
<br>
-o: specifies the output file to encrypt (default: stdout).
<br>
-n: specifies the file containing the public key (default: rsa.pub).
<br>
-v: enables verbose output.
<br>
-h: displays program synopsis and usage.
<br>

In order to run decrypt, use
...
./decrypt [- ]
...

The value in the brackets should be substituted by:
<br>
-i: specifies the input file to encrypt (default: stdin).
<br>
-o: specifies the output file to encrypt (default: stdout).
<br>
-n: specifies the file containing the private key (default: rsa.priv).
<br>
-v: enables verbose output.
<br>
-h: displays program synopsis and usage.
<br>

## Files

1. decrypt.c: This contains the implementation and main() function for the decrypt program.
2. encrypt.c: This contains the implementation and main() function for the encrypt program.
3. keygen.c: This contains the implementation and main() function for the keygen program.
4. numtheory.c: This contains the implementations of the number theory functions.
5. numtheory.h: This specifies the interface for the number theory functions.
6. randstate.c: This contains the implementation of the random state interface for the RSA library
and number theory functions.
7. randstate.h: This specifies the interface for initializing and clearing the random state.
8. rsa.c: This contains the implementation of the RSA library.
9. rsa.h: This specifies the interface for the RSA library.
10. Makefile: makes the files executable
11. README.md: describes how to execute code

