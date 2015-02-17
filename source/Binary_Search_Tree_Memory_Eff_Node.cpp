#include <string.h>
#include "Binary_Search_Tree_Memory_Eff_Node.h"

void Reads_Node_Eff::Add_Info(int _sum_qual) {
	/*binary ID*/

	sum_qual = _sum_qual;

}

void Reads_Node_Eff::Add_Info(uint64_t *_seq_bin, int size) {

	/*binary ID*/
	seq_bin = (uint64_t *)malloc(sizeof(uint64_t) * size );
	memcpy(seq_bin, _seq_bin, sizeof(uint64_t) * size);

}

void Reads_Node_Eff::Add_Info(uint64_t *_seq_bin, int _sum_qual, fpos_t _file_loc1, fpos_t _file_loc2, int size) {
	/*binary ID*/
	seq_bin = (uint64_t *)malloc(sizeof(uint64_t) * size );
	memcpy(seq_bin, _seq_bin, sizeof(uint64_t) * size);

	file_loc1 = _file_loc1;
	file_loc2 = _file_loc2;
	sum_qual = _sum_qual;

}

