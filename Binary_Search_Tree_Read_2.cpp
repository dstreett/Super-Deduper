#include <string.h>
#include "Binary_Search_Tree_Read_2.h"


int Sum_Qual_Score_2(char *qual) {

	int i = 0;
	double qual_sum = 0;
	while (qual[i] == '\0' || qual[i] == '\n') {
		qual_sum += (qual[i] - 33);
		i++;
	}

	return qual_sum;
}

void Read_Two_Node::Add_Info(long double _seq_bin, char *_id, char *_seq, char *_qual) {

        seq_bin = _seq_bin;
        seq = strdup(_seq);
        id = strdup(_id);
        qual = strdup(_qual);
//	str_length = strlen(_seq);
	sum_qual = Sum_Qual_Score_2(qual);
}

