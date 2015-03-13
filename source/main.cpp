#include<unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <fcntl.h>
#include <zlib.h>

#include "Binary_Search_Tree.h"
#include <getopt.h>
#include <time.h>
#include <unistd.h>

#include<stdint.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>



bool DEBUG = false;

const char *usage_string = 
"\n\n\n"
"Usage: super_deduper [OPTIONS] -1  <comma seperated .fastq.gz/fastq files> -2 <comma seperated .fastq.gz/fastq file>\n"
"	super_deduper [OPTIONS] -U <comma seperated .fastq.gz/fastq files>\n"
"	super_deduper [OPTIONS] -i <comma seperated .fastq.gz/fastq files>\n"
"\n\n"
"-------------------------------Description-----------------------------------\n"
"\n"
"Super Deduper is a duplication removal application designed to remove duplicates\n"
"introduced by PCR amplification. It accomplishes this by creating a binary search\n"
"tree and creating a unique sequence ID for each read (or region of read) specified\n"
"by the user. This approach creates a fast, accurate de-duplication that is able to be\n"
"used on non-mapped reads directly from a fastq.gz/fastq file. It also allows users to \n"
"receive the highest quality sequence back (by default) allowing for higher quality\n"
"data to be outputted. \n"
"\n\n"
"-------------------------------Required Inputs-------------------------------\n"
"\n"
"Required inputs are fastq.gz paired end reads specified in -1 and -2, or single end\n"
"fastq.gz file -U. Both of these can be comma separated and given multiple files\n"
"When using the -1 and -2 options, the comma separated files are assumed\n"
"to show up in the same order as the pairs occur.\n"
"\n\n"
"-------------------------------Output----------------------------------------\n"
"\n\n"
"The default output is output_nodup_PE1.fastq and output_nodup_PE2 for paired end reads\n"
"and output_nodup_PE1 for single end reads. However, the prefix can be changed\n"
"from output to whatever is deemed appropriate by the user with the -o option\n"
"(the suffix is always _nodup_PE?.fastq). Additionally, this program will discard\n"
"any sequence that contains an N in the sequence ID. This is because N's should be\n"
"of low quality, and they are not 'true DNA' sequence. The number discarded will be\n"
"be outputted at the end or during verbose options (-v).\n"
"\n\n"
"-------------------------------Options---------------------------------------\n"
"  -1, --read1 PATHS		Paths to paired end Read 1 fastq.gz files that are desired\n"
"		 to be deduplicated. Multiple files can be give if they are comma \n"
"		separated.\n"
"\n"
"  -2, --read2 PATHS		Paths to paired end Read 2 fastq.gz files that are desired\n"
"		 to be deduplicated. Multiple files can be give if they are comma \n"
"		separated.\n"
"\n"
"  -U, --singleend PATHS	Paths to single end reads fastq.gz files that are desired\n"
"		 to be deduplicated. Multiple files can be give if they are comma \n"
"		separated.\n"
"\n"
"  -i, --interleaved-input PATH Paths to interleaved fastq/fastq.gz files that are desired\n"
"		to be deduplicated. Multiple files can be given if they are comma \n"
"		seperated.\n"
"\n"
"  -o, --interleaved-output PATH Output in interleaved format. This output will go to\n"
"		prefix + '_nodup_PE1.fastq' or '_nodup_PE2.fastq.gz' if -g is specified\n"
"\n"
"  -p, --output-prefix PREFIX	Prefix to outputted unique sequences. Default: output\n"
"\n"
"  -g, --gzip-output		Will output in a gzipped compressed file. This will slow down\n"
"		Run time significantly\n"
"\n"
"  -s, --start NUM		The starting location for the starting base pair of the unique\n"
"		sequence ID. Default: 10\n"
"\n"
"  -l, --length NUM		The length of the base pairs in the unique sequence ID (the number.\n"
"		of base pairs in the unique ID). Default 25\n"
"\n"
"  -M, --memory-saving-on	Turns on a memory efficiency. Using efficient memory options slows \n"
"		down the speed of computation and uses virtual memory, swap space, and IO versus RAM.\n"
"		(This slows things down by about 10 times, but cuts back on RAM usage by about 1/6.)\n"
"		Default: false\n"
"\n"
"  -q, --quality-check-off	Turns off quality check. This will reduce run time since it will not\n"
"		compute nor check for highest quality scores. (For fastest speeds run this option\n"
"		with -M) Default: false\n"
"\n"
"  -v, --verbose		Turns on verbose option - ouputs information every 1,000,000 reads. Default: false\n"
"\n"
"  -O, --output-tree FILE 	Name of file where you what the outputed tree information to go.\n"
"\n"
"  -I, --input-tree PATHS	Name of the input file in which you want to upload a previously\n"
" 			seen tree before (output of -O).\n"
"\n"
"\n"
"version 1.1.1\n"
;


