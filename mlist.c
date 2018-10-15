#ifndef _MLIST_INCLUDED_
#define _MLIST_INCLUDED_

#define MAXBUCKLIM 20
#define SCALEFACTOR 2

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "mentry.h"
#include "mlist.h"

typedef struct node{
  MEntry * entry;
  struct node * next;
} Node;

typedef struct bucket{
  int size;
  struct node * head;
  struct node * tail;
} Bucket;

typedef struct mlist {
  int size;
  struct bucket **bucket;
} MList;

int ml_verbose = 0;		/* if true, prints diagnostics on stderr */

MEntry *ml_lookup(MList *ml, MEntry *me);
void ml_transfer(MList * target_ml, MList * source_ml);
MList * ml_resize(MList * ml);
void ml_destroy(MList * ml);

/* ml_create - created a new mailing list */
MList *ml_create(void){

  MList * ml;

  if(ml_verbose){ fprintf(stderr, "MList\t: Creating mailing list\n"); }

  // MALLOC 1
  if((ml = malloc(sizeof(MList))) == NULL){ return NULL; }

  // MALLOC 2
  if((ml->bucket = calloc(MAXBUCKLIM,sizeof(Bucket))) == NULL){ return NULL; }

  ml->size = MAXBUCKLIM;

  for(int i = 0; i < MAXBUCKLIM; i++){

    if((ml->bucket[i] = malloc(MAXBUCKLIM * sizeof(Node))) == NULL){ return 0; }

    ml->bucket[i]->head = (Node *)NULL;
    ml->bucket[i]->size = 0;

  }

  return ml;

}

/* ml_add - adds a new MEntry to the list;
 * returns 1 if successful, 0 if error (malloc)
 * returns 1 if it is a duplicate */
int ml_add(MList **ml, MEntry *me){

  MList * list = *ml;
  Node * nn;
  int nodeCounter = 0;
  unsigned int hash = me_hash(me,list->size);

  if ((nn = malloc(sizeof(Node))) == NULL){
		return 0;
	}

  //create new node to add in
  nn->entry = me;
  nn->next = NULL;

  if(list->bucket[hash] == NULL){
    list->bucket[hash] = NULL;
  }

  // if list contains no nodes
  if(list->bucket[hash]->size == 0){
    list->bucket[hash]->head = nn;
    list->bucket[hash]->tail = nn;
    list->bucket[hash]->size += 1;
    return 1;
  }

  Node * tempn = list->bucket[hash]->tail;

  if(list->bucket[hash]->size < MAXBUCKLIM){
    list->bucket[hash]->tail->next = nn;
    list->bucket[hash]->tail = nn;
    list->bucket[hash]->size += 1;
    return 1;

  }

  if(list->bucket[hash]->size >= MAXBUCKLIM){

    MList * newml;
    newml = ml_resize(list);
    ml_transfer(newml,list);
    list = newml;
    ml_destroy(newml);

  }
  return 0;
}

void ml_transfer(MList * target_ml, MList * source_ml){

  Node * current;

  for(int i = 0; i < source_ml->size; i++){
       current = source_ml->bucket[i]->head;
       while(current != NULL){
           ml_add(&target_ml, current->entry);
           current = current->next;
       }
   }
}

MList * ml_resize(MList * ml){

  unsigned long newsize;
  MList * newml;

  if((newml = (MList *)malloc(sizeof(MList))) == NULL) return NULL;

  newml->size = ml->size * SCALEFACTOR;

  if((newml->bucket = calloc(newml->size, sizeof(Bucket))) == NULL) return NULL;

   // allocating memory for each LList and setting head node to null
  for(int i = 0; i < newml->size; i++){
    if((newml->bucket[i] = malloc(newml->size * sizeof(Node))) == NULL){ return 0; }
    newml->bucket[i]->head = NULL;
    newml->bucket[i]->size = 0;
  }
  return newml;
}

/* ml_lookup - looks for MEntry in the list, returns matching entry or NULL */
MEntry *ml_lookup(MList *ml, MEntry *me){

  unsigned long hash = me_hash(me,ml->size);
  Node * node;

  if((ml == NULL)||(ml->bucket[hash] == NULL)||(ml->bucket[hash]->head == NULL)){return NULL;}

  if((node = ml->bucket[hash]->head) == NULL){return NULL;}

  if(me_compare(node->entry,me) == 0){return node->entry;}

  for(int i = 0; i < ml->bucket[hash]->size; i++){

    node = node->next;

    if(node != NULL){
      if(me_compare(node->entry,me) == 0){return node->entry;}
    }
  }
  return NULL;
}

void ml_destroy(MList * ml){

  struct node * temp;
  Node * nextnode;
  Node * thisnode;

  for(int i = 0; i < ml->size; i++){
    thisnode = ml->bucket[i]->head;
    nextnode = ml->bucket[i]->head->next;
    if(thisnode != NULL){
      free(thisnode);
    }

    while(nextnode != NULL){
      thisnode = nextnode;
      nextnode = nextnode->next;
      free(thisnode);
    }
    free(ml->bucket[i]);
  }
  free(ml);
}

#endif /* _MLIST_INCLUDED_ */
