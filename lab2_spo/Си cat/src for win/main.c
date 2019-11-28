#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>


 
enum gflagsEnum
{
    show_all,
    number_nonblank,
    show_ends,
    number,
    squeeze_blank,
    show_tabs,
    all
};


 
int gflags[all] = { 0 };
 
void printFile(const char*);
void formatLine(const char*, char*);
void setupFlags(enum gflagsEnum);
 
void printHelp(int);
void printVersion();
 
int main(int argc, char* argv[])
{
    while(1)
    {
        int c;
        int ind = 0;
        static struct option options[] =
        {
            {"show-all",        0, NULL, 'A'},
            {"number-nonblank", 0, NULL, 'b'},
            {"show-ends",       0, NULL, 'E'},
            {"number",          0, NULL, 'n'},
            {"squeeze-blank",   0, NULL, 's'},
            {"show-tabs",       0, NULL, 'T'},
            {"help",            0, NULL, 'h'},
            {"version",         0, NULL, 'v'},
            {NULL,              0, NULL,  0 },
        };
        
        c = getopt_long(argc, argv, "AbEnsT", options, &ind);
        
        if(c == -1)
            break;
        
        switch(c)
        {       
        case 'h':
            printHelp(0);
            
        case 'v':
            printVersion();
            
        case 'A':
            setupFlags(show_ends);
            setupFlags(show_tabs);
            break;
            
        case 'b':
            setupFlags(number_nonblank);
            break;
        
        case 'E':
            setupFlags(show_ends);
            break;
        
        case 'n':
            setupFlags(number);
            break;
            
        case 's':
            setupFlags(squeeze_blank);
            break;
            
        case 'T':
            setupFlags(show_tabs);
            break;
            
        case '?':   
            printHelp(1);
        }
    }
    
    if(optind == argc)
        printFile("-");
    
    while(optind < argc)
        printFile(argv[optind++]);
 
    exit(0);
}
 
void printFile(const char* file)
{
    FILE* f;
    static char buf[BUFSIZ];
    static char fBuf[BUFSIZ];
    if(strcmp(file, "-") == 0)
        f = stdin;
    else if((f = fopen(file, "r")) == NULL)
    {
        perror(file);
        return;
    }
    
    while(fgets(buf, BUFSIZ, f))
    {
        formatLine(buf, fBuf);
        if(fputs(fBuf, stdout) == EOF)
        {
            perror("cat");
            break;
        }
        *fBuf = '\0';
    }
        
    if(ferror(f))
        perror(file);
        
    if(f != stdin)
        fclose(f);
}
 
void formatLine(const char* line, char* formatted_line)
{
    static size_t line_counter = 1;
    static int prev_blank = 0;
    if(gflags[squeeze_blank])
    {
        if(*line == '\n')
        {
            if(prev_blank)
                *formatted_line = '\0';
 
            else
            {
                *formatted_line++ = '\n';
                *formatted_line = '\0';
            }
            prev_blank = 1;
            return;
        }
        else
            prev_blank = 0;
    }
    if(gflags[number] || gflags[number_nonblank])
    {
        if(*line == '\n')
        {
            if(!gflags[number_nonblank])
                sprintf(formatted_line, "%6u  ", line_counter++);
        }
        else
            sprintf(formatted_line, "%6u  ", line_counter++);
    }
    if(gflags[show_tabs])
    {
        const char* pl;
        char* pfl;
        for(pl = line, pfl = formatted_line + strlen(formatted_line); *pl; ++pl)
        {
            if(*pl == '\t')
            {
                *pfl++ = '^';
                *pfl++ = 'I';
            }
            else
                *pfl++ = *pl;
        }
        *pfl = '\0';
    }
    else
        strcat(formatted_line, line);
    if(gflags[show_ends])
    {
        size_t len = strlen(formatted_line);
        if(len)
            --len;
        formatted_line[len] = '$';
        formatted_line[len + 1] = '\n';
        formatted_line[len + 2] = '\0';
    }
}
 
void setupFlags(enum gflagsEnum curr_flag)
{
    gflags[curr_flag] = 1;
}
 
void printHelp(int ret_code)
{
    puts(
        "\n./cat - объединяет файлы и выводит их на стандартный вывод\n"
        "Использование:\n"
        "./cat [OPTION]... [FILE]...\n\n"
        "Доступные опции:\n"
        "\t-A, --show-all\n"
        "\t\tэквивалентно -ET\n"
        "\t-b, --number-nonblank\n"
        "\t\tнумерует непустые строки\n"
        "\t-E, --show-ends\n"
        "\t\tвыводить символ $ в конце каждой строки\n"
        "\t-n, --number\n"
        "\t\tнумерует все строки\n"
        "\t-s, --squeeze-blank\n"
        "\t\tподавляет повторяющиеся пустые строки\n"
        "\t-T, --show-tabs\n"
        "\t\tвыводит символы TAB как ^I\n"
        "\t    --help\n"
        "\t\tвыводит справку\n"
        "\t    --version\n"
        "\t\tвыводит информацию о версии программы\n\n"
        "Если FILE не задан или задан как -, то читает стандартный ввод");
		
    exit(ret_code);
}
 
void printVersion()
{
    puts(
        "\nmycat 1.0\nАналог программы cat, созданный для учебных целей."
        );
    exit(0);
}
