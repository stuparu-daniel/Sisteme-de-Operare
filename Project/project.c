#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

#define MAX_PATH_LENGTH 256
#define HEADER_SIZE 54
#define IMAGE_SIZES 18
#define MAX_BUFFER_SIZE 4096

void rigths(struct stat fileInfo, char* user, char* group, char* other)
{
    strcat(user, (fileInfo.st_mode & S_IRUSR) ? "R" : "-");
    strcat(user, (fileInfo.st_mode & S_IWUSR) ? "W" : "-");
    strcat(user, (fileInfo.st_mode & S_IXUSR) ? "X" : "-");

    strcat(group, (fileInfo.st_mode & S_IRGRP) ? "R" : "-");
    strcat(group, (fileInfo.st_mode & S_IWGRP) ? "W" : "-");
    strcat(group, (fileInfo.st_mode & S_IXGRP) ? "X" : "-");

    strcat(other, (fileInfo.st_mode & S_IROTH) ? "R" : "-");
    strcat(other, (fileInfo.st_mode & S_IWOTH) ? "W" : "-");
    strcat(other, (fileInfo.st_mode & S_IXOTH) ? "X" : "-");
}

void removeExtension(const char *fileName) {
    char *lastDot = strrchr(fileName, '.');
    if (lastDot != NULL) {
        *lastDot = '\0';
    }
}

int checkIfRegularFile(const char* fileName) {
    struct stat info;

    if (lstat(fileName, &info) == -1) {
        perror("Error getting file information\n");
        exit(EXIT_FAILURE);
    }

    return S_ISREG(info.st_mode);
}

