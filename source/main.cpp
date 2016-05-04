#include "argCollector.h"
#include "binarySearch.h"
#include "readInfo.h"
#include "fileHelper.h"

int main(int argc, char *argv[]) {
        
    argCollector *args = new argCollector(argc, argv);

    BinarySearchTree *bst = new BinarySearchTree();
    bst->setLength(args->bpWindowSize);
    bst->setStart(args->startLoc);

    /*Error checking to make sure R1 and R2 exist in arrCollector*/
    if (args->R1_In && args->R2_In) {
        FileHelper *R1 = args->R1_In;
        FileHelper *R2 = args->R2_In;
        readInfo *r1, *r2;

        /*Yeah, I hate this too, I did try and do while((r1 = R1-readData) != NULL && (r2 = R2->readData....
         * however, it would check the R1->getData, receive NULL, and then exit if statment
         * this means r2 wouldn't also be set to null to check the size of the file R2
         * that is why I stuck with always true with the break*/
        while(1) {
            R1->readData(&r1);
            R2->readData(&r2);
            if (!r1 || !r2) {
                break;
            }
            /*The only reason I'm doing this is for error checking 
             * I need to make sure the files are the same size*/
            bst->AddNode(r1, r2);
        }

        /*Means the files are different lengths*/
        if (!r1 && r2) {
            fprintf(stderr, "File R1 is shorter than File R2\n");
            exit(16);
        } else if (r1 && !r2) {
            fprintf(stderr, "File R2 is shorter than File R1\n");
            exit(17);
        }
    }

    if (args->SE_In) {
        FileHelper *SE = args->SE_In;
        readInfo *se;

        while(1) {
            SE->readData(&se);
            if (se) {
                /*Single end reads R1 is !null R2 is null*/
                bst->AddNode(se, NULL);
            } else {
                break;
            }
        }      

    }

    if (args->INTER_In) {
        FileHelper *INTER = args->INTER_In;
        readInfo *r1, *r2;

        while (1) {
            INTER->readData(&r1, &r2);
            if (r1 && r2) {
                bst->AddNode(r1, r2);
            } else {
                break;
            }    
        }
    }
    
    if (args->TAB_In) {
        FileHelper *TAB = args->TAB_In;
        readInfo *r1, *r2;
        while (1) {
            TAB->readData(&r1, &r2);
            if (r1) {
                bst->AddNode(r1, r2);
            } else {
                break;
            }
        }
    }

    if (args->STDIN_In) {
        FileHelper *STDIN = args->STDIN_In;
        readInfo *r1, *r2;
        while (1) {
            STDIN->readData(&r1, &r2);
            if (r1) {
                bst->AddNode(r1, r2);
            } else {
                break;
            }
        }
    }
    
    bst->PrintAndDelete(args->R1_Out, args->R2_Out, args->SE_Out);
    bst->endTime();
    bst->outputStats(args->log);
    delete bst;
}
