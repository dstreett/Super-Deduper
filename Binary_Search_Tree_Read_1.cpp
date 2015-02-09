
#include <string.h>
#include "Binary_Search_Tree_Read_1.h"


int Sum_Qual_Score_1(char *qual) {

	int i = 0;
	double qual_sum = 0;
	while (qual[i] == '\0' || qual[i] == '\n') {
		qual_sum += (qual[i] - 33);
		i++;
	}

	return qual_sum;
}

bool Read_One_Node::In_LinkedList(long double seq_bin) {
	return ll_R2->Is_In_LinkedList(seq_bin);

}

Read_Two_Node *Read_One_Node::Pop() {

	return ll_R2->Pop();
	
}
void Read_One_Node::Add_Info(long double _seq_bin, char *_id, char *_seq, char *_qual) {

	seq_bin = _seq_bin;
	seq = strdup(_seq);
	id = strdup(_id);
	qual = strdup(_qual);
	sum_qual = Sum_Qual_Score_1(qual);
//	str_length = strlen(_seq);
	count++;
}

/*Add the read two the the array of ptr to the nodes
 * this is point to the other binary search tree*/

bool Read_One_Node::Add_Read_2_Ptr(Read_Two_Node **r2_nodes) {
	read_2_ptrs_count++;

	return ll_R2->AddNode_Order(*r2_nodes);
/*	printf("here ptr\n");

	if (read_2_ptrs_count == 1) {
		Read_2s = (Read_Two_Node **)malloc(sizeof(Read_Two_Node *) * read_2_ptrs_count); // Do not cast malloc in C
		Read_2s[read_2_ptrs_count - 1] = *r2_nodes;
	} else {
		printf("here ptr\n");
		Read_Two_Node **Read_2s_tmp = (Read_Two_Node **)realloc(Read_2s, sizeof(Read_Two_Node *) * read_2_ptrs_count); // Do not cast malloc in C

		if (Read_2s_tmp == NULL) {
			printf("Something went horribly, horribly wrong [realloc failed]\n");
			exit(-2);
		} else {
			Read_2s = Read_2s_tmp;
			Read_2s[read_2_ptrs_count - 1] = *r2_nodes;
		//	free(Read_2s_tmp);
		}
		
	
	} 
*/
}
