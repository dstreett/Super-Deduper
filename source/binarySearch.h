#ifndef SOURCE_BINARYSEARCHTREE_H_
#define SOURCE_BINARYSEARCHTREE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


#include "readInfo.h"
#include "fileWriter.h"


class BinarySearchTree {

    private:
        class Node {
            public:
            Node *left;
            Node *right;
            readInfo *R1;
            readInfo *R2;
            uint16_t *id;
            uint32_t qualScore;
            bool single;
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

            ~Node() {
                delete left;
                delete right;
                delete R1;
                delete R2;
                free(id);
            }
            
            void Replace(readInfo *R1_, readInfo *R2_, uint32_t qualScore_) {
                delete R1;
                delete R2;
                R1 = R1_;
                R2 = R2_;
                qualScore = qualScore_;
            } 
        };
        uint8_t A;
        uint8_t T;
        uint8_t G ;
        uint8_t C;
        
        uint64_t disReads;
        uint64_t nodesCreated;



        bool qualCheck; 
        Node *root;
        void PrivateAddNode(Node **n, readInfo *R1_, readInfo *R2_, uint16_t *id, uint32_t qualScore);
        uint32_t qualSum(char *q);
        bool getID(readInfo *R1, readInfo *R2, uint16_t **id);
        uint16_t charLength, mallocLength, start;


        bool FlipBitsChars(readInfo *R1, readInfo *R2, uint16_t **id);
        int GreaterThan(uint16_t *test, uint16_t *value);
        void PrintAndDeletePrivate(Node *n, FileWriter *R1, FileWriter *R2, FileWriter *SE);
    public:


        BinarySearchTree() {
            root = NULL;
            /*Defaults*/
            qualCheck = true;
            setStart(9);
            setLength(12);
            disReads = 0;
        
            A = 0;
            T = 3;
            G = 2;
            C = 1;
        };
        
        void AddNode(readInfo *R1_, readInfo *R2_);
        /*set the length to be 3 (for 3 bit format) * length specified / 16 (num of bits) + 1 (allocate the correct amount) *
         * 16 bits for each (sizeof)*/
        void setLength(uint16_t i) {mallocLength = ((3*i)/16 + 1) * sizeof(uint16_t); charLength = i;};
        /*converts human value to correct position in zero start array*/
        void setStart(uint16_t i) {start = i;};
        void setQualCheck(bool b) {qualCheck = b;};
        void PrintAndDelete(FileWriter *R1, FileWriter *R2, FileWriter *SE);




};







#endif

