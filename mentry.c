#ifndef _MENTRY_INCLUDED_
#define _MENTRY_INCLUDED_

#define MAXLINE 100

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "mentry.h"

typedef struct mentry {
	char *surname;
	int house_number;
	char *postcode;
	char *full_address;
} MEntry;

/* me_get returns the next file entry, or NULL if end of file*/
MEntry *me_get(FILE *fd){

	// declare variables
	char line1 [MAXLINE], line2 [MAXLINE], line3 [MAXLINE], full_address [MAXLINE];
	char *ptr;
	int surname_len, postcode_len, fulladdress_len;

	// allocate memory in heap for entry and its parameters
	MEntry * entry = malloc(sizeof(MEntry));

	// reads line and stores to line
	// returns NULL if reached EOF
	if(fgets(line1,MAXLINE, fd) == NULL){
		// fprintf(stderr,"-> eof ||\n\n");
		return NULL;
	}

	fgets(line2,MAXLINE, fd);
	fgets(line3,MAXLINE, fd);

	// FULL_ADDRESS -- concatenating line1, line2, line3 into entry->full_address
	sprintf(full_address,"%s%s%s",line1, line2, line3);
	fulladdress_len = strlen(full_address);

	// SURNAME -- declare a char pointer to point to string after last instance of " "
	char * surname;
	if(strchr(line1,',')!=NULL){
		surname = strtok(line1,",");
	} else {
		surname = strrchr(line1,' ') + 1;
	}

	// SURNAME -- lower the case of lower_surname
	for(int i = 0; surname[i]; i++){
		if(i == surname_len-1){ surname[i] = '\0';}
		surname[i] = tolower(surname[i]);
	}

	// POSTAL CODE -- ower the case of postcode
	int n = 0;
	char postcode_val[strlen(line3)-1];
	for(int i = 0; i < strlen(line3); i++){
    if(isalnum(line3[i])){ postcode_val[n++] = tolower(line3[i]);}
  }

	postcode_len = strlen(postcode_val);

	// allocate memory for each char parameter as per the string length + 1
	entry->surname = malloc(strlen(surname)+1);
	entry->full_address = malloc(fulladdress_len+1);
	entry->postcode = malloc(postcode_len+1);

	strcpy(entry->full_address,full_address); // entry->full_address = fulll_address (line1 + line2 + line3)
	strcpy(entry->surname,surname); // entry->surname = 1 index after last instance of " "
	strcpy(entry->postcode,postcode_val); // entry->postcode = "AB35 4CB"
	entry->house_number = atoi(strtok(line2," ")); /* entry->house_number =
																										string token before first instance of
																										" " and converts to integer */
	return entry; // Returns MEntry entry
}

/* me_hash computes a hash of the MEntry, mod size */
unsigned long me_hash(MEntry *me, unsigned int size){

	unsigned long hash_val = 0;

	for(int i = 0; i < strlen(me->surname); i++){
		hash_val += me->surname[i];
	}
	for(int i = 0; i < strlen(me->postcode); i++){
		hash_val += me->postcode[i];
	}
	hash_val += me->house_number;

	return (hash_val % size);
}

/* me_print prints the full address on fd */
void me_print(MEntry *me, FILE *fd){

	// print surname, full address, and postcode
	printf("%s\n",me->full_address);

}

/* me_compare compares two mail entries, returning <0, 0, >0 if
* me1<me2, me1==me2, me1>me2
*/
int me_compare(MEntry *me1, MEntry *me2){

	// declare variables
	char me1_string[MAXLINE], me2_string[MAXLINE];

	// concatenate all parameters into a single string each
	sprintf(me1_string, "%s %i %s" ,me1->surname, me1->house_number, me1->postcode);
	sprintf(me2_string, "%s %i %s" ,me2->surname, me2->house_number, me2->postcode);
	// printf("%s || %s\n\n",me1_string,me2_string);

	// compare string1 and string2 and return output
	return strcmp(me1_string,me2_string);
}

/* me_destroys the mail entry
*/
void me_destroy(MEntry *me){

	//free memory of the MEntry object
	free(me->surname);
	free(me->full_address);
	free(me->postcode);
	free(me);

}

#endif /* _MENTRY_INCLUDED_ */
