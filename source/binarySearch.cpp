

#include "binarySearch.h"


void BinarySearchTree::outputStats(FILE *f) {
    fprintf(f, "Reads_Read\tReads_Written\tReads_Discarded\tSingletons\tDoubles\tThree_Plus\tDisqualifed_Reads\tReplacements_Called\tReads_Per_Second\tTotal_Time\n");
    fprintf(f, "%llu\t%llu\t%llu\t%llu\t%llu\t%llu\t%llu\t%llu\t%f\t%u\n",
                reads_read,
                    nodesCreated,
                        dup_gone,
                           singletons,
                                 doubles,
                                       threeplus,
                                             disReads,
                                                    replaced,
                                                       (double)reads_read/(double)((double)time_end-(double)time_start),
                                                       time_end-time_start);
}


/*Print and Delete*/
void BinarySearchTree::PrintAndDelete(FileWriter *R1, FileWriter *R2, FileWriter *SE) {
    PrintAndDeletePrivate(root, R1, R2, SE);
}

void BinarySearchTree::PrintAndDeletePrivate(Node *n, FileWriter *R1, FileWriter *R2, FileWriter *SE) {
    if (n != NULL) {
        if (n->count == 1) {
            singletons++;
        } else if (n->count == 2) {
            doubles++;
        } else {
           threeplus++;
        }

        PrintAndDeletePrivate(n->left, R1, R2, SE); 
        PrintAndDeletePrivate(n->right, R1, R2, SE); 
     
        /*Only if Fastq file exists*/
        if (R2) {
            /*R1 and R2 printed Normal Fastq format*/
            if (n->R2) {

                R1->writeData(n->R1, NULL, NULL);
                R2->writeData(n->R2, NULL, NULL);
            } else {
                SE->writeData(n->R1, NULL, NULL);
            }
        } else {
            if (n->R2 || !SE ) {
                R1->writeData(n->R1, n->R2, NULL);
            } else {
                SE->writeData(n->R1, NULL , NULL);
            }
            
        }
    }
}

/*Goes throught the char * with a 33 offset of each character*/
uint32_t BinarySearchTree::qualSum(char *q) {
    uint32_t score = 0;
    uint16_t i = 0;
    /*Should end on a null character, no tab or newlines*/
    while (q[i] != '\0') {
        /*Allows error checking*/
        if (q[i] > '~' || q[i] < '!') {
            fprintf(stderr, "Quality score is not between ascii [33,126], or [\",~]\n");
            fprintf(stderr, "Bad quality string = %s\n", q);
            fprintf(stderr, "Bad character='%c'\n", q[i]);
            exit(12);
        } else {
            score += q[i]-33;
        }
        i++;
    }

    return score;
}

/*Flip bits functionality for character string*/
bool BinarySearchTree::FlipBitsChars(readInfo *R1, readInfo *R2, uint16_t **id) {

    uint16_t loc = start;
    char *seq_1, *seq_2;
    
    /*Incementor for id array*/
    uint16_t idLoc = 0, bitShifts = 0;

    seq_1 = R1->getSeq();

    if (R2) {
        seq_2 = R2->getSeq();
    } else {
        seq_2 = NULL;
    }


    /*Another error check for the humans*/
    if (strlen(seq_1) < start+charLength) {
        fprintf(stderr, "Error within binarySearch.cpp in funciton FlipBitsChars() strlen of R1 larger than start + length\n");
        fprintf(stderr, "String Len = %lu, start = %d, length = %d\n", strlen(seq_1), start, charLength);
        disReads++;
        return false;
    }

    char *seq = seq_1;
    
    while (loc < start+charLength) {
        uint16_t tmp;
        if (seq[loc] == 'A') {
            tmp = A;
        } else if (seq[loc] == 'T') {
            tmp = T;
        } else if (seq[loc] == 'G') {
            tmp = G;
        } else if (seq[loc] == 'C') {
            tmp = C;
        } else if (seq[loc] == 'N') {
            disReads++;
            return false;
        } else {
            fprintf(stderr, "In binarySearch.cpp in funciton FlipBitsChar() Bad character in R1\n");
            fprintf(stderr,"Bad character string = %s Bad Char = %c\n", seq, seq[loc]);
            exit(27);
        }

        (*id)[idLoc] ^= (tmp << bitShifts);
        
        bitShifts += 2;

        if (bitShifts == 16) {
            bitShifts = 0;
            idLoc++;
            (*id)[idLoc] = 0;
        }
        loc++;
    }
    /*Bit flips for the R2 if needed*/
    if (R2 == NULL) {
    /*This is totally fine, SE reads*/
    } else {
        loc = start;
        if (strlen(seq_1) < start+charLength) {
            fprintf(stderr, "Error within binarySearch.cpp in funciton FlipBitsChars() strlen of R2 larger than start + length\n");
            fprintf(stderr, "String Len = %lu, start = %d, length = %d\n", strlen(seq_1), start, charLength);
            disReads++;
            return false;
        }
        
        char *seq = seq_2;

        while (loc < start+charLength) {
            uint16_t tmp;
            if (seq[loc] == 'A') {
                tmp = A;
            } else if (seq[loc] == 'T') {
                tmp = T;
            } else if (seq[loc] == 'G') {
                tmp = G;
            } else if (seq[loc] == 'C') {
                tmp = C;
            } else if (seq[loc] == 'N') {
                disReads++;
                return false;
            } else {
                fprintf(stderr, "In binarySearch.cpp in funciton FlipBitsChar() Bad character in R2\n");
                fprintf(stderr,"Bad character string = %s Bad Char = %c\n", seq, seq[loc]);
                exit(26);
            }

            (*id)[idLoc] ^= (tmp << bitShifts);
            
            bitShifts += 2;

            if (bitShifts == 16) {
                bitShifts = 0;
                idLoc++;
                (*id)[idLoc] = 0;
            }
            loc++;
        }
    }

    idLoc++;
    for (int i = idLoc; i < mallocLength; i++) {
        (*id)[i] = 0;
    }

    return true;

}


