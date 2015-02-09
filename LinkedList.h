
#ifndef Binary_Search_Tree_Read_2_h
#define Binary_Search_Tree_Read_2_h

#include "Binary_Search_Tree_Read_2.h"
#include <stdio.h>


class LinkedList {
	private:
		//Structure
		struct node {
			Read_Two_Node *r2_node;
     			node *left, *right;
  		};

		typedef node * nodeptr;

  		struct node *head;

	public:

       	// Constructor

   	LinkedList() {
      		head = NULL;
   	}


    
	// Add a node onto the front of the linked list.
	bool AddNode_Order(Read_Two_Node *r2);
	bool AddNode_Order_Private(struct node **ll_node, Read_Two_Node *r2);
  	bool Is_In_LinkedList(long double seq_bin); 
	Read_Two_Node *Pop();
};


#endif