/*struct for arguments
 * typedef to args*/
typedef struct {

	char **read_1;
	char **read_2;
	char **single_reads;
	char **interleaved;

	char **input_trees;	
	
	int read_1_files;
	int read_2_files;
	int single_files;
	int tree_files;
	int interleaved_files;

	
	char *output_filename_1;
	char *output_filename_2;
	char *output_tree;
	
	int start;
	int length;

	bool mem_saving;
	bool quality_checking;
	bool verbose;
	bool interleaved_input;
	bool interleaved_output;	
	bool gzip_output;
	bool output_stdout;
	

} args;

/*Constrcutor function for args*/

void Start_Args(args*  arg) {
	arg->output_tree = NULL;
	arg->start = 10;
	arg->length = 25;
	arg->read_1_files = 0;
	arg->read_2_files = 0;
	arg->single_files = 0;
	arg->tree_files = 0;
	arg->mem_saving = false;
	arg->quality_checking = true;
	arg->verbose = false;
	arg->interleaved_input = false;
	arg->interleaved_output = false;
	arg->gzip_output = false;
	arg->output_filename_1 = strdup("output");
	arg->output_filename_2 = strdup("output");

}


/*Since there is a 64 unsigned int you can add 31 base pairs at a time*/
int BPS_ADDED = 31;
int size;

/* This function is the convert that creates a sequence binary number (seq_bin) used in the binary search tree */
bool converter(char *test, int start, int end, uint64_t *seq_bin_id, int &bp_added, int &index, uint64_t &incrementor, bool cr);


/*Checks to see if the file is gzipped
 * or of the file is just fastq*/
int gzipped_File(char *fname) {

	FILE *test = fopen(fname, "r");
	/*4096 is just arbritary*/
	char tmp[4096];
	int i = 0;
	bool check1 = false, check2 = false;

	while (fgets(tmp, 4096, test) != NULL) {
		if (i == 0 && tmp[0] == '@') {
			check1 = true;
		}
		if (i == 2 && tmp[0] == '+') {
			check2 = true;
		}
		if (i == 3) {
			break;
		}
		i++;
	}

	fclose(test);
	/*empty case*/
	if (i == 0) {
		return -1;
	/*fastq format files*/
	} else if (check1 && check2) {
		return 0;
	} else {
		return 1;
	}
				
}

/*Get four lines in files for @id, sequence, +, quality*/

bool get_four(char ***buf, FILE *f) {
	for (int i = 0; i < 4; i++) {
		if (fgets((*buf)[i], 4096, f) == NULL) {
			return false;
		}
	}

	/* 1st must be an '@' and 3rd must be '+'*/
	if ((*buf)[0][0] == '@' && (*buf)[2][0] == '+') {
		return true;
	} else {
		fprintf(stderr, "FASTQ format is not held, please check your files\n");
		exit(-100);
	}


}

/*Get data will pull in the file information*/
bool get_data(char ***buf_1, char ***buf_2, FILE *R1, FILE *R2, bool interleaved) {

	bool check, check2;
	/*interleaved R1 then R2*/
	if (interleaved) {
		check = get_four(buf_1, R1);
		check2 = get_four(buf_2, R1);

		if (check != check2) {
			fprintf(stderr, "Read 1 and Read 2 Lengths are not equal\n");
			exit(-101);
		}

	/*This is the case of a single end reads*/
	} else if (R2 == NULL) {
		check = get_four(buf_1, R1);
	/*Read 1 and Read 2 conditions*/
	} else {
		check = get_four(buf_1, R1);
		check2 = get_four(buf_2, R2);

		if (check != check2) {
			exit(-101);
		}
	}

	return check;
	
}



