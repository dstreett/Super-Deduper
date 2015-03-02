#include "Binary_Search_Tree_Reads_Node.h"

#include <string.h>

/*
void Copy_Ptr(uint64_t *copyto, uint64_t *copy, int size) {

	for (int i = 0; i < size; i++) {
		copyto[i] = copy[i];
	}
}
*/

void Reads_Node::Add_Info(uint64_t *_seq_bin, int size, bool tree_making) {
	/*binary ID*/
	seq_bin = (uint64_t *)malloc(sizeof(uint64_t) * size );
	memcpy(seq_bin, _seq_bin, sizeof(uint64_t) * size);

	sum_qual = -1;
}

void Reads_Node::Add_Info(uint64_t *_seq_bin, char *_id_1, char *_seq_1, char *_qual_1, char *_id_2, char *_seq_2, char *_qual_2, int _sum_qual, int size) {
	if (sum_qual != -1) {
		/*binary ID*/
		seq_bin = (uint64_t *)malloc(sizeof(uint64_t) * size );
		memcpy(seq_bin, _seq_bin, sizeof(uint64_t) * size);
	
	//		seq_bin = _seq_bin;
		/*read 1*/
		seq_1 = strdup(_seq_1);
		id_1 = strdup(_id_1);
		qual_1 = strdup(_qual_1);
		/*read 2*/
		seq_2 = strdup(_seq_2);
		id_2 = strdup(_id_2);
		qual_2 = strdup(_qual_2);

		sum_qual = _sum_qual;
	}

}
void Reads_Node::Add_Info(char *_id_1, char *_seq_1, char *_qual_1, char *_id_2, char *_seq_2, char *_qual_2, int _sum_qual) {
	if (sum_qual != -1) {
		/*read 1*/
		seq_1 = strdup(_seq_1);
		id_1 = strdup(_id_1);
		qual_1 = strdup(_qual_1);
		/*read 2*/
		seq_2 = strdup(_seq_2);
			id_2 = strdup(_id_2);
		qual_2 = strdup(_qual_2);
	
		sum_qual = _sum_qual;
	}

}

