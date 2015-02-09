#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "Binary_Search_Tree.h"
#include <getopt.h>
#include <time.h>

bool converter(char *test, int start, int end, double *seq_bin_id, int &bp_added, int &index, int &size);

void unzip_file(Binary_Search_Tree_Read_1_Read_2 *x, char *ifile1, char *ifile2,  int start, int end) {
        const char zcat[] = "zcat ";
        char *command_1, *command_2;
        FILE *unzipped_out_1, *unzipped_out_2;
        char buf_1[4][4096];
        char buf_2[4][4096];
	int line_num = 0;
	
       command_1 = (char *)malloc(sizeof(zcat) + strlen(ifile1) + 1);
       command_2 = (char *)malloc(sizeof(zcat) + strlen(ifile2) + 1);

        sprintf(command_1, "%s%s", zcat, ifile1);
	sprintf(command_2, "%s%s", zcat, ifile2);

        unzipped_out_1 = popen(command_1, "r");
        unzipped_out_2 = popen(command_2, "r");
	double *seq_bin_id = (double *)malloc(0);
	int i = 0;
	int repeat = 1;
	int tmp = 0;
	seq_bin_id[0] = 0;
	seq_bin_id[1] = 0;

	bool is_N = false;
	int bp_added = 0, index = 0, size = 1;

        while ((fgets(buf_1[line_num], 4096, unzipped_out_1) != NULL) && (fgets(buf_2[line_num], 4096, unzipped_out_2) != NULL)) {

		if (line_num == 3) {
			bp_added = 0;
			index = 0;
			size = 2;
			seq_bin_id = (double *)malloc(sizeof(double) * size);
			seq_bin_id[0] = 0;
			seq_bin_id[1] = 0;
			is_N = converter(buf_1[1], start, end, seq_bin_id, bp_added, index, size);
			is_N = converter(buf_2[1], start, end, seq_bin_id, bp_added, index, size);
			
			x->Reads_Add_Tree_Public(seq_bin_id, buf_1[0], buf_2[0], buf_1[1], buf_2[1], buf_1[3], buf_2[3]);
			tmp++;

			if (tmp % 1000000 == 0) {
				printf("%i\n", tmp);
			}

		}

		i++;
//		printf("%s\n", buf_1[2]);
		line_num = (line_num + 1) % 4;

	
	}

	free(seq_bin_id);
	fclose(unzipped_out_1);
	fclose(unzipped_out_2);

}

/*returns false if there is a N or true if it is sucessful*/
bool converter(char *test, int start, int end, double *seq_bin_id, int &bp_added, int &index, int &size) {

	long double A = 0;
	long double C = 2;
	long double G = 1;
	long double T = 3;
//	int i = strlen(test) - 1;
	int i = start -1;
	end = end - 1;
	int length = strlen(test);
	if (i > end) {
		printf("[ERROR] out of bounds. Start: %i End: %i Length: %i\n", start, end, length);
		exit(-1);
	}

	if (i < 0) {
		printf("[ERROR] out of bounds. Start: %i End: %i Length: %i\n", start, end, length);
		exit(-1);
		i = 0;
	} else if ( i > strlen(test) -1) {
		printf("[ERROR] out of bounds. Start: %i End: %i Length: %i\n", start, end, length);
		exit(-1);
	}

	if (end > length) {
		printf("[ERROR] out of bounds. Start: %i End: %i Length: %i\n", start, end, length);
		exit(-1);
		//end = length;
	} else if (end < 1) {
		printf("[ERROR] out of bounds. Start: %i End: %i Length: %i\n", start, end, length);
		exit(-1);
	}
	
	long double incrementor = 1;
	bool cont = true;

	while (cont) {
		/* remalloc memory - execept after the first malloc */

		if (bp_added == 0 && index >= size - 1) {
			/*Increment index so that when reallocing the proper size*/
			size++;
			seq_bin_id = (double *) realloc(seq_bin_id, sizeof(double)*(size+1));
			seq_bin_id[size] = 0;
		} else if (bp_added == 0) {
			seq_bin_id[index] = 0;
		}

		if (test[i] == 'A') {
			seq_bin_id[index] += A * incrementor;
		} else if (test[i] == 'C') {
			seq_bin_id[index] += C * incrementor;
		} else if (test[i] == 'T') {
			seq_bin_id[index] += T * incrementor;
		} else if (test[i] == 'G') {
			seq_bin_id[index] += G * incrementor;
		} else if (test[i] == 'N') {
			return false;
		} else if (test[i] == '\n' || test[i] == '\0') {
			cont = false;
		}

		if (end == i) {
			cont = false;
		}

		/*prevent from overflow 2^53 is the highest 100% percision mark*/
		if (bp_added < 25) {
			bp_added++;
		} else {
			bp_added = 0;
			index++;
		}


		incrementor *= 4;
		i++;
		
	}
	

	return true;

}

