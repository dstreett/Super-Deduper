
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
class Reads_Node {

	public:
		uint64_t *seq_bin;
		char *seq_1, *seq_2;
		char *id_1, *id_2;
		char *qual_1, *qual_2;
		int sum_qual;	

		Reads_Node *left, *right;		

		void Add_Info(char *_id_1, char *_seq_1, char *_qual_1, char *_id_2, char *_seq_2, char *_qual_2, int _sum_qual);
		void Add_Info(uint64_t *_seq_bin, char *_id_1, char *_seq_1, char *_qual_1, char *_id_2, char *_seq_2, char *_qual_2, int _sum_qual, int size);
	
		Reads_Node () {
			seq_bin = NULL;
			seq_1 = NULL;
			id_1 = NULL;
			qual_1 = NULL;
			seq_2 = NULL;
			id_2 = NULL;
			qual_2 = NULL;


			left = NULL;
			right = NULL;
		}
	

		~Reads_Node() {
			free(seq_bin);
			free(seq_1);
			free(id_1);
			free(qual_1);
			free(seq_2);
			free(id_2);
			free(qual_2);
		}
};