uint64_t *Greater_Than(uint64_t* seq, uint64_t* seq_cr, int size) {
	int i = 0;

	while (i < size && seq[i] == seq_cr[i]) {
		i++;
	}

	if (seq[i] > seq_cr[i]) {
		return seq;
	} else {
		return seq_cr;
	}

}

bool Fill_In_Binary_Tree(Binary_Search_Tree_Read_1_Read_2 *x, FILE* file_1, FILE* file_2, args *arg, FILE *f_read1, FILE *f_read2, double time_start) {


	       	 
	char **buf_1 = NULL;
        char **buf_2 = NULL;

	buf_1 = (char **)malloc(sizeof(char *) * 4);
	buf_2 = (char **)malloc(sizeof(char *) * 4);
	
	for (int tmp = 0; tmp < 4; tmp++) {
		buf_1[tmp] = (char *)malloc(sizeof(char)*4096);	
		buf_2[tmp] = (char *)malloc(sizeof(char)*4096);	
	}

	int line_num = 0;
	uint64_t *seq_bin_id = NULL;
	uint64_t *seq_bin_id_cr = NULL, *seq_bin = NULL;
	long int i = 0;
	int repeat = 1;
	int reads = 0;
	bool no_N = false;
	int bp_added = 0, index = 0;
	uint64_t incrementor;
	
	/*This loop is getting read information and putting it back in the buf_2*/
	while (get_data(&buf_1, &buf_2, file_1, file_2, arg->interleaved_input)) {
		
		bp_added = 0;
		index = 0;
		incrementor = 1;

		seq_bin_id = (uint64_t *)malloc(sizeof(uint64_t) * size);
		seq_bin_id_cr = (uint64_t *)malloc(sizeof(uint64_t) * size);

		for (int clean = 0; clean < size; clean++) {
			seq_bin_id[clean] = 0;
			seq_bin_id_cr[clean] = 0;
		}
		
		no_N = converter(buf_1[1], arg->start, arg->length, seq_bin_id, bp_added, index, incrementor, false);
	
		/*make sure you want R1 and R2 files*/
		if (file_2 != NULL || arg->interleaved_input) {
			no_N = converter(buf_2[1], arg->start, arg->length, seq_bin_id, bp_added, index, incrementor, true);
		}

		bp_added = 0;
		index = 0;
		incrementor = 1;


		if (file_2 != NULL || arg->interleaved_input) {
			no_N = converter(buf_2[1], arg->start, arg->length, seq_bin_id_cr, bp_added, index, incrementor, false);
		}
		
		no_N = converter(buf_1[1], arg->start, arg->length, seq_bin_id_cr, bp_added, index, incrementor, true);
		
		seq_bin = Greater_Than(seq_bin_id, seq_bin_id_cr, size); 
		//printf("%" PRIu64 "\n", seq_bin[0]);

		/*If there are some N's in the sequences id ignore it*/

		if (no_N) { 
			if (arg->mem_saving) {
				reads = x->Reads_Add_Tree_Public(seq_bin, buf_1[0], buf_1[1], buf_1[3], buf_2[0], buf_2[1], buf_2[3], f_read1, f_read2, arg->quality_checking, size);
			} else {
				reads = x->Reads_Add_Tree_Public(seq_bin, buf_1[0], buf_1[1], buf_1[3], buf_2[0], buf_2[1], buf_2[3], arg->quality_checking, size);
			}
		} else {
			x->Discarded();
		}

		free(seq_bin_id);
		seq_bin_id = NULL;
		free(seq_bin_id_cr);
		seq_bin_id_cr= NULL;
		
		
		if (arg->verbose) {
			if (reads % 1000000 == 0) {
					x->Display_Info((clock() - time_start)/CLOCKS_PER_SEC);
			}
		}


		i++;
		reads++;

		sprintf(buf_1[1], "\0");
		sprintf(buf_2[1], "\0");
	}


}