int main(int argc, char *argv[]) {

	clock_t begin, end_c;
	double time_spent;
	
	begin = clock();
	int cmd_line_char;
        extern char *optarg;
        extern int optind;
        extern int optopt;
        extern int opterr;

        char *fin_name_R1 = NULL;
        char *fin_name_R2 = NULL;
	char *fout_prefix_R1 = NULL;
	char *fout_prefix_R2 = NULL;

	int start = 10;
	int length = 25;
        FILE *sam_file;
        FILE *output_file;
        int cores = 1;
        int long_index;
        bool to_reference = false;
        bool proper_alligment = true;
	bool fasta_gz = true;
	

        const struct option longopts[] =
        {
                {"read1", required_argument, 0, '1'},
                {"read2", required_argument, 0, '2'},
                {"output", required_argument, 0, 'o'},
                {"start", required_argument, 0, 's'},
		{"length", required_argument, 0, 'l'},
		{"fasta", required_argument, 0, 'f'},
		{"fastagz", required_argument, 0, 'g'},
                {0, 0, 0, 0}
        };


        while ((cmd_line_char = getopt_long(argc, argv, "1:2:o:s:l:fg", longopts, &long_index)) != EOF) {
                switch(cmd_line_char) {
                        case '1':
                                fin_name_R1 = strdup(optarg);
                                break;
                        case '2':
                                fin_name_R2 = strdup(optarg);
                                break;
			case 'o':
				fout_prefix_R1 = strdup(optarg);
				fout_prefix_R2 = strdup(optarg);
				break;
			case 's':
				start = atoi(optarg);
				break;
			case 'l':
				length = atoi(optarg);
				break;
			case 'f':
				fasta_gz = false;	
				break;
			case 'g':
				fasta_gz = true;
				break;
                        case '?':
                                printf("-%c is not a valid argument\n", cmd_line_char);
                                break;
                }
        }

	int end = start + length;

	Binary_Search_Tree_Read_1_Read_2 *x = new Binary_Search_Tree_Read_1_Read_2();
	char *tok_R1 = NULL, *tok_R2 = NULL;
	
	if (fin_name_R1 != NULL && fin_name_R2 && fout_prefix_R1 != NULL) {
			
		tok_R1 = strtok_r(fin_name_R1, ",", &fin_name_R1);
 		tok_R2 = strtok_r(fin_name_R2, ",", &fin_name_R2);

		while (tok_R1 != NULL && tok_R2 != NULL) { 
				unzip_file(x, tok_R1, tok_R2, start, end);
				tok_R1 = strtok_r(NULL, ",", &fin_name_R1);
				tok_R2 = strtok_r(NULL, ",", &fin_name_R2);
		}
	}

	const char *fout_suffix_R1 = "_nodup_PE1.fastq\0";
	const char *fout_suffix_R2 = "_nodup_PE2.fastq\0";


	char *fout_name_R1 = (char*)malloc(	(strlen(fout_prefix_R1) + strlen(fout_suffix_R1) + 10) * sizeof(char *));
	fout_name_R1[0] = '\0';
	strcat(fout_name_R1, fout_prefix_R1);
	strcat(fout_name_R1, fout_suffix_R1);

	char *fout_name_R2 = (char*)malloc(strlen(fout_prefix_R2) + strlen(fout_suffix_R2) + 10);
	fout_name_R2[0] = '\0';
	strcat(fout_name_R2, fout_prefix_R2);
	strcat(fout_name_R2, fout_suffix_R2);
	
	FILE *output_file_1 = fopen(fout_name_R1, "w");
	FILE *output_file_2 = fopen(fout_name_R2, "w");
        
	end_c = clock();
	time_spent = (double)(end_c - begin) / CLOCKS_PER_SEC;

	x->Delete_And_Print(output_file_1, output_file_2, fasta_gz, time_spent);

	fclose(output_file_1);
	fclose(output_file_2);
	
	free(fout_name_R1);
	free(fout_name_R2);
	delete x;


}
