/* output_appaserver.h */
/* -------------------- */

#ifndef OUTPUT_APPASERVER_H
#define OUTPUT_APPASERVER_H

/* Includes */
/* -------- */
#include "list.h"
#include "attribute.h"
#include "dictionary.h"
#include "folder.h"

/* Constants */
/* --------- */

/* Protoypes */
/* --------- */
void output_table_headings( LIST *heading_list );
void output_populated_horizontal_cells(
				char *customer,
				char *application,
				LIST *attribute_name_list,
				HASH_TABLE *attribute_hash_table,
				LIST *data_dictionary_list,
				int row );
int output_insert_horizontal_cells( 	
				FOLDER *folder,
				char *customer,
				char *application,
				int row );
void output_vertical_cells( 	FOLDER *folder,
				char *customer,
				char *application,
				int number_columns,
				DICTIONARY *data_dictionary );
void output_dictionary_as_hidden( char *dictionary );
#endif
