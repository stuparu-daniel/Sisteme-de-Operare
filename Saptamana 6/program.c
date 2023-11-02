#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

int main(int argc, char *argv[]) 
{
    if (argc != 4) 
    {
        perror("Invalid number of arguments\n");
        exit(-1);
    }

    char *fin = argv[1];
    char *fout = argv[2];
    char caracter = argv[3][0];

    int fdin = open(fin, O_RDONLY);
    if (fdin == -1) 
    {
        perror("Error opening input file");
        exit(-1);
    }

    int fdout = open(fout, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fdout == -1) 
    {
        perror("Error opening output file");
        close(fdin);
        exit(-1);
    }

    struct stat fileInfo;
    if (fstat(fdin, &fileInfo) == -1) 
    {
        perror("Error getting file information");
        close(fdin);
        close(fdout);
        exit(-1);
    }
    int nr_lit_mici = 0;
    int nr_lit_mari = 0;
    int nr_cifre = 0;
    int nr_apar = 0;

    char buffer[1];
    while (read(fdin, buffer, 1) > 0) 
    {
        if (islower(buffer[0])) 
        {
            nr_lit_mici++;
        } 
        else if (isupper(buffer[0])) 
        {
            nr_lit_mari++;
        } 
        else if (isdigit(buffer[0])) 
        {
            nr_cifre++;
        }
        if (buffer[0] == caracter) 
        {
            nr_apar++;
        }
    }

    char output[256];
    sprintf(output, "numar litere mici: %d\nnumar litere mari: %d\nnumar cifre: %d\nnumar aparitii caracter: %d\ndimensiune fisier: %ld\n",
    nr_lit_mici, nr_lit_mari, nr_cifre, nr_apar, fileInfo.st_size);

    if (write(fdout, output, strlen(output)) == -1)
    {
        perror("Error writing to output file");
    }

close(fdin);
close(fdout);

return 0;
}