void processImage(const char *inputFilePath, const char *outputDirectory, const char* name) {
    int file = open(inputFilePath, O_RDWR);
    if (file == -1) {
        perror("Error opening file\n");
        exit(EXIT_FAILURE);
    }

    struct stat info;

    if (stat(inputFilePath, &info) == -1) {
        perror("Error getting file information\n");
        exit(EXIT_FAILURE);
    }

    if(lseek(file, IMAGE_SIZES, SEEK_SET) == -1) {
        perror("Error setting cursor\n");
        close(file);
        exit(EXIT_FAILURE);
    }

    int height = 0, width = 0;
    read(file, &width, 4);
    read(file, &height, 4);
    char user_rights[4] = "", group_rights[4] = "", other_rights[4] = ""; 
    char date[20];
    char buffer[MAX_BUFFER_SIZE] = "";
    strftime(date, sizeof(date), "%d.%m.%Y", localtime(&info.st_mtime));
    rigths(info, user_rights, group_rights, other_rights);

    fflush(NULL);

    removeExtension(name);
    snprintf(buffer, sizeof(buffer), "nume fisier: %s\n"
                                     "inaltime: %d\n"
                                     "lungime: %d\n"
                                     "dimensiune: %ld\n"
                                     "identificatorul utilizatorului: %d\n"
                                     "timpul ultimei modificari: %s\n"
                                     "contorul de legaturi: %ld\n"
                                     "drepturi de acces user: %s\n"
                                     "drepturi de acces grup: %s\n"
                                     "drepturi de acces altii: %s\n\n", 
                name, height, width, info.st_size, info.st_uid, date, info.st_nlink, user_rights, group_rights, other_rights);

    char stat_file_name[MAX_BUFFER_SIZE] = "";
    snprintf(stat_file_name, sizeof(stat_file_name), "%s/%s_statistica.txt", outputDirectory, name);
    int file_descriptor_out = open(stat_file_name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (file_descriptor_out == -1) {
        perror("Error opening output file\n");
        exit(-1);
    }

    if(strcmp(buffer, "") != 0) {
        if (write(file_descriptor_out, buffer, strlen(buffer)) == -1) {
            perror("Error writing to output file\n");
            close(file);
            close(file_descriptor_out);
            exit(EXIT_FAILURE);
        }
    }

    close(file);
    close(file_descriptor_out);
}

void grayscaleImage(const char *inputFilePath)
{
    int file = open(inputFilePath, O_RDWR);
    if (file == -1) {
        perror("Error opening file\n");
        exit(EXIT_FAILURE);
    }

    if(lseek(file, 0, SEEK_SET) == -1) {
        perror("Error resetting cursor to the begining of the file\n");
        close(file);
        exit(EXIT_FAILURE);
    }

    if(lseek(file, HEADER_SIZE, SEEK_SET) == -1) {
        perror("Error jumping over header\n");
        close(file);
        exit(EXIT_FAILURE);
    }

    unsigned char pixel[3];
    while (read(file, pixel, 3) == 3) {
        unsigned char grayscalePixel = (unsigned char)(0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 * pixel[0]);
        lseek(file, -3, SEEK_CUR);
        write(file, &grayscalePixel, 1);
        write(file, &grayscalePixel, 1);
        write(file, &grayscalePixel, 1);
    }

    close(file);

    exit(EXIT_SUCCESS);
}

void processFile(const char *inputFilePath, const char *outputDirectory, const char* name) {

    struct stat info;

    if (lstat(inputFilePath, &info) == -1) {
        perror("Error getting file information\n");
        exit(EXIT_FAILURE);
    }

    char buffer[MAX_BUFFER_SIZE] = "";

    if(S_ISREG(info.st_mode))
    {
        strcpy(buffer, "");
        char user_rights[4] = "", group_rights[4] = "", other_rights[4] = ""; 
        char date[20];
        strftime(date, sizeof(date), "%d.%m.%Y", localtime(&info.st_mtime));
        rigths(info, user_rights, group_rights, other_rights);

        removeExtension(name);
        snprintf(buffer, sizeof(buffer), "nume fisier: %s\n"
                             "dimensiune: %ld\n"
                             "identificatorul utilizatorului: %d\n"
                             "timpul ultimei modificari: %s\n"
                             "contorul de legaturi: %ld\n"
                             "drepturi de acces user: %s\n"
                             "drepturi de acces grup: %s\n"
                             "drepturi de acces altii: %s\n\n", 
        name, info.st_size, info.st_uid, date, info.st_nlink, user_rights, group_rights, other_rights);

        char stat_file_name[MAX_BUFFER_SIZE] = "";
        snprintf(stat_file_name, sizeof(stat_file_name), "%s/%s_statistica.txt", outputDirectory, name);
        int file_descriptor_out = open(stat_file_name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (file_descriptor_out == -1) {
            perror("Error opening output file\n");
            exit(-1);
        }

        if(strcmp(buffer, "") != 0) {
            if (write(file_descriptor_out, buffer, strlen(buffer)) == -1) {
                perror("Error writing to output file\n");
                close(file_descriptor_out);
                exit(EXIT_FAILURE);
            }
        }

        close(file_descriptor_out);
    }
    else if(S_ISDIR(info.st_mode)) {
        strcpy(buffer, "");
        char user_rights[4] = "", group_rights[4] = "", other_rights[4] = ""; 
        rigths(info, user_rights, group_rights, other_rights);

        removeExtension(name);
        snprintf(buffer, sizeof(buffer), "nume director: %s\n"
                                        "identificatorul utilizatorului: %d\n"
                                        "drepturi de acces user: %s\n"
                                        "drepturi de acces grup: %s\n"
                                        "drepturi de acces altii: %s\n\n",
                name, info.st_uid, user_rights, group_rights, other_rights);
        
        char stat_file_name[MAX_BUFFER_SIZE] = "";
        snprintf(stat_file_name, sizeof(stat_file_name), "%s/%s_statistica.txt", outputDirectory, name);
        int file_descriptor_out = open(stat_file_name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (file_descriptor_out == -1) {
            perror("Error opening output file\n");
            exit(-1);
        }

        if(strcmp(buffer, "") != 0) {
            if (write(file_descriptor_out, buffer, strlen(buffer)) == -1) {
                perror("Error writing to output file\n");
                close(file_descriptor_out);
                exit(EXIT_FAILURE);
            }
        }

        close(file_descriptor_out);
    }
    else if(S_ISLNK(info.st_mode)) {
        strcpy(buffer, "");
        char user_rights[4] = "", group_rights[4] = "", other_rights[4] = ""; 
        rigths(info, user_rights, group_rights, other_rights);
        struct stat target_file_info;
        if(stat(inputFilePath, &target_file_info) == -1)
        {
            perror("Error getting target file information for symbolic link\n");
            exit(-1);
        }

        removeExtension(name);
        snprintf(buffer, sizeof(buffer), "nume legatura: %s\n"
                                        "dimensiune legatura: %ld\n"
                                        "dimensiune fisier: %ld\n"
                                        "drepturi de acces user: %s\n"
                                        "drepturi de acces grup: %s\n"
                                        "drepturi de acces altii: %s\n\n",
                name, info.st_size, target_file_info.st_size, user_rights, group_rights, other_rights);

        char stat_file_name[MAX_BUFFER_SIZE] = "";
        snprintf(stat_file_name, sizeof(stat_file_name), "%s/%s_statistica.txt", outputDirectory, name);
        int file_descriptor_out = open(stat_file_name, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (file_descriptor_out == -1) {
            perror("Error opening output file\n");
            exit(-1);
        }

        if(strcmp(buffer, "") != 0) {
            if (write(file_descriptor_out, buffer, strlen(buffer)) == -1) {
                perror("Error writing to output file\n");
                close(file_descriptor_out);
                exit(EXIT_FAILURE);
            }
        }

        close(file_descriptor_out);
    }
}

void processEntry(const char *entryName, const char *inputDirectory, const char *outputDirectory, const char scriptArgument) {
    char inputFilePath[MAX_PATH_LENGTH];
    snprintf(inputFilePath, sizeof(inputFilePath), "%s/%s", inputDirectory, entryName);

    int regFilePipe[2];
    if(pipe(regFilePipe) < 0) {
	  perror("Eroare la crearea pipe-ului\n");
	  exit(EXIT_FAILURE);
	}

    pid_t childPid1 = fork();

    if(childPid1 == -1) {
        perror("Error forking\n");
        exit(EXIT_FAILURE);
    } else if(childPid1 == 0) {

        if(strstr(entryName, ".bmp") != NULL) {

            //Get data about image
            processImage(inputFilePath, outputDirectory, entryName);
            exit(EXIT_SUCCESS);

        } else {

            //Get data about file
            processFile(inputFilePath, outputDirectory, entryName);
            if(checkIfRegularFile(inputFilePath) == 0) {
                exit(EXIT_SUCCESS);
            }

            close(regFilePipe[0]);
            dup2(regFilePipe[1], STDOUT_FILENO); //Redirecting stdout to pipe for the parent
            close(regFilePipe[0]);
            execlp("cat", "cat", inputFilePath, (char *)NULL);

            //Shouldn't get here, only if command fails
            perror("Error executing cat command\n");
            exit(EXIT_FAILURE);

        }
    } else {
        if(strstr(entryName, ".bmp") != NULL) {

            pid_t childPid2 = fork();

            if(childPid2 == -1) {
                perror("Error forking\n");
                exit(EXIT_FAILURE);

            } else if(childPid2 == 0) {
                grayscaleImage(inputFilePath);
                exit(EXIT_SUCCESS);

            } else {
                int status;
                pid_t who;
                who = wait(&status);

                if (WIFEXITED(status)) {
                    printf("S-a încheiat procesul cu pid-ul %d si codul %d\n", who, WEXITSTATUS(status));
                } else {
                    printf("Procesul cu pid-ul %d si codul %d nu s-a încheiat normal\n", who, WEXITSTATUS(status));
                }

                who = wait(&status);
                if (WIFEXITED(status)) {
                    printf("S-a încheiat procesul cu pid-ul %d si codul %d\n", who, WEXITSTATUS(status));
                } else {
                    printf("Procesul cu pid-ul %d si codul %d nu s-a încheiat normal\n", who, WEXITSTATUS(status));
                }
            }
        } else if(checkIfRegularFile(inputFilePath) != 0){
            int status;
            pid_t who;
            who = wait(&status);

            if (WIFEXITED(status)) {
                printf("S-a încheiat procesul cu pid-ul %d si codul %d\n", who, WEXITSTATUS(status));
            } else {
                printf("Procesul cu pid-ul %d si codul %d nu s-a încheiat normal\n", who, WEXITSTATUS(status));
            }

            pid_t childPid3 = fork();

            if(childPid3 == -1) {
                perror("Error forking\n");
                exit(EXIT_FAILURE);

            } else if(childPid3 == 0) {


                close(regFilePipe[1]);
                dup2(regFilePipe[0], STDIN_FILENO); //redirecting stdin to pipe
                char scriptArgs[2];
                scriptArgs[0] = scriptArgument;
                scriptArgs[1] = '\0';
                execlp("bash", "bash", "propozitii.sh", scriptArgs, (char *)NULL);

                //Shouldn't get here, only if command fails
                perror("Error executing propozitii.sh script\n");
                exit(EXIT_FAILURE);

            } else {

                close(regFilePipe[0]);
                int status;
                pid_t who;
                who = wait(&status);

                if (WIFEXITED(status)) {
                    printf("S-a încheiat procesul cu pid-ul %d si codul %d\n", who, WEXITSTATUS(status));
                    printf("Au fost identificate in total %d propozitii corecte care contin caracterul %c\n", WEXITSTATUS(status), scriptArgument);
                } else {
                    printf("Procesul cu pid-ul %d si codul %d nu s-a încheiat normal\n", who, WEXITSTATUS(status));
                }
            }
        } else {
            int status;
            pid_t who;
            who = wait(&status);

            if (WIFEXITED(status)) {
                printf("S-a încheiat procesul cu pid-ul %d si codul %d\n", who, WEXITSTATUS(status));
            } else {
                printf("Procesul cu pid-ul %d si codul %d nu s-a încheiat normal\n", who, WEXITSTATUS(status));
            }
        }

    }
}

void processDirectory(const char *inputDirectory, const char *outputDirectory, const char scriptArgument) {
    DIR *dir;
    struct dirent *entry;

    dir = opendir(inputDirectory);
    if (dir == NULL) {
        perror("Error opening input directory\n");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            processEntry(entry->d_name, inputDirectory, outputDirectory, scriptArgument);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        perror("Mod de utilizare: ./program <director_intrare> <director_iesire> <c>\n");
        exit(EXIT_FAILURE);
    }

    const char *inputDirectory = argv[1];
    const char *outputDirectory = argv[2];
    const char scriptArgument = argv[3][0];

    processDirectory(inputDirectory, outputDirectory, scriptArgument);
    

    return 0;
}
