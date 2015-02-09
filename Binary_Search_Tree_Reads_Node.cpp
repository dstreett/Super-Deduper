
#include <string.h>
#include "Binary_Search_Tree_Reads_Node.h"


int Sum_Qual_Score_1(char *qual) {

	int i = 0;
	double qual_sum = 0;
	while (qual[i] == '\0' || qual[i] == '\n') {
		qual_sum += (qual[i] - 33);
		i++;
	}

	return qual_sum;
}

void Reads_Node::Add_Info(double *_seq_bin, char *_id_1, char *_seq_1, char *_qual_1, char *_id_2, char *_seq_2, char *_qual_2, int _sum_qual) {
	/*binary ID*/
	seq_bin = _seq_bin;
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
