#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "candy_db.h"
#define MAXSIZE 16


typedef struct Child_In_Garten {
	char  name_of_child[MAXSIZE];
	candy* candy_pp;
} child;

int AllocateKidsArray(int num_children, child **children_array);
int AppendKid(child **children_array, int arr_size, char new_child[MAXSIZE], char new_name_of_candy[MAXSIZE]);
void ExcangeCandies(child **children_array, int arr_size);

