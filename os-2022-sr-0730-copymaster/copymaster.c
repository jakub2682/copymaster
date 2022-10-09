#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "options.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <inttypes.h>
#include <stdbool.h>


void FatalError(char c, const char* msg, int exit_status);
void PrintCopymasterOptions(struct CopymasterOptions* cpm_options);

int Check(int infile, int outfile) {
    if (infile == -1 || outfile == -1) { return -1; }
    return 0;
}

int main(int argc, char* argv[])
{
    struct CopymasterOptions cpm_options = ParseCopymasterOptions(argc, argv);

    //-------------------------------------------------------------------
    // Kontrola hodnot prepinacov
    //-------------------------------------------------------------------

    // Vypis hodnot prepinacov odstrante z finalnej verzie

    PrintCopymasterOptions(&cpm_options);

    //-------------------------------------------------------------------
    // Osetrenie prepinacov pred kopirovanim
    //-------------------------------------------------------------------

    if (cpm_options.fast && cpm_options.slow) {
        fprintf(stderr, "CHYBA PREPINACOV\n");
        exit(EXIT_FAILURE);
    }
    if (cpm_options.create && cpm_options.overwrite) {
        fprintf(stderr, "CHYBA PREPINACOV\n");
        exit(EXIT_FAILURE);
    }
    if (cpm_options.append && cpm_options.overwrite) {
        fprintf(stderr, "CHYBA PREPINACOV\n");
        exit(EXIT_FAILURE);
    }
    if (cpm_options.append && cpm_options.lseek) {
        fprintf(stderr, "CHYBA PREPINACOV\n");
        exit(EXIT_FAILURE);
    }
    if (cpm_options.truncate && cpm_options.delete_opt) {
        fprintf(stderr, "CHYBA PREPINACOV\n");
        exit(EXIT_FAILURE);
    }



    int inSubor, outSubor,count,EndOfFile;
    inSubor = open(cpm_options.infile, O_RDONLY);
    // TODO Nezabudnut dalsie kontroly kombinacii prepinacov ...

    if (cpm_options.overwrite) {
        outSubor = open(cpm_options.outfile, O_WRONLY | O_TRUNC, 777);
        if (outSubor == -1) { FatalError('o', "SUBOR NEEXISTUJE", 24); }
    }
    else if (cpm_options.create) {
        outSubor = open(cpm_options.outfile, O_WRONLY | O_EXCL | O_CREAT, cpm_options.create_mode);
        if (outSubor == -1) { FatalError('c', "SUBOR NEEXISTUJE", 23); }
    }
    else if (cpm_options.append) {
        outSubor = open(cpm_options.outfile, O_WRONLY | O_EXCL | O_APPEND, 777);
        if (outSubor == -1) { FatalError('a', "SUBOR NEEXISTUJE", 22); }
    }else {
        outSubor = open(cpm_options.outfile, O_WRONLY | O_CREAT | O_TRUNC, 777);
        if(outSubor == -1){ FatalError('f',"INA CHYBA",21); }
    }


    if (cpm_options.fast)
    {
        count = 1000;

       if ((inSubor = open(cpm_options.infile, O_RDONLY)) == -1)
        {
           FatalError('f', "SUBOR NEEXISTUJE", 21);
        }

       if ((outSubor = open(cpm_options.outfile, O_WRONLY | O_TRUNC | O_CREAT, 777)) == -1)
       {
           FatalError('f', "SUBOR NEEXISTUJE", 21);
       }
    }

     else if (cpm_options.slow)
    {
        count = 1;

        if ((inSubor = open(cpm_options.infile, O_RDONLY)) == -1)
        {
            FatalError('s', "SUBOR NEEXISTUJE", 21);
        }

        if ((outSubor = open(cpm_options.outfile, O_WRONLY | O_TRUNC | O_CREAT, 777)) == -1)
        {
            FatalError('s', "SUBOR NEEXISTUJE", 21);
        }

    }

    else
    {
        count = 100;
        if ((inSubor = open(cpm_options.infile, O_RDONLY)) == -1)
        {
            FatalError('B', "SUBOR NEEXISTUJE", 21);
        }

        if ((outSubor = open(cpm_options.outfile, O_WRONLY | O_TRUNC | O_CREAT, 777)) == -1)
        {
            FatalError('B', "SUBOR NEEXISTUJE", 21);
        }
    }

    //-------------------------------------------------------------------
    // Kopirovanie suborov
    //-------------------------------------------------------------------

    // TODO Implementovat kopirovanie suborov

    char buf[count];


    while ((EndOfFile = read(inSubor, buf, count)) > 0)
    {
        write(outSubor, buf, EndOfFile);
    }


    //-------------------------------------------------------------------
    // Vypis adresara
    //-------------------------------------------------------------------

    if (cpm_options.directory){
            struct stat st;
            stat(cpm_options.infile, &st);

            if (!S_ISDIR(st.st_mode))
                FatalError('D', "VSTUPNY SUBOR NIE JE ADRESAR", 28);

            FILE* outfile = fopen(cpm_options.outfile, "w");

            if (outfile == NULL)
                FatalError('D', "VYSTUPNY SUBOR - CHYBA", 28);

            DIR* directory = opendir(cpm_options.infile);

            if (!directory)
                FatalError('D', "VSTUPNY SUBOR NEEXISTUJE", 28);
        }

        // TODO Implementovat vypis adresara


    //-------------------------------------------------------------------
    // Osetrenie prepinacov po kopirovani
    //-------------------------------------------------------------------

    // TODO Implementovat osetrenie prepinacov po kopirovani
    close(inSubor);
    close(outSubor);


    return 0;
}


