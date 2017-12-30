#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define MAXSIZE 16

typedef struct Candy {
	char name_of_candy[MAXSIZE];
	bool  is_vegan;
}candy;

static candy Candy_Bank[10] = { {"Apple",true}, {"Popcorn",true } ,{"Peanuts",true } ,{"Almonds",true },{"Chips",true}, {"Nougat",false}, {"Chocolate Bar",false}, {"Bamba",false}, {"Gummy Bears",false}, {"Toffee",false} };

int GetCandy(char* name_of_candy, candy** candyp);
