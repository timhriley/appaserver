#ifndef LOADLANG_H
#define LOADLANG_H

#include "tbl_load.h"

#define MAX_LINES		2048
#define MAX_LINE_LENGTH		1024
#define EOLN			10

typedef struct
{
	char *table_key;
	TABLE_LOAD *table_load;
} LANGUAGE_TABLE;

typedef struct
{
	char *line;
	int next_instruction;
	int eof_instruction;

} LANGUAGE_LINE;


/* Package Variables */
/* ----------------- */
LIST *table_list;				/* List of LANGUAGE_TABLE */
char input_field_delimiter;
char input_record_delimiter;
LANGUAGE_LINE *language_file_array[ MAX_LINES ];
char *current_table_key;
LANGUAGE_TABLE *current_language_table_ptr;
int debug_mode;
TABLE *variable_table;
int eof_indicator;
int program_counter;
FILE *input_file;
FILE *output_file;
FILE *exception_file;
FILE *error_file;
char input_string[ MAX_LINE_LENGTH ];
char input_field_delimiter;
char input_record_delimiter;
char end_of_file_marker;
int this_record_failed_column2variable;
int to_exception_upon_failure;
char *text_to_populate_if_duplicate_found;

/* Function prototypes */
/* ------------------- */
int set_table_key();
int set_uid_pwd();
int set_select_stmt();
int set_select_column();
int load_the_table();
int output_column();
int set_debug_mode();
int output_message();
int column2variable();
int output_variable();
int for_each_row();
int end_for();
int set_input_file();
int set_output_file();
int set_exception_file();
int set_error_file();
int set_variable();
int output_variable();
int loadlang_substr();
int set_constant();
int set_sysdate();
int set_last_work_date_month();
int add();
int subtract();
int multiply();
int divide();
int set_input_field_delimiter();
int set_input_record_delimiter();
int set_end_of_file_marker();
int set_byte();
int set_first_line_into();
int exit();
int concatenate();
int output_exception_file();
int set_argument();
int send_to_exception_upon_failure();
int populate_upon_duplicate();
int if_strcmp_is_true_continue();
int if_strcmp_is_false_continue();

/* Array of functions */
/* ------------------ */
static struct
{
	char *fn_name;
	int (*fn)();
} fn_array[] = {"set_table_key",		set_table_key,
		"set_uid_pwd",			set_uid_pwd,
		"set_select_stmt",		set_select_stmt,
		"set_select_column",		set_select_column,
		"load_the_table",		load_the_table,
		"output_column",		output_column,
		"set_debug_mode",		set_debug_mode,
		"output_message",		output_message,
		"column2variable",		column2variable,
		"set_input_file",		set_input_file,
		"set_output_file",		set_output_file,
		"set_exception_file",		set_exception_file,
		"set_error_file",		set_error_file,
		"output_variable",		output_variable,
		"for_each_row",			for_each_row,
		"end_for",			end_for,
		"set_variable",			set_variable,
		"output_variable",		output_variable,
		"substr",			loadlang_substr,
		"set_constant",			set_constant,
		"set_sysdate",			set_sysdate,
		"set_last_work_date_month",	set_last_work_date_month,
		"add",				add,
		"subtract",			subtract,
		"multiply",			multiply,
		"divide",			divide,
		"set_input_field_delimiter",	set_input_field_delimiter,
		"set_input_record_delimiter",	set_input_record_delimiter,
		"set_end_of_file_marker",	set_end_of_file_marker,
		"set_byte",			set_byte,
		"set_first_line_into",		set_first_line_into,
		"exit",				exit,
		"concatenate",			concatenate,
		"output_exception_file",	output_exception_file,
		"set_argument",			set_argument,
		"send_to_exception_upon_failure",send_to_exception_upon_failure,
		"populate_upon_duplicate",	populate_upon_duplicate,
		"if_strcmp_is_true_continue",	if_strcmp_is_true_continue,
		"if_strcmp_is_false_continue",	if_strcmp_is_false_continue,
		(char *)0,		NULL };

#endif
