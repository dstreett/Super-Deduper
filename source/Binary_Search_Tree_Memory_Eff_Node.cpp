#include <string.h>
#include "Binary_Search_Tree_Memory_Eff_Node.h"

void Reads_Node_Eff::Add_Info(int _sum_qual) {
	/*binary ID*/

	sum_qual = _sum_qual;

}

void Reads_Node_Eff::Add_Info(uint64_t *_seq_bin, int size) {
	if (sum_qual != -1) {
		/*binary ID*/
		seq_bin = (uint64_t *)malloc(sizeof(uint64_t) * size );
		memcpy(seq_bin, _seq_bin, sizeof(uint64_t) * size);
	}
}

void Reads_Node_Eff::Add_Info(uint64_t *_seq_bin, int size, bool tree_make) {
	/*binary ID*/
	seq_bin = (uint64_t *)malloc(sizeof(uint64_t) * size );
	memcpy(seq_bin, _seq_bin, sizeof(uint64_t) * size);
	
	/* This is a flag used to show this was added by the Make-Tree argument
 * 	   This means the read will not be outputed*/
	if (tree_make == true) {
		sum_qual = -1;
	}

}

void Reads_Node_Eff::Add_Info(uint64_t *_seq_bin, int _sum_qual, fpos_t _file_loc1, fpos_t _file_loc2, int size) {
	if (sum_qual != -1) {
		/*binary ID*/
		seq_bin = (uint64_t *)malloc(sizeof(uint64_t) * size );
		memcpy(seq_bin, _seq_bin, sizeof(uint64_t) * size);
	
		file_loc1 = _file_loc1;
		file_loc2 = _file_loc2;
		sum_qual = _sum_qual;
	}
}