/*this one will allocate memory, that is why i'm not return a uint16 * and passing it by ref*/
/*This creates the id that the binary search tree will use to dive down the tree*/
bool BinarySearchTree::getID(readInfo *R1, readInfo *R2, uint16_t **id) {
    /*3 bit * length / 16 this was handled constructur*/
    if (!R1 and !R2) {
        fprintf(stderr, "Both reads within binarySearch.cpp function getID() are NULL\n");
        fprintf(stderr, "R1 CANNOT be NULL\n");
        exit(15);
    }

    /*Single end read*/
    if (R2 == NULL) {
        (*id) = (uint16_t *)malloc(mallocLength);
    } else {
        /*Lenght for both read 1 and two*/
        (*id) = (uint16_t *)malloc(mallocLength*2);
    }



    /*Not funcitonal yet*/
    if (R1->optimized) {
        fprintf(stderr, "Optimizmations no funciotnal yet\n");
        exit(-1);
    } else {
        return FlipBitsChars(R1, R2, id);
    }
    return false;
}




/*Greather than return 1 (true), 0 for equal, and -1 for less than*/
/* 1 Greater Than
 * 0 Equal To
 * -1 Less Than*/

int BinarySearchTree::GreaterThan(uint16_t *test, uint16_t *value) {
    for (int i = 0; i < mallocLength; i++) {
        if (test[i] > value[i]) {
            return 1;
        } else if (test[i] < value[i]) {
            return -1;
        }
    }
    return 0;
}



/*Recursive function to add Node*/
void BinarySearchTree::PrivateAddNode(Node **n, readInfo *R1_, readInfo *R2_, uint16_t *id, uint32_t qualScore ) {
    /*Add Node condition*/
    int tmpValue;
    if ((*n) == NULL) {
        nodesCreated++;
        (*n) = new Node(R1_, R2_, id, qualScore);

        (*n)->count = 1;
        return;
    } else if ((tmpValue = GreaterThan(id, (*n)->id)) == 1) {
        PrivateAddNode(&((*n)->left), R1_, R2_, id, qualScore);
    } else if (tmpValue == -1) {
        PrivateAddNode(&((*n)->right), R1_, R2_, id, qualScore);
    /*Nodes are equal*/
    } else {
        /*Makes sure that single ends are kept track of*/

        if ((!R2_ && ((*n)->single)) || (R2_ && !(*n)->single)) {
            (*n)->count++;
            dup_gone++;
            if (qualScore > (*n)->qualScore) {
                replaced++;
                (*n)->Replace(R1_, R2_, qualScore);
                /*exits*/
            } else {
                delete R1_;
                delete R2_;
            }
        /*If node is single end but the read isn't*/
        } else if (R2_) {
            PrivateAddNode(&((*n)->left), R1_, R2_, id, qualScore);
        } else if (!R2_) {
            PrivateAddNode(&((*n)->right), R1_, R2_, id, qualScore);
        }
    }

} 
void BinarySearchTree::AddNode(readInfo *R1_, readInfo *R2_) {

    uint16_t *id;
    uint32_t qualScore = 0;
    reads_read++;


    if (!getID(R1_, R2_, &id)) {
        disReads++;
        return;
    } 
    /*R1 will never be null, but checking incase something goes horriblely wrong*/
    if (R1_ != NULL) {
        if (qualCheck) { 
            qualScore += qualSum((R1_)->getQual());
        }
    } else {
        fprintf(stderr, "Read 1 should never be NULL within BinarySearchTree.cpp PrivateAddNode()\n");
        exit(11);
    }

    if (R2_ != NULL) {
        if (qualCheck) {
            qualScore += qualSum((R2_)->getQual());
        }
    } else {
        /*This just means it is single end reads*/
        /*This is acceptable, no error message*/
    }
    PrivateAddNode(&root, R1_, R2_, id, qualScore);

}
