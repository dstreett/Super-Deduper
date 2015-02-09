
#include <string.h>
#include "Binary_Search_Tree_Memory_Eff_Node.h"

void Reads_Node_Eff::Add_Info(double *_seq_bin, int _sum_qual) {
	/*binary ID*/
	seq_bin = _seq_bin;
	sum_qual = _sum_qual;

}
void Reads_Node_Eff::Add_Info(double *_seq_bin, int _sum_qual, fpos_t _file_loc1, fpos_t _file_loc2) {
	/*binary ID*/
	seq_bin = _seq_bin;
	file_loc1 = _file_loc1;
	file_loc2 = _file_loc2;
	sum_qual = _sum_qual;

}

