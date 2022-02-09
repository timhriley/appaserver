/* library/scatterplot.h						*/
/* -------------------------------------------------------------------- */
/* This is an ADT to produce grace scatterplots.			*/
/*									*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org	 		*/
/* -------------------------------------------------------------------- */

#ifndef SCATTERPLOT_H
#define SCATTERPLOT_H

#include "dictionary.h"
#include "list.h"
#include "aggregate_level.h"

/* Structures */
/* ---------- */
typedef struct
{
	char *anchor_entity;
	char *anchor_datatype;
	LIST *compare_entity_name_list;
	LIST *compare_datatype_name_list;
	DICTIONARY *anchor_value_dictionary;
	LIST *compare_value_dictionary_list;
	LIST *plot_list;
} SCATTERPLOT;

typedef struct
{
	char *entity_datatype;
	DICTIONARY *dictionary;
} COMPARE_VALUE_DICTIONARY;

typedef struct
{
	double anchor_value;
	double compare_value;
} POINT;

typedef struct
{
	char *anchor_entity_datatype;
	char *compare_entity_datatype;
	LIST *point_list;
} PLOT;

/* Prototypes */
/* ---------- */
SCATTERPLOT *scatterplot_new( void );

void scatterplot_populate_compare_value_dictionary_list(
				DICTIONARY **anchor_value_dictionary_pointer,
				LIST *compare_value_dictionary_list,		
				char *anchor_entity_datatype,
				char *sys_string,
				int measurement_date_time_piece,
				int datatype_piece,
				int value_piece,
				char record_delimiter );

PLOT *scatterplot_get_or_set_plot(
				LIST *plot_list,
				char *anchor_entity_datatype,
				char *compare_entity_datatype );

COMPARE_VALUE_DICTIONARY *scatterplot_compare_value_dictionary_seek(
				char *compare_entity_datatype,
				LIST *compare_value_dictionary_list );

LIST *scatterplot_get_compare_value_dictionary_list(
				LIST *compare_entity_name_list,
				LIST *compare_datatype_name_list );

COMPARE_VALUE_DICTIONARY *scatterplot_compare_value_dictionary_new(
				char *entity_datatype );

PLOT *scatterplot_plot_new(	char *anchor_entity_datatype,
				char *compare_entity_datatype );

POINT *scatterplot_point_new(	double anchor_value,
				double compare_value );

LIST *scatterplot_get_plot_list(char *anchor_entity,
				char *anchor_datatype,
				LIST *compare_value_dictionary_list,
				char *sys_string,
				int measurement_date_time_piece,
				int datatype_piece,
				int value_piece,
				char record_delimiter,
				char entity_datatype_delimiter );

void scatterplot_output_scatter_plot(
				char *application_name,
				char *sys_string,
				char *anchor_entity,
				char *anchor_datatype,
				char *sub_title,
				char *appaserver_data_directory,
				LIST *compare_entity_name_list,
				LIST *compare_datatype_name_list,
				enum aggregate_level aggregate_level,
				char *anchor_units,
				LIST *compare_datatype_units_list,
				int measurement_date_time_piece,
				int datatype_piece,
				int value_piece,
				char record_delimiter,
				char entity_datatype_delimiter );

#endif
