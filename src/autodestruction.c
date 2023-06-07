#include "includes.h"

// Just hanging around! 

/* Routine to execute in remote process.  */
static void remote_thread(SELFDEL *remote)
{
    // wait for parent process to terminate
    void *status;
    pthread_join(pthread_self(), &status);
    // try to delete the executable file 
    while(remote->fnUnlink(remote->szFileName) == -1)
    {
        // failed - try again in one second's time
        remote->fnSleep(1);
    }
    // finished! exit so that we don't execute garbage code
    remote->fnExit(0);
}


// PUBLIC 

/*  Delete currently running executable and exit     */
int SelfDelete(int fRemoveDirectory)
{
    SELFDEL local = {0};
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    } else if (pid == 0) { // child process
        // copy in binary code
        memcpy(local.opCodes, &remote_thread, CODESIZE);
        local.fnWaitForSingleObject = (void (*)(void *))pthread_join;
        local.fnCloseHandle = (void (*)(void *))pthread_detach;
        local.fnUnlink = unlink;
        local.fnSleep = (void (*)(unsigned int))sleep;
        local.fnExit = exit;
        local.fRemDir = fRemoveDirectory;
        getcwd(local.szFileName, PATH_MAX);
        strcat(local.szFileName, "/");
        strcat(local.szFileName, program_invocation_name);
        // Give remote process a copy of our own process pid
        local.hParent = getpid();
        // create detached thread
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
        pthread_t tid;
        int rc = pthread_create(&tid, &attr, (void *(*)(void *))&remote_thread, &local);
        pthread_attr_destroy(&attr);
        if (rc != 0) {
            perror("pthread_create");
            return -1;
        }
        // sleep for a second before exiting
        sleep(1);
        return 0;
    } else { // parent process
        return 1;
    }
}


int main(void)
{
    SelfDelete(1);
    return 0;
}

