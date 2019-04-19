#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_ELEMENTS 10000			/*Maximum number of array elements*/
#define MAX_LINHA 50				/*Max size of the user input*/
#define MAX_FILE_NAME 80			/*Max size of the file's name*/

/*				Estruturas					*/

/*Structure for each element of the matrix*/
typedef struct {
	unsigned long line;
	unsigned long column;
	double value;
} Element;


/*				Global variables			*/


Element matrix[MAX_ELEMENTS];				/*Array with the elements of the matrix*/
Element lower_limit;						/*Dfines the matrix's lower bound*/
Element upper_limit;						/*Dfines the matrix's upper bound*/
int i = 0;									/*Number of elements represented*/
double zero = 0.0;
float dens;
char file_name[MAX_FILE_NAME];
int last_index_values;						/* Used to compress the matrix */


/*					Functions					*/


void command();
void redefine_dimensions();
void add_element(unsigned long new_line, unsigned long new_column, double new_value);
void list_elements();
void matrix_characteristics();
void list_line(unsigned int line_to_print);
void list_column(unsigned int column_to_print);
void sort_lines(int flag);
void sort_columns(int flag);
void redefine_zero(double new_zero);
void eliminate_value(unsigned int pos_to_eliminate);
void double_offset_indexing();
int find_max(int number_elements[MAX_ELEMENTS], unsigned int number_lines);
void set_values(int offset[MAX_ELEMENTS], double values[2*MAX_ELEMENTS], unsigned int line_max, int m);
void set_index(int index[2*MAX_ELEMENTS], unsigned int line_max, int m);
void save_in_file(char file_name[MAX_FILE_NAME]);


/*					Functions					*/


int main(int argc, char const *argv[]) {
	/*Beggining of the program. Recieves inputs from the command line*/

	char answer[MAX_LINHA];
	unsigned long a2, a3;
	double a4;

	/*Add the values of the file input to the matrix (received from command line)*/
	if (argc == 2) {

		FILE *fp;
		strcpy(file_name, argv[1]);
		fp = fopen(argv[1], "r");
		if (fp != NULL) {
			while (!feof(fp)) {
				fgets(answer, MAX_LINHA, fp);
				sscanf(answer, "%lu %lu %lf", &a2, &a3, &a4);
				add_element(a2, a3, a4);
			}
		}

		fclose(fp);
	}

	command();

	return 0;
}


void command() {

	char answer[MAX_LINHA], a1, a5[10] = "";
	unsigned long a3 = 0, a2 = 0;
	double a4 = zero;

	fgets(answer, MAX_LINHA, stdin);
	sscanf(answer, "%c", &a1);


	switch(a1) {

		case 'a':
			sscanf(answer, "%*c %lu %lu %lf", &a2, &a3, &a4);
			add_element(a2, a3, a4); 
			break;

		case 'p':
			list_elements(); 
			break;

		case 'i':
			matrix_characteristics();
			break;

		case 'l':
			sscanf(answer, "%*c %lu", &a2);
			list_line(a2); 
			break;

		case 'c':
			sscanf(answer, "%*c %lu", &a2);
			list_column(a2); 
			break;

		case 'o':
			sscanf(answer, "%*c %s", a5);

			if (!strcmp(a5, "column"))
				sort_columns(1);
			else
				sort_lines(1);
			
			break;

		case 'z':
			sscanf(answer, "%*c %lf", &a4);
			redefine_zero(a4);
			break;

		case 's':
			double_offset_indexing();
			break;

		case 'w':
			sscanf(answer, "%*c %s", a5);
			if (strcmp(a5, "")) {
				strcpy(file_name, a5);						/*Uptades the name of the last file used*/
				save_in_file(file_name);
			}
			else {
				
				if (!strcmp(file_name, ""))
					strcpy(file_name, "out.txt");			/*If there's no file name then it's created a file with a default name "out.txt"*/
				save_in_file(file_name);
			}

			break;

		case 'q':
			exit(0);

		default:
			command();

	}
	command();

}


void redefine_dimensions(){
	/*Redefine the matrix's upper and the lower limit */

	int j;

	lower_limit.line = MAX_ELEMENTS;
	lower_limit.column = MAX_ELEMENTS;
	upper_limit.line = 0;
	upper_limit.column = 0;

	for (j = 0; j < i; j++) {
	
		if(lower_limit.line > matrix[j].line)
			lower_limit.line = matrix[j].line;
		
		if(lower_limit.column > matrix[j].column)
			lower_limit.column = matrix[j].column;
		
		if(upper_limit.line < matrix[j].line)
			upper_limit.line = matrix[j].line;
		
		if(upper_limit.column < matrix[j].column)
			upper_limit.column = matrix[j].column;

	}
}


