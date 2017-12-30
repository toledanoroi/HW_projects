#include "kindergarten.h"


int AllocateKidsArray(int num_children, child **children_array) {
	//child *child_array = NULL;
	(*children_array) = (child*)malloc(num_children * sizeof(child));
	//*children_array = child_array;
	if (*children_array == NULL) {
		printf("Error! memory not allocated!\n");
		return -1;
	}
	//array to zero
	for (int j = 0; j < num_children; j++) {
		*( (*children_array)[j].name_of_child ) = '\0';
		//*( (*children_array)[j].candy_pp->name_of_candy ) = "";
		(*children_array)[j].candy_pp = NULL;
	}
	return 0;
};

int AppendKid(child **children_array, int arr_size, char new_child[MAXSIZE], char new_name_of_candy[MAXSIZE]) {
	candy *new_candy;
	int x = GetCandy(new_name_of_candy, &new_candy);
	if (x < 0) {
		printf("Error - GetCandy failed\n");
		return (-1);
	}


	for (int i = 0; i < arr_size; i++)
	{
		if ((*children_array)[i].name_of_child[0] =='\0'){
			strcpy((*children_array)[i].name_of_child, new_child);
			(*children_array)[i].candy_pp = new_candy;
			return 0;
		}
	}
	return (-1);
};

void ExcangeCandies(child **children_array, int arr_size) {
	int count_dis , first_veg = -1, first_non = -1, k, j;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	candy **copy_candies = (candy**)malloc(arr_size * sizeof(candy*));
	if (copy_candies == NULL) {
		printf("Error! memory not allocated!\n");
		return -1;
	}
	for (int i = 0; i < arr_size; i++)
	{
		copy_candies[i] = (*children_array)[i].candy_pp;

//		printf("%s,%s", copy_candies[i].name_of_candy, (*children_array)[i].candy_pp->name_of_candy);
//		printf("\n");
	}
	

	for (j = 0; j < arr_size; j++){
		if ((*copy_candies[j]).is_vegan == true) {
			if (first_veg == -1) {
				first_veg = j;
			}
			for (k = j + 1; k <= arr_size; k++){
				if (k < arr_size) {
					if ((*children_array)[k].candy_pp->is_vegan == true) {
						((*children_array)[k].candy_pp) = (copy_candies[j]);
						break;
					}
				}
				else {
					((*children_array)[first_veg].candy_pp) = (copy_candies[j]);
					break;
				}
			}
		}
	}
	for (int jj = 0; jj < arr_size; jj++) {
		if ((*copy_candies[jj]).is_vegan == false) {
			if (first_non == -1) {
				first_non = jj;
			}
			for (int kk = jj + 1; kk <= arr_size; kk++) {
				if (kk < arr_size) {
					if ((*children_array)[kk].candy_pp->is_vegan == false) {
						((*children_array)[kk].candy_pp) = (copy_candies[jj]);
						break;
					}
				}
				else {
					((*children_array)[first_non].candy_pp) = (copy_candies[jj]);
					break;
				}
			}
		}
	}

	free(copy_candies);
}