/* gzipped are most effinectly handled by opening a pipe
 * with zcat*/
FILE *gzip_open(char *f) {

	/*Opens zcat*/
	const char *zcat_cmd = "gunzip -c ";
	
	char *cmd = (char *)malloc(sizeof(char) * (strlen(f) + strlen(zcat_cmd) + 1));
	sprintf(cmd, "%s%s", zcat_cmd, f);
	

	FILE * piped_file = popen(cmd, "r");

	free(cmd);

	return piped_file;

}


void unzip_file(Binary_Search_Tree_Read_1_Read_2 *x, char *ifile1, char *ifile2, args *arg, FILE *f_read1, FILE *f_read2, double time_start) {

	int check;

	FILE *file_1 = NULL, *file_2 = NULL;

	/*calculates size of array need*/

	/*Check is 1 for gzip, 0 for fastq, and -1 for empty file*/
	if (strcmp(ifile1, "stdin") == 0) {
		file_1 = stdin;
	} else {
		check = gzipped_File(ifile1);
		
		if (check == 1) {
			file_1 = gzip_open(ifile1);
		} else if (check == 0) {
				file_1 = fopen(ifile1, "r");
		} else {
			fprintf(stderr, "File named %s is empty\n", ifile1);
		}
		
		/*single versus read 1 and read 2*/	
		if (ifile2 != NULL) {
			check = gzipped_File(ifile2);

			if (check == 1) {
				file_2 = gzip_open(ifile2);
			} else if (check == 0) {
				file_2 = fopen(ifile2, "r");
			} else {
				fprintf(stderr, "File named %s is empty\n", ifile2);
		}
		
		}
	}
	/*This is the read-in function of the program*/
	Fill_In_Binary_Tree(x, file_1, file_2, arg, f_read1, f_read2, time_start);

}

/*returns false if there is a N or true if it is sucessfuli
 *creates binary number for sequence on a two bit format
 * A - 00 (0)
 * T - 11 (3)
 * C - 10 (2)
 * G - 01 (1)
 * */

/*Possible enhancement 
 * - using x86 intrisic functions to read in characters and making the converation*/

bool converter(char *test, int start, int len, uint64_t *seq_bin_id, int &bp_added, int &index, uint64_t &incrementor, bool cr) {


	/*Values for corresponding values*/
	int A = 0;
	int C = 2;
	int G = 1;
	int T = 3;


	if (cr) {
		A = 3;
		T = 0;
		C = 1;
		G = 2;
	}

	int i = start -1;
	int end = start + len - 1;
	int length = strlen(test) - 1;

/*Ensures proper numbers are assigned (positive numbers and that it doesn't fall off the end)*/
	if (i > end) {
		fprintf(stderr, "[ERROR] out of bounds. Start: %i End: %i Length: %i\n", start, end, length);
		exit(-1);
	}

	if (i < 0) {
		fprintf(stderr, "[ERROR] out of bounds. Start: %i End: %i Length: %i\n", start, end, length);
		exit(-1);
		i = 0;
	} else if ( i > length -1) {
		fprintf(stderr, "[ERROR] out of bounds. Start: %i End: %i Length: %i\n", start, end, length);
		exit(-1);
	}

	if (end > length) {
		fprintf(stderr, "[ERROR] out of bounds. Start: %i End: %i Length: %i\n", start, end, length);
		exit(-1);
	} else if (end < 1) {
		fprintf(stderr, "[ERROR] out of bounds. Start: %i End: %i Length: %i\n", start, end, length);
		exit(-1);
	}

	if (!cr) {	
		for (i = start - 1; i < end; i++) {
			/*Creates the 'binary number'*/
			if (test[i] == 'A') {
				seq_bin_id[index] += (A * incrementor);
			} else if (test[i] == 'C') {
				seq_bin_id[index] += (C * incrementor);
			} else if (test[i] == 'T') {
				seq_bin_id[index] += (T * incrementor);
			} else if (test[i] == 'G') {
					seq_bin_id[index] += (G * incrementor);
			} else if (test[i] == 'N') {
				/*Tells if there is a bad case*/
				return false;
			}
			/*Shifts bits by 3 to create the binary number*/
			incrementor *= 4;
			/*prevent from overflow 2^64 is the highest 100% percision mark*/
	
			if (bp_added == BPS_ADDED) {
				bp_added = 0;
				index++;
				incrementor = 1;
				} else {
				bp_added++;
			}
		}
	} else {
		
		for (i = end-1; i > start-2; i--) {
			/*Creates the 'binary number'*/
			if (test[i] == 'A') {
				seq_bin_id[index] += (A * incrementor);
			} else if (test[i] == 'C') {
				seq_bin_id[index] += (C * incrementor);
			} else if (test[i] == 'T') {
				seq_bin_id[index] += (T * incrementor);
			} else if (test[i] == 'G') {
					seq_bin_id[index] += (G * incrementor);
			} else if (test[i] == 'N') {
				/*Tells if there is a bad case*/
				return false;
			}
			/*Shifts bits by 3 to create the binary number*/
			incrementor *= 4;
			/*prevent from overflow 2^64 is the highest 100% percision mark*/
	
			if (bp_added == BPS_ADDED) {
				bp_added = 0;
				index++;
				incrementor = 1;
			} else {
				bp_added++;
			}
		}
	}
		

	/*no Ns*/
	return true;

}


