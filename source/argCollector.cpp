#include "argCollector.h"

argCollector::argCollector(int argc, char *argv[]) {

    const char *help = "\
        --version, -v Version print\n\
        --read1-input, -1 Read 1 input (comma sep for multiple files)\n\
        --read2-input, -2 Read 2 input (comma sep for multiple files)\n\
        --singleend-input, -U Single end read input (comma sep for multiple files)\n\
        --tab-input, -T Tab input (comma sep for multiple files)\n\
        --interleaved-input, -I Interleaved input I (comma sep for multiple files)\n\
        --stdin-input, -S STDIN input (MUST BE TAB DELIMITED INPUT)\n\
        --start, -s start location for unique ID (int)\n\
        --length, -l length of unique ID (int)\n\
        --quality-check-off, -q Quality Checking Off First Duplicate seen will be kept\n\
        --gzip-output, -g Output gzipped\n\
        --interleaved-output, -i Output to interleaved\n\
        --fastq-output, -f Fastq format outputted (R1 and R2)\n\
        --force, -F Forces overwrite of files\n\
        --tab-output, -t Tab-delimited output\n\
        --to-stdout, -O Prints to STDOUT in Tab Delimited\n\
        --prefix, -P Prefix for outputted files\n\
        --log-file, -L Output-Logfile\n\
        --no-log, N No logfile (outputs to stderr)\n\
        --help, -h Prints help.\n";

    
    /*Initilize Values*/ 
    R1_In = NULL;
    R2_In = NULL;
    SE_In = NULL;
    TAB_In = NULL;
    INTER_In = NULL;
    STDIN_In = NULL;
    R1_Out = NULL;
    R2_Out = NULL;
    SE_Out = NULL;
    log = NULL;

    bpWindowSize = 12;
    startLoc = 10; 
    
    int long_index;
    int cmd_line_char;
    extern char *optarg;
    extern int optind;
    extern int optopt;
    extern int opterr;

    const struct option longopts[] = {
        {"version", no_argument, 0, 'v'},
        {"read1-input", required_argument, 0, '1'},
        {"read2-input", required_argument, 0, '2'},
        {"singleend-input", required_argument, 0, 'U'},
        {"tab-input", required_argument, 0, 'T'},
        {"interleaved-input", required_argument, 0, 'I'},
        {"stdin-input", no_argument, 0, 'S'},
        {"start", required_argument, 0, 's'},
        {"length", required_argument, 0, 'l'},
        {"quality-check-off", no_argument, 0, 'q'},
        {"gzip-output", no_argument, 0, 'g'},
        {"interleaved-output", no_argument, 0, 'i'},
        {"fastq-output", no_argument, 0, 'f'},
        {"force", no_argument, 0, 'F'},
        {"tab-output", no_argument, 0, 't'},
        {"to-stdout", no_argument, 0, 'O'},
        {"prefix", no_argument, 0, 'P'},
        {"help", no_argument, 0, 'h'},
        {"log-file", required_argument, 0, 'L'},
        {"no-log", no_argument, 0, 'N'},
        {0, 0, 0, 0}
    };

    /*Qual check on by defualt*/
    bool qual_check = false;


    /*Fastq by default*/
    bool gzip_out = false;
    bool tab = false;
    bool to_stdout = false;
    bool interleaved = false;
    bool fastq = true;
    bool force = false;
    char *prefix = NULL;

    char *tmplog = (char *)malloc(10);
    sprintf(tmplog, "sd.log");

    while ((cmd_line_char = getopt_long(argc, argv, "VNL:1:2:U:I:T:Os:l:qgiftsP:FSh", longopts, &long_index)) != EOF) {
        switch(cmd_line_char) {
            case 'N':
                log = stderr;
                break;
            case 'L':
                tmplog = strdup(optarg);
                break;
            case 'O':
                to_stdout = true;
                fastq = false;
                break;
            case 'v':
                fprintf(stderr, "Versions goes here\n");
                exit(4);
            case '1':
                R1_In = new FileHelper(optarg);
                R1_In->setFastq(true);
                break;
            case 'S':
                STDIN_In = new FileHelper();
                break;
            case '2':
                R2_In = new FileHelper(optarg);
                R2_In->setFastq(true);
                break;
            case 'U':
                SE_In = new FileHelper(optarg);
                SE_In->setFastq(true);
                break;
            case 'T':
                TAB_In = new FileHelper(optarg);
                TAB_In->setTab(true);
                break;
            case 'I':
                INTER_In = new FileHelper(optarg);
                INTER_In->setInterleaved(true);
                break;
            case 'q':
                qual_check = false;
                break;
            case 'g':
                gzip_out = true;
                break;
            case 'i':
                interleaved = true;
                fastq = false;
                break;
            case 'f':
                fastq = true;
                break;
            case 't':
                tab = true;
                fastq = false;
                break;
            case 's':
                startLoc = atoi(optarg);
                break;
            case 'l':
                bpWindowSize = atoi(optarg);
                break;
            case 'F':
                force = true;
                break;
            case 'P':
                prefix = strdup(optarg);
                break;
            case 'h':
                //fprintf(stderr, "Helpful help doc :)\n");
                fprintf(stderr, "%s\n", help );
                exit(28);
                break;
            default:
                fprintf(stderr, "This argument is no good %c\n Bad argument :( Exiting\n", cmd_line_char);
                exit(5);
           }  
    }

    if (force || access(tmplog, F_OK) == -1) {
        if (log == NULL) {
            log = fopen(tmplog, "w");
            if (log == NULL) {
                fprintf(stderr, "Within argCollecter.cpp in argCollector tmpLog all ready exists\n");
                fprintf(stderr, "Error with opening file %s\n", tmplog);
                exit(29);
            }
        }
    } else if (!log) {
        fprintf(stderr, "Within argCollecter.cpp in argCollector tmpLog all ready exists\n");
        fprintf(stderr, "Please, use -F option (force overwrite), -L to change the name, or manually remove or move %s\n", tmplog);
        exit(28);
    }
       

    if (!R1_In && !R2_In && !SE_In && !TAB_In && !INTER_In && !STDIN_In) {
        fprintf(stderr, "Within argCollector.cpp argCollector(). Error: No input files given\n");
        exit(17);
    }

    /*Make sure R1 and R2 are given*/
    if ((!R1_In && R2_In) || (R1_In && !R2_In) ) {
        fprintf(stderr, "error in argCollector.cpp in function argCollector()\n");
        fprintf(stderr, "R1 and R2 (-1 and -2) must both supplied.\n");
        exit(15);
    }

    /*Default prefix is no_dup*/
    /*no_dup or prefix will be followed by either _R1.fastq, _R2.fastq, _SE.fastq, _interleaved.fastq, _tab.tastq*/
    if (!prefix) {
        prefix = (char *)malloc(6*sizeof(char));
        sprintf(prefix, "no_dup");
    }

    if (tab) {
        /*Sets up character start with prefix (either collected or default)*/
        /*Plus one is full the null character*/
        char *r1 = (char *)malloc(strlen(prefix)+strlen("_tab.tastq")+1);
        sprintf(r1, "%s_tab.tastq", prefix);

        /*Spins of file writer*/  
        R1_Out = new FileWriter(force);
        R1_Out->OpenFile(r1);
        R1_Out->setTab(true);
        R1_Out->setForce(true);
        R2_Out = NULL;
        SE_Out = NULL;
    } else if (interleaved) {
        /*Sets up character start with prefix (either collected or default)*/
        /*Plus one is full the null character*/
        char *r1 = (char *)malloc(strlen(prefix)+strlen("_interleaved.fastq")+1);
        sprintf(r1, "%s_interleaved.fastq", prefix);

        R1_Out = new FileWriter(force);
        R1_Out->OpenFile(r1);
        R1_Out->setInterleaved(true);
        R1_Out->setForce(force);

        R2_Out = NULL;
        
        /*SE read MIGHT need to be used if Tab input or SE input*/
        if (SE_In || TAB_In || STDIN_In) {
            char *se = (char *)malloc(strlen(prefix)+strlen("_SE.fastq")+1);
            sprintf(se, "%s_SE.fastq", prefix);
            SE_Out = new FileWriter(force);
            SE_Out->OpenFile(se);
            SE_Out->setFastq(true);
            SE_Out->setForce(force);
        }

    } else if (fastq) {
        /*Sets up character start with prefix (either collected or default)*/
        /*Plus one is full the null character*/
        /*Make sure there is at least one R1 or R2*/
        if (R1_In || TAB_In || INTER_In || STDIN_In) {
            char *r1 = (char *)malloc(strlen(prefix)+strlen("_R1.fastq")+1);
            sprintf(r1, "%s_R1.fastq", prefix);
            char *r2 = (char *)malloc(strlen(prefix)+strlen("_R2.fastq")+1);
            sprintf(r2, "%s_R2.fastq", prefix);

            R1_Out = new FileWriter(force);
            R1_Out->OpenFile(r1);
            R1_Out->setFastq(true);
            R1_Out->setForce(force);

            R2_Out = new FileWriter(force);
            R2_Out->OpenFile(r2);
            R2_Out->setFastq(true);
            R2_Out->setForce(force);
        }

        if (SE_In || TAB_In || STDIN_In) {
            char *se = (char *)malloc(strlen(prefix)+strlen("_SE.fastq")+1);
            sprintf(se, "%s_SE.fastq", prefix);
            SE_Out = new FileWriter(force);
            SE_Out->OpenFile(se);
            SE_Out->setFastq(force);
        }
    } else if (to_stdout) {
        R1_Out = new FileWriter(force);
        R1_Out->setFastq(false);
        R1_Out->setInterleaved(false);
        R1_Out->setStdout(true);
        R1_Out->OpenFile(NULL);
        R1_Out->setForce(force);


        R2_Out = NULL;
        SE_Out = NULL;        
    }



}