void add_element(unsigned long new_line, unsigned long new_column, double new_value) {
	/*Add a new element to the matix in line new_line, column new_column, with the value new_value*/

	Element aux;
	int j, flag = 0;

	for (j = 0; j < i; j++)
		if(matrix[j].line == new_line && matrix[j].column == new_column)
			if (new_value == zero)
				eliminate_value(j);

	if (new_value == zero)
		command();

	/*If there's an element in that position the value is replaced*/
	for (j = 0; j < i; j++){
		if (matrix[j].line == new_line){
			if (matrix[j].column == new_column){
				matrix[j].value = new_value;
				flag = 1;
			}
		}
	}

	if (i == 0){
		
		lower_limit.line = new_line;
		lower_limit.column = new_column;
		upper_limit.line = new_line;
		upper_limit.column = new_column;
	}

	if (flag == 0){

		aux.line = new_line;
		aux.column = new_column;
		aux.value = new_value;

		matrix[i] = aux;
		i++;
		redefine_dimensions();
	}
}


void list_elements() {
	/*List the elemento that are efectively represented int the form [line, column] = value */

	int j = 0;

	if (i == 0)
		printf("empty matrix\n");


	for (j = 0; j < i; j++)
		printf("[%lu;%lu]=%0.3f\n", matrix[j].line, matrix[j].column, matrix[j].value);

}


void matrix_characteristics() {
	/*Apresenta as carateristicas da matriz: os limites superiores e inferiores, o numero de elementos da matriz,
	o numero de elementos da matriz efetivamente representados(i) e a densidade(dens), dada por (i / size * 100)% */

	int size = 0;

	if (i == 0) {
		printf("empty matrix\n");
		command();
	}

	size = (upper_limit.line - lower_limit.line + 1) * (upper_limit.column - lower_limit.column + 1);
	dens = i / (float) size * 100;

	printf("[%lu %lu] [%lu %lu] %d / %d = %0.3f%%\n", lower_limit.line, lower_limit.column, upper_limit.line, upper_limit.column, i, size, dens);
}


void list_line(unsigned int line_to_print) {
	/*Lists the matrix's line line_to_print, including the zero elements. If there's only zero elements returns "empty line" */

	int j, flag = 0;
	double aux[MAX_ELEMENTS];

	if ((line_to_print > upper_limit.line) || (line_to_print < lower_limit.line)) {
		printf("empty line\n");
		command();
	}
	
	for (j = 0; j < i; j++)
		if (line_to_print == matrix[j].line)
			flag = 1;

	if (!flag) {
			printf("empty line\n");
			command();
	}

	for (j = 0; j <= upper_limit.column; j++)
		aux[j] = zero;

	for (j = 0; j < i; j++)
		if (line_to_print == matrix[j].line)
			aux[matrix[j].column] = matrix[j].value;

	for (j = lower_limit.column; j <= upper_limit.column; j++)
		printf(" %0.3f", aux[j]);

	printf("\n");
}


void list_column(unsigned int column_to_print) {
	/*Lists the matrix's column column_to_print, incluinding the zero elements. If there's only zero elements returns "empty column" */

	int j, flag = 0;
	double aux[MAX_ELEMENTS];

	if ((column_to_print > upper_limit.column) || (column_to_print < lower_limit.column)) {
		printf("empty column\n");
		command();
	}
	
	for (j = 0; j < i; j++)
		if (column_to_print == matrix[j].column)
			flag = 1;

	if (flag == 0) {
		printf("empty column\n");
		command();
	}

	for (j = 0; j <= upper_limit.column; j++)
		aux[j] = zero;

	for (j = 0; j < i; j++)
		if (matrix[j].column == column_to_print)
			aux[matrix[j].line] = matrix[j].value;
	
	for (j = lower_limit.line; j <= upper_limit.line; j++)
		printf("[%d;%d]=%0.3f\n", j, column_to_print, aux[j]);
}


void sort_lines(int flag){
	/*If we're calling the function from the command function flag = 1, if we're calling from another function flag = 0*/

	int x, y, done;
	Element temporary;

	if (flag)
		sort_columns(0);

	for (x = 1; x < i; x++) {

		done = 1;

		for (y = 0; y < i - 1; y++) {

			if (matrix[y].line > matrix[y+1].line) {

				temporary = matrix[y];
				matrix[y] = matrix[y+1];
				matrix[y+1] = temporary;
				done = 0;
			}
		}
		if (done) break;
	}

}


void sort_columns(int flag){
	/*If we're calling the function from the command function flag = 1, if we're calling from another function flag = 0*/

	int x, y, done;
	Element temporary;

	if (flag)
		sort_lines(0);

	for (x = 1; x < i; x++) {

		done = 1;

		for (y = 0; y < i - 1; y++) {

			if (matrix[y].column > matrix[y+1].column) {

				temporary = matrix[y];
				matrix[y] = matrix[y+1];
				matrix[y+1] = temporary;
				done = 0;
			}
		}

		if (done) break;
	}
}


