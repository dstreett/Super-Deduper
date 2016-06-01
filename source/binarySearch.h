#ifndef SOURCE_BINARYSEARCHTREE_H_
#define SOURCE_BINARYSEARCHTREE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>


#include "readInfo.h"
#include "fileWriter.h"


class BinarySearchTree {

    private:
        class Node {
            public:
                /*Search pointers for the bst*/
                Node *left;
                Node *right;
                

                /*Contains read info, it will either have R1, or both R1 and R2*/
                readInfo *R1;
                readInfo *R2;

                /*ID is the key for the BST*/
                uint16_t *id;

                /*If quality checking is off, qualScore is zero*/
                uint32_t qualScore;
                uint16_t count;

                bool single;
                
                /*This is the only constructor needed, the only reason to create is to have reads within each node*/
                Node (readInfo *R1_, readInfo *R2_, uint16_t *id_, uint32_t qualScore_) {
                    left = NULL;
                    right = NULL;
                    R1 = R1_;
                    R2 = R2_;
                    qualScore = qualScore_;
                    if (!R2) {
                        single = true;
                    }
                    id = id_;
                }

                /*Deconstructor to ensure memory cleanup*/
                ~Node() {
                    delete left;
                    delete right;
                    delete R1;
                    delete R2;
                    free(id);
                }

                /*Replace values within the node and frees old memory*/
                void Replace(readInfo *R1_, readInfo *R2_, uint32_t qualScore_) {
                    delete R1;
                    delete R2;
                    R1 = NULL;
                    R2 = NULL;
                    R1 = R1_;
                    R2 = R2_;
                    qualScore = qualScore_;
                } 
        };
        /*A, T, G, C binary mapped to values*/
        uint8_t A;
        uint8_t T;
        uint8_t G ;
        uint8_t C;
       

        /*Tree Stats*/
        uint32_t time_start;
        uint32_t time_end;
        long long unsigned int disReads;
        long long unsigned int nodesCreated;
        long long unsigned int singletons;
        long long unsigned int doubles;
        long long unsigned int threeplus;
        long long unsigned int replaced;
        long long unsigned int reads_read;
        long long unsigned int dup_gone;

        bool qualCheck; 
        
        
        Node *root;


        uint32_t qualSum(char *q);
        uint16_t charLength, mallocLength, start;
        

        bool getID(readInfo *R1, readInfo *R2, uint16_t **id);
        bool FlipBitsChars(readInfo *R1, readInfo *R2, uint16_t **id);
        bool FlipBitsCharsRC(readInfo *R1, readInfo *R2, uint16_t **id);
        int GreaterThan(uint16_t *test, uint16_t *value);
        void PrivateAddNode(Node **n, readInfo *R1_, readInfo *R2_, uint16_t *id, uint32_t qualScore);
        void PrintAndDeletePrivate(Node *n, FileWriter *R1, FileWriter *R2, FileWriter *SE);
    public:


        BinarySearchTree() {
            /*Time start*/
            time_start = time(NULL);

            root = NULL;
            /*Defaults*/
            qualCheck = true;
            /*Default values*/
            setStart(10);
            setLength(12);
            
            
            
            /*Stats zero out*/
            nodesCreated = 0;
            singletons = 0;
            doubles = 0;
            threeplus = 0;
            replaced = 0;
            disReads = 0;
            reads_read = 0;    
            dup_gone = 0;

            /*00*/ 
            A = 0;
            /*11*/
            T = 3;
            /*10*/
            G = 2;
            /*01*/
            C = 1;
        };
        void Cleaner(uint16_t **bin);

        void AddNode(readInfo *R1_, readInfo *R2_);
        /*set the length to be 3 (for 3 bit format) * length specified / 16 (num of bits) + 1 (allocate the correct amount) *
         * 16 bits for each (sizeof)*/
        void setLength(uint16_t i) {mallocLength = ((2*i/16)+1) * sizeof(uint16_t); charLength = i;};
        /*converts human value to correct position in zero start array*/
        void setStart(uint16_t i) {start = i - 1;};
        void setQualCheck(bool b) {qualCheck = b;};
        void PrintAndDelete(FileWriter *R1, FileWriter *R2, FileWriter *SE);
        void endTime() {time_end = time(0);};
        
        /*Must be called after PrintAndDelete*/
        void outputStats(FILE *f);

};







#endif

