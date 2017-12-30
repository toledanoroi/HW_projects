#include "candy_db.h"


int GetCandy(char* name_of_candy, candy** candyp) {
	if (name_of_candy == NULL) {
		return -1;
	}
	for (int i = 0; i < 10; i++)
	{
		if ((strcmp(name_of_candy, Candy_Bank[i].name_of_candy)) == 0) {
			*candyp = &(Candy_Bank[i]);
			return 0;
		}
	}
	printf("Candy not found in Candy_Bank\n");
	return -1;
};