void redefine_zero(double new_zero) {
	/*Redifines the value zero and eliminates the elements with that values*/

	int j;

	zero = new_zero;

	for (j = 0; j < i; j++) {

		if (matrix[j].value == new_zero){
			eliminate_value(j);
			j--;
		}

	}

	redefine_dimensions();
}


void eliminate_value(unsigned int pos_to_eliminate) {
	/*Deletes an Element in the position pos_to_eliminate*/

	int j;

	for (j = pos_to_eliminate; j < i; j++)
		matrix[j] = matrix[j+1];
	
	i--;
}


void save_in_file(char file_name[MAX_FILE_NAME]) {
	/* Stores the matrix info in a file*/
	
	FILE *fp;
	int j;

	fp = fopen(file_name, "w");
	
	if (fp != NULL) {
		for (j = 0; j < i; j++)
			fprintf(fp, "%lu %lu %.3f\n", matrix[j].line, matrix[j].column, matrix[j].value);
	
	}	
	fclose(fp);
}


void double_offset_indexing() {
	/* Matrix compression */

	double values[2*MAX_ELEMENTS];
	int index[2*MAX_ELEMENTS];
	int offset[MAX_ELEMENTS];
	int number_elements[MAX_ELEMENTS];												/*Stores the number of elements of each line*/
	int line_max = 0;																/*The line that has the highest number of elements*/
	int x, y, end = 0, flag, m = 0, elements_analised = 0;							/*flag = 0 -> Unsucess, flag = 1 -> Sucess, m -> offset*/
	int number_lines;

	number_lines = upper_limit.line - lower_limit.line + 1;							/* Current number of lines */

	last_index_values = i;
	
	if (dens > 50) {
		printf("dense matrix\n");
		command();
	}

	for (x = 0; x < MAX_ELEMENTS; x++) {
		values[x] = zero;
		number_elements[x] = 0;
		offset[x] = 0;
	}

	for (x = 0; x < i; x++)
		number_elements[matrix[x].line]++;

	line_max = find_max(number_elements, number_lines);

	m = 0;
	for (y = 0; y < i; y++){
		flag = 0;
		if (matrix[y].line == line_max) {
			elements_analised++;
			while (values[matrix[y].column+m] != zero) {
				m++;
				flag = 1;
			}

			if ((flag == 1) && (elements_analised != 1 )) {
				y = -1;
				elements_analised = 0;
			}

			if ((number_elements[line_max] == 1) && (flag == 1)) {
				y = -1;
				elements_analised = 0;
			}

			if ((elements_analised == number_elements[line_max]) && (flag == 0)) {
				set_values(offset ,values, line_max, m);
				set_index(index, line_max, m);
				number_elements[line_max] = 0;

				for (x = 0; x < number_lines; x++)
					if (number_elements[x] != 0)
						end = 1;
				
				if (end) {

					line_max = find_max(number_elements, number_lines);
					
					elements_analised = 0;
					m = 0;
					y = -1;
				}

				else
					y = i;
			}

		}
	}

	printf("value =");
	for (x = 0; x <= last_index_values; x++)
		printf(" %.3f", values[x]);
	printf("\n");

	printf("index =");
	for (x = 0; x <= last_index_values; x++)
		printf(" %d", index[x]);
	printf("\n");

	printf("offset =");
	for (x = lower_limit.line; x <= upper_limit.line ; x++)
		printf(" %d", offset[x]);
	printf("\n");

}


int find_max(int number_elements[MAX_ELEMENTS], unsigned int number_lines) {

	int x, line_max = 0;

	for (x = 0; x < number_lines; x++)
		if (number_elements[x] > number_elements[line_max])
			line_max = x;

	return line_max;
}


void set_values(int offset[MAX_ELEMENTS], double values[2*MAX_ELEMENTS], unsigned int line_max, int m) {
	/* Adds values to the values array */

	int x;
	int highest_column = 0;

	offset[line_max] = m;

	for (x = 0; x < i; x++)
		if (matrix[x].line == line_max) {
			values[matrix[x].column+m] = matrix[x].value;
			
			if (matrix[x].column > highest_column)
				highest_column = matrix[x].column;
		}

	while (((upper_limit.column - highest_column) + highest_column + m) > last_index_values)
		last_index_values++;
}


void set_index(int index[2*MAX_ELEMENTS], unsigned int line_max, int m) {
	/* Adds values to the index array */

	int x;

	for (x = 0; x < i; x++)
		if (matrix[x].line == line_max)
			index[matrix[x].column+m] = matrix[x].line;
}