/*Make sures the files actually exist*/

void Check_Permissions(char *fname, int permissions) {

	if (access(fname, F_OK) != -1 ) {
		return;
	} else {
		fprintf(stderr, "Do not have proper permissions to access this file %s\n", fname);
		exit(-10);
	}

}


/*Counter made for strtok_r function to understand how much room is needed*/
int Count_String(char *file_string, char character) {
	int i = 0, count = 0;
	while (file_string[i] != '\0') {
		if (file_string[i] == character) {
			count++;
		}
		i++;
	}
	return count + 1;
	
}


/* Split Function */
int Parse_Comma_Separation(char *file_string, char ***arg_string) {

	int number_of_string = Count_String(file_string, ',');
	char * tok, *tmp, **holder_arg;
	int i = 0, count = 0;

	holder_arg = (char **)malloc(sizeof(char *) * number_of_string);
	
	if (file_string != NULL) {
		tok = strtok_r(file_string, ",", &file_string);

		while (tok != NULL) {
			//Check_Permissions(tok, R_OK);
			holder_arg[i] = strdup(tok);
			i++;
			tok = strtok_r(NULL, ",", &file_string);
			count++;
		}
	
		if (count != number_of_string) {
			fprintf(stderr, "Please no spaces in command line arguments\n");
			exit(-1);
		}	
		
	} else {
		fprintf(stderr, "Please, specify the files in -1, -2, or -U\n");
		exit(-10);
	} 
	*arg_string = holder_arg;	
	
	return number_of_string;
}


/*Creates name of the output file (gz)*/	
void Concat_File_Names(char **outputfile, const char * suffix, bool gzip) {
	char *correct_output = (char *)malloc(sizeof(char) * (strlen(suffix)+strlen(*outputfile)) + 4);

	strcpy(correct_output, *outputfile);
	strcpy(&correct_output[strlen(correct_output)], suffix);
	
	if (gzip) {
		strcpy(&correct_output[strlen(correct_output)], ".gz");
	}
	free(*outputfile);
	*outputfile = correct_output;
}

