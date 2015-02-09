#include "LinkedList.h"
#include <stdlib.h>



bool LinkedList::AddNode_Order_Private(struct node **ll_node, Read_Two_Node *r2_node) {






}


bool LinkedList::AddNode_Order(Read_Two_Node *r2_node) {

		AddNode_Order_Private(&head, r2_node);
/*		
		struct node *n, *prev, *curr;

		if (head == NULL) {
			n = new node;
			n->r2_node = r2_node;

			head = n;
			head->next = NULL;
		} else if (r2_node->seq_bin < (head->r2_node)->seq_bin) {
			n = new node;
			n->r2_node = r2_node;
	
			n->next = head;
			head = n;
		} else {
			prev = head;
			curr = head->next;
		
			//Don't go off the list and find the place to between
			while (curr != NULL && r2_node->seq_bin > (curr->r2_node)->seq_bin) {
				prev = curr;
				curr = curr->next;
			}

			//If curr is null, add to then end of the list
			if (curr == NULL) {
				n = new node;
				n->r2_node = r2_node;
		
				prev->next = n;
				n->next=NULL;
			} else if (r2_node->seq_bin == (curr->r2_node)->seq_bin) {
				return false;	
			} else {
			//Otherwise, put it in the middle
				n = new node;
				n->r2_node = r2_node;
		
				n->next = curr;
				prev->next=n;
			}
		}


		return true;					
*/

}

Read_Two_Node *LinkedList::Pop() {
/*
	struct node *tmp = head;

	if (head == NULL) {
		return NULL;
	}

	head = head->next;
	Read_Two_Node *r2 = tmp->r2_node;
	free(tmp);
	return r2;
*/
}


bool LinkedList::Is_In_LinkedList(long double seq_bin) {
/*
	if (head == NULL) {
		return false;
	} else {
		struct node *tmp = head;

		while (tmp != NULL && (tmp->r2_node)->seq_bin >= seq_bin) {
			if ((tmp->r2_node)->seq_bin == seq_bin) {
				return true;
			}
			tmp = tmp->next;
		}
	}

	return false;
*/
}
