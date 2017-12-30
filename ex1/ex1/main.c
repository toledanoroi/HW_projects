//#include "candy_db.h"
#include "kindergarten.h"
#include <stdio.h>

int GetNumLines(FILE *fp, int *Line) {
	

	if (fp == NULL) {
		printf("The file doesn't exist\n");
		return (-1);
	}
	while (!feof(fp))
	{
		char ch = fgetc(fp);
		if (ch == '\n' || ch == EOF) {
			(*Line)++;
		}
	}
	return (0);
}

int ParseLine(char *Child_Line, child *Parse_Child) {
	int count_child = 0;
	int count_candy = 0 , is_candy = 1;
	candy *candyp;
	//char *name_of_candy;
	char name_of_candy[MAXSIZE];
	int jj = 0;
	int comma = -1;
	int end = -1;
	while ((*(Child_Line + jj)) !='\n')
	{
		if ((*(Child_Line + jj)) == ',') {
			comma = jj;
			count_child = jj;
			if (count_child == 0) {
				printf("No name of child is given\n");
				return (-1);
			}
			if ((*(Child_Line + (count_child + 1))) == '\n') {
				printf("No name of candy is given\n");
				return (-1);
			}
			strncpy((*Parse_Child).name_of_child, Child_Line, count_child);
			//strncpy_s((*Parse_Child).name_of_child, MAXSIZE, *Child_Line, count_child);
			//((*Parse_Child).name_of_child)[count_child] = '\0';
			(*Parse_Child).name_of_child[count_child] = '\0';
		}
		else
		{
			if ((*(Child_Line + (jj + 1))) == '\n') {
				if (comma == -1) {
					printf("No comma is given in the child line\n");
					return (-1);
				}
			}
		}
		jj++;
	}
	count_candy = jj -(count_child + 1);
	//strncpy((*Parse_Child).candy_pp->name_of_candy, *(Child_Line+(count_child+1)), count_candy);
	//((*Parse_Child).candy_pp->name_of_candy)[count_candy] = '\0';
	//strncpy((*Parse_Child).name_of_child, Child_Line, count_child);
	strncpy(name_of_candy, (Child_Line + (count_child + 1)), count_candy);
	name_of_candy[count_candy] = '\0';
	is_candy = GetCandy(name_of_candy, &candyp);

	if (is_candy < 0) {
		printf("The candy : %s, is not in the Candy Bank\n", name_of_candy);
		return (-1);
	}
	(*Parse_Child).candy_pp = candyp;
	return 0;

}

int ImportKidsArray(FILE *fp, int num_of_children, child **children_arr) {
	
	int is_arr = 1, is_Parse = 1, jj = 0 , is_append = 1;
	char Line[51];
	child Parse_child;
	is_arr = AllocateKidsArray(num_of_children, children_arr);
	if (is_arr < 0) {
		printf("Memory allocation failed!!! exit\n");
		return (-1);
	}
	int go_to_start = fseek(fp, 0, SEEK_SET);
	while (!feof(fp)){
		while (fgets(Line,51,fp)) {
			is_Parse = ParseLine(Line, &Parse_child);

			if (is_Parse < 0) {
				printf("Parse failed - must exit\n");
					return (-1);
			}
			is_append = AppendKid(children_arr, num_of_children, (Parse_child).name_of_child, (Parse_child).candy_pp->name_of_candy);
			if (is_append < 0) {
				printf("Append_kid failed - must exit\n");
				return (-1);
			}
			jj++;
		}
		if (jj == num_of_children) {
			return (0);
		}
	}
}

int ExportKidsArray(FILE *fp_out, int num_of_children, child *children_arr) {
	if (fp_out == NULL) {
		printf("Problem with the output file");
		return (-1);

	}
	for (int jj = 0; jj < num_of_children; jj++){
		fprintf(fp_out, "%s,%s\n", children_arr[jj].name_of_child, children_arr[jj].candy_pp->name_of_candy);
	}
	return (0);
}

int main(int argc, char* argv[]) {
	// open files & parameters 
	FILE *fp_in, *fp_out;
	errno_t FP_in = fopen_s(&fp_in, "example_input3.txt", "r");
	errno_t FP_out = fopen_s(&fp_out, "my_output_test_0.txt", "w+");
	//check if files are valid......................................................
	if (fp_in == NULL) {
		printf("File is not exist\n");
		return (-1);
	}
	else {
		fseek(fp_in, 0, SEEK_END);
		int size = ftell(fp_in);
		if (size == 0) {
			printf("File is empty\nNo exchanges possible\nNeed to exit\n");
			return (-1);
		}
	}
	if (fp_out == NULL) {
		printf("Error! creating file failed\n");
		return (-1);
	}
	fseek(fp_in, 0, SEEK_SET);
	//..............................................................................
	int is_imp = 1, is_exp = 1, is_getnumlines = 1;
	int Line = 0;
	child *children_arr;

	// run 
	is_getnumlines = GetNumLines(fp_in, &Line);
	if (is_getnumlines < 0) {
		printf("GetNumLines failed\n");
		fclose(fp_in);
		fclose(fp_out);
		remove(argv[2]);
		return (-1);
	}
	is_imp = ImportKidsArray(fp_in, Line, &children_arr);
	if (is_imp < 0) {
		printf("ImportKidsArray failed\n");
		fclose(fp_in);
		fclose(fp_out);
		remove(argv[2]);
		return (-1);
	}
	ExcangeCandies(&children_arr, Line);
	is_exp = ExportKidsArray(fp_out, Line, children_arr);
	if (is_exp < 0) {
		printf("ExportKidsArray failed\n");
		fclose(fp_in);
		fclose(fp_out);
		remove(argv[2]);
		return (-1);
	}
	
	printf("The exchange is succesfully done!\n");
	//free
	

	free(children_arr);

	//close files 
	fclose(fp_in);
	fclose(fp_out);
	return (0);
}