/*Main function hads argv and argc here*/
args *Arguements_Collection(int argc, char *argv[]) {


	/*Get opt long varaibles*/
        int cmd_line_char;
        extern char *optarg;
        extern int optind;
        extern int optopt;
        extern int opterr;
	int long_index;

	/*Creates args struct*/
	args *program_args = (args *)malloc(sizeof(args));
	Start_Args(program_args);
       

	/*Check and see what is used (R1 and R2, Single End Read, or Interleaved Input*/
	bool read1 = false, read2 = false, single = false;
	program_args->output_stdout = false;

	/*Arguments for super_deduper*/
	const struct option longopts[] =
        {
                {"read1", required_argument, 0, '1'},
                {"read2", required_argument, 0, '2'},
                {"singleend", required_argument, 0, 'U'},
                {"output-tree", required_argument, 0, 'O'},
                {"input-tree", required_argument, 0, 'I'},
                {"output-prefix", required_argument, 0, 'p'},
                {"start", required_argument, 0, 's'},
                {"length", required_argument, 0, 'l'},
                {"memory-saving-on", required_argument, 0, 'M'},
                {"quality-check-off", required_argument, 0, 'q'},
                {"verbose", required_argument, 0, 'v'},
                {"debug", required_argument, 0, 'd'},
                {"interleaved-input", required_argument, 0, 'i'},
                {"interleaved-output", required_argument, 0, 'o'},
        	{"gzip-output", required_argument, 0, 'g'},
		{"help", required_argument, 0, 'h'},
                {0, 0, 0, 0}
        };

	/*Parser of command lines*/
        while ((cmd_line_char = getopt_long(argc, argv, "1:2:p:s:l:d:MqvU:O:I:hi:og", longopts, &long_index)) != EOF) {

                switch(cmd_line_char) {
                        case '1':
				program_args->read_1_files = Parse_Comma_Separation(optarg, &(program_args->read_1));
				read1 = true;
                                break;
                        case '2':
				program_args->read_2_files = Parse_Comma_Separation(optarg, &(program_args->read_2));
				read2 = true;
                                break;
                        case 'p':
				if (strcmp(optarg, "stdout") == 0) {
					program_args->output_stdout = true;
				}

				program_args->output_filename_1 = strdup(optarg);	
				program_args->output_filename_2 = strdup(optarg);	
                                break;
                        case 's':
				program_args->start = atoi(optarg);
                                break;
                        case 'M':
				program_args->mem_saving = true;
                                break;
                        case 'q':
				program_args->mem_saving = true;
                             	program_args->quality_checking = false;
                                break;
                        case 'l':
				program_args->length = atoi(optarg);
                                break;
                        case 'v':
				program_args->verbose = true;
                                break;
                        case 'd':
                               // debug_file = strdup(optarg);
                            //    DEBUG = true;
                                break;
                        case 'U':
				program_args->single_files = Parse_Comma_Separation(optarg, &(program_args->single_reads));
				single = true;
                                break;
                        case 'I':
				program_args->tree_files = Parse_Comma_Separation(optarg, &(program_args->input_trees));
                                break;
                        case 'O':
				program_args->output_tree = strdup(optarg);
                                break;
			case 'o':
				program_args->interleaved_output = true;
				break;
			case 'i':
				program_args->interleaved_files = Parse_Comma_Separation(optarg, &(program_args->interleaved));
				program_args->interleaved_input = true;
				break;
			case 'g':
				program_args->gzip_output = true;
				break;
                        case 'h':
                             	fprintf(stderr, "%s", usage_string);
                             	exit(0);
                                break;
                        case '?':
				fprintf(stderr, "%s\n", optarg);
                                fprintf(stderr, "-%c is not a valid argument\n", cmd_line_char);
                                break;
			
                }
        }


	/*Error handling to make sure multiple formats are not created*/
	if (read1 && read2 && single && program_args->interleaved_input) {
		fprintf(stderr, "Please, specifiy either read 1 AND read 2 (-1 and -2)  OR single end read (-U) OR interleaved files (-i), not all multiple types\n");
		exit(-10);
	} else if (!read1 && !read2 && !single && !program_args->interleaved_input) {
		fprintf(stderr, "Please, specifiy either read 1 AND read 2 (-1 and -2)  OR single end read (-U) OR interleaved files (-i)\n");
		exit(-10);
	} else if (program_args->read_1_files != program_args->read_2_files) {
		fprintf(stderr, "Differing number of Read 1 and Read 2 files\n");
		exit(-10);
	}

	if (read1 && read2) {
		Concat_File_Names(&(program_args->output_filename_1), "_nodup_PE1.fastq", program_args->gzip_output);
		Concat_File_Names(&(program_args->output_filename_2), "_nodup_PE2.fastq", program_args->gzip_output);
	} else if (single) {
		Concat_File_Names(&(program_args->output_filename_1), "_nodup_PE1.fastq", program_args->gzip_output);
		free(program_args->output_filename_2);
		program_args->output_filename_2=NULL;
	} else if (program_args->interleaved_output) {
		Concat_File_Names(&(program_args->output_filename_1), "_nodup_PE1.fastq", program_args->gzip_output);
		free(program_args->output_filename_2);
		program_args->output_filename_2=NULL;
	} else { //interleaved input with r1 r2 output
		Concat_File_Names(&(program_args->output_filename_1), "_nodup_PE1.fastq", program_args->gzip_output);
		Concat_File_Names(&(program_args->output_filename_2), "_nodup_PE2.fastq", program_args->gzip_output);
	}


	if (program_args->interleaved_output && single) {
		fprintf(stderr, "You cannot have an interleaved output with single end reads\n");
		exit(-100);
	}

	if (read1 && read2 && single) {
		fprintf(stderr, "You cannot specify a single end read, along with read 1 and read 2\n");
		exit(-101);
	}

	return program_args;


}