void FatalError(char c, const char* msg, int exit_status)
{
    fprintf(stderr, "%c:%d\n", c, errno); 
    fprintf(stderr, "%c:%s\n", c, strerror(errno));
    fprintf(stderr, "%c:%s\n", c, msg);
    exit(exit_status);
}

void PrintCopymasterOptions(struct CopymasterOptions* cpm_options)
{
    if (cpm_options == 0)
        return;
    
    printf("infile:        %s\n", cpm_options->infile);
    printf("outfile:       %s\n", cpm_options->outfile);
    
    printf("fast:          %d\n", cpm_options->fast);
    printf("slow:          %d\n", cpm_options->slow);
    printf("create:        %d\n", cpm_options->create);
    printf("create_mode:   %o\n", (unsigned int)cpm_options->create_mode);
    printf("overwrite:     %d\n", cpm_options->overwrite);
    printf("append:        %d\n", cpm_options->append);
    printf("lseek:         %d\n", cpm_options->lseek);
    
    printf("lseek_options.x:    %d\n", cpm_options->lseek_options.x);
    printf("lseek_options.pos1: %ld\n", cpm_options->lseek_options.pos1);
    printf("lseek_options.pos2: %ld\n", cpm_options->lseek_options.pos2);
    printf("lseek_options.num:  %lu\n", cpm_options->lseek_options.num);
    
    printf("directory:     %d\n", cpm_options->directory);
    printf("delete_opt:    %d\n", cpm_options->delete_opt);
    printf("chmod:         %d\n", cpm_options->chmod);
    printf("chmod_mode:    %o\n", (unsigned int)cpm_options->chmod_mode);
    printf("inode:         %d\n", cpm_options->inode);
    printf("inode_number:  %lu\n", cpm_options->inode_number);
    
    printf("umask:\t%d\n", cpm_options->umask);
    for(unsigned int i=0; i<kUMASK_OPTIONS_MAX_SZ; ++i) {
        if (cpm_options->umask_options[i][0] == 0) {
            // dosli sme na koniec zoznamu nastaveni umask
            break;
        }
        printf("umask_options[%u]: %s\n", i, cpm_options->umask_options[i]);
    }
    
    printf("link:          %d\n", cpm_options->link);
    printf("truncate:      %d\n", cpm_options->truncate);
    printf("truncate_size: %ld\n", cpm_options->truncate_size);
    printf("sparse:        %d\n", cpm_options->sparse);
}
