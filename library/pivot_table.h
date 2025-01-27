/* library/pivot_table.h				   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */
#ifndef PIVIT_TABLE_H
#define PIVIT_TABLE_H

#include <stdio.h>
#include "list.h"

/* Constants */
/* --------- */
#define PIVOT_TABLE_ANCHOR_STRING_SIZE		4096

/* Enumerated types */
/* ---------------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *datatype_name;
	char *value;
} PIVOT_TABLE_DATATYPE;

typedef struct
{
	char delimiter;
	int anchor_column_count;
	LIST *datatype_list;
	char prior_anchor_string[ PIVOT_TABLE_ANCHOR_STRING_SIZE ];
} PIVOT_TABLE;

/* Usage */
/* ----- */
PIVOT_TABLE *pivot_table_new(
			char delimiter );

/* Process */
/* ------- */
PIVOT_TABLE *pivot_table_calloc(
			void );

/* Public */
/* ------ */
void pivot_table_set_datatype(
			LIST *datatype_list,
			char *datatype_name );

boolean pivot_table_set_string(
			FILE *output_pipe,
			int *anchor_column_count,
			char *prior_anchor_string,
			LIST *datatype_list,
			char delimiter,
			char *string );

void pivot_table_output(	FILE *output_pipe,
				char *prior_anchor_string,
				LIST *datatype_list,
				char delimiter );

void pivot_table_cleanup(	LIST *datatype_list );

PIVOT_TABLE_DATATYPE *pivot_table_seek_datatype(
				LIST *datatype_list,
				char *datatype_name );

#endif
