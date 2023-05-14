#include <stdio.h>
#include <stdlib.h>

void createFile() {
    char* filename="100MB-File.txt";
    FILE* fp;
    long int i;    // Check if file already exists
    
    if (fopen(filename, "r") != NULL) {
        if (p_flag) {
            printf("File already exists\n");
        }
        return;
    }


    fp = fopen(filename, "wb");
    if (fp == NULL) {
        printf("Error opening file\n");
        return;
    }

    // //100MB
    for (i = 0; i < 10000000L; i++) { 
        fputc('a', fp);
    }

    fputc('x', fp);
    fclose(fp);

    if (p_flag)
    {
        printf("File created successfully\n");
    }
}