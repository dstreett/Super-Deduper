

#include "binarySearch.h"


void BinarySearchTree::outputStats(FILE *f) {

    const char *print_names[10] = {"Reads", "Written", "Discarded", "Singletons", "Doubles", "Three_Plus", "Disqualified", \
                                    "Replacements", "Reads/Sec", "Total_Time(s)"};

    uint64_t print_values[8] = {reads_read, nodesCreated, dup_gone, singletons, doubles, threeplus, disReads, replaced};

    for (int i = 0; i < 10; i++)
    {
        fprintf(f, "%11s\t", print_names[i]);
    }
    fprintf(f, "\n");
    for (int i = 0; i < 8; i++)
    {
        fprintf(f, "%11" PRIu64"\t", print_values[i]);
    }
    fprintf(f, "%11.0f\t %11u \n", (double)reads_read/(double)((double)time_end-(double)time_start), time_end-time_start);

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






bool BinarySearchTree::FlipBitsCheck(char *seq, bool r2) {
    /*Another error check for the humans*/
    if (strlen(seq) < start+charLength) {
        fprintf(stderr, "Error within binarySearch.cpp in funciton FlipBitsChars() strlen of %s larger than start + length\n", r2 ? "R2" : "R1");
        fprintf(stderr, "String Len = %lu, start = %d, length = %d\n", strlen(seq), start, charLength);
        disReads++;
        return false;
    }

    return true;

}


char RC_BP(char bp) {
    if (bp == 'N') {
        return 'N';
    } else if (bp == 'A') {
        return 'T';
    } else if (bp == 'T') {
        return 'A';
    } else if (bp == 'G') {
        return 'C';
    } else if (bp == 'C') {
        return 'G';
    } else {
        return '\0';
    }
}

void RC_Read(char **seq) {
    char *new_seq = strdup(*seq);
    int len = strlen(*seq);
    int i = 0;
    while ((*seq)[i] != '\0') {
        (*seq)[(len - 1) - i] = RC_BP(new_seq[i]);
        if ((*seq)[(len -1) - i] == '\0') {
            fprintf(stderr, "ERROR in RC_Read in binarySearch.cpp, Illegal char in %s\n", *seq);
            exit(42);
        }
        i++;
    }
    free(new_seq);
}



/*Flip bits functionality for character string*/
bool BinarySearchTree::FlipBitsChars(readInfo *R1, readInfo *R2, uint16_t **id, bool RC) {
    char *seq_1, *seq_2;

    /*Incementor for id array*/
    uint16_t idLoc = 0, bitShifts = 0;

    seq_1 = R1->getSeq();
    if (!(FlipBitsCheck(seq_1, false))) { return false; }

    if (R2) {
        seq_2 = R2->getSeq();
        if (!(FlipBitsCheck(seq_2, true))) { return false; }
    } else {
        seq_2 = NULL;
    }

    char *seq;

    if (seq_2 == NULL) {
        seq = (char *)malloc(sizeof(char) * charLength + 1);
        sprintf(seq, "%.*s", charLength, seq_1 + start);
    }
    else if (RC) {
        seq = (char *)malloc(sizeof(char) * charLength * 2 + 1 );
        sprintf(seq, "%.*s%.*s", charLength, seq_2 + start, charLength, seq_1 + start);
    }
    else {
        seq = (char *)malloc(sizeof(char) * charLength * 2 + 1 );
        sprintf(seq, "%.*s%.*s", charLength, seq_1 + start, charLength, seq_2 + start);

    }

/*
    if (RC) {
        RC_Read(&seq);
    }
*/

    uint16_t loc = 0;

    (*id)[0] = 0;

    while (seq[loc] != '\0') {
        uint16_t bit_1, bit_2;

        if (seq[loc] == 'N') {
            disReads++;
            return false;
        }

        bit_1 = !!((seq[loc] + 10) & (1 << 2));
        bit_2= !!((seq[loc] + 10) & (1 << 4));
        (*id)[idLoc] ^= (bit_1 << bitShifts);
        bitShifts++;
        (*id)[idLoc] ^= (bit_2 << bitShifts);
        bitShifts++;

        if (bitShifts == 16) {
            bitShifts = 0;
            idLoc++;
            (*id)[idLoc] = 0;
        }
        loc++;
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

    //(*id) = (uint16_t *)malloc(mallocLength);

    uint16_t *tmp_id = (uint16_t *)malloc(mallocLength);
    uint16_t *tmp_id_rc = (uint16_t *)malloc(mallocLength);


    /*Not funcitonal yet*/
    if (R1->optimized) {
        fprintf(stderr, "Optimizmations no funciotnal yet\n");
        exit(-1);
    } else {
        if (FlipBitsChars(R1, R2, &tmp_id, false) && FlipBitsChars(R1, R2, &tmp_id_rc, true)) {
            if (GreaterThan(tmp_id, tmp_id_rc) > 0) {
                (*id) = tmp_id;
                free(tmp_id_rc);
            } else {
                (*id) = tmp_id_rc;
                free(tmp_id);
            }
            //printStuff(tmp_id, mallocLength);
            //printStuff(tmp_id_rc, mallocLength);
            /*Add Greater Than*/
            return true;
        }
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
    int tmpValue = 0;
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