int main(int argc, char *argv[]) {
	/*Set up arguements from user*/	
	args *program_args = Arguements_Collection(argc, argv);

	clock_t begin, end_c;
	double time_spent;
	
	begin = clock();


	/*Constructs Binary Search Tree*/
	Binary_Search_Tree_Read_1_Read_2 *x = new Binary_Search_Tree_Read_1_Read_2();
	
	/*Size must be calculated in order to see if tree is compatitable
 * 	  If single files then we won't multiiple by 2*/
	if (program_args->read_2_files != 0 || program_args->interleaved_input ) {
		size = (((program_args->length)*2)/BPS_ADDED)+1;
	} else {
		size = (((program_args->length))/BPS_ADDED)+1;
	}

	/*Gives tree size, gzip, boolean for output type*/
	x->Input_Size(size);
	x->Interleaved(program_args->interleaved_output);

	if (program_args->gzip_output) {
		x->Set_Gzipped();
	}

	/*Creates tree if that option is given*/
	for (int i = 0; i < program_args->tree_files; i++) {
		x->Create_Tree(program_args->input_trees[i], program_args->mem_saving);
	}
	

	/*Opens output files (in the case of memory eff version*/
	FILE *output_file_1 = NULL;
	FILE *output_file_2 = NULL;

	if (program_args->output_stdout) {
		output_file_1 = stdout;
		output_file_2 = NULL;
	} else if(program_args->interleaved_output) {
		output_file_1 = fopen(program_args->output_filename_1, "w");
		output_file_2 = NULL;
	} else if (program_args->output_filename_2 != NULL) {
		output_file_1 = fopen(program_args->output_filename_1, "w");
		output_file_2 = fopen(program_args->output_filename_2, "w");
	} else if (program_args->single_reads != NULL) {
		output_file_1 = fopen(program_args->output_filename_1, "w");
	}
	 
	/*Loops through all files given, regardless of type (types with non-given would just be 0 and the loop would not be entered)*/
	for (int i = 0; i < program_args->read_1_files; i++) {
		unzip_file(x, (program_args->read_1)[i], (program_args->read_2)[i], program_args, output_file_1, output_file_2, begin);
	}

	for (int i = 0; i < program_args->interleaved_files; i++) {
		unzip_file(x, (program_args->interleaved)[i], NULL, program_args, output_file_1, output_file_2, begin);
	}	

	for (int i = 0; i < program_args->single_files; i++) {
		unzip_file(x, (program_args->single_reads)[i], NULL, program_args, output_file_1, output_file_2, begin);
	}	

	
	/*Outputs files*/
	if (program_args->output_tree != NULL) {
		x->Write_Tree(program_args->output_tree);
	}

	if (!program_args->mem_saving) {
		x->Delete_And_Print(output_file_1, output_file_2);
	} else {
		x->Delete_Public();
	}

	
	end_c = clock();
	time_spent = (double)(end_c - begin) / CLOCKS_PER_SEC;
	x->Display_Info(time_spent);

	fclose(output_file_1);

	if (output_file_2 != NULL) {
		fclose(output_file_2);
	}

	/*Clean up*/
	if (!program_args->mem_saving) {
		delete x;
	}


}
