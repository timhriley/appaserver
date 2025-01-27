/* ----------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/column.h					   */
/* ----------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	   */
/* ----------------------------------------------------------------------- */

#ifndef COLUMN_H
#define COLUMN_H

#include "list.h"

#define COLUMN_MAX_FIELD_LENGTH			80
#define COLUMN_MAX_FIELDS			500

LIST *column2list(
		char *s );

char *column(	char *destination, 
		int field_offset,
		char *source );

int count_columns(
		char *s );

char *column_last(
		char *destination,
		char *source );

int column_count(
		char *s );

int column_get_str(
		char *buffer,
		char *string );

char *column_fill_characters(
		char *destination,
		char *string );

char *column2delimiter(
		char *destination,
		char *source,
		char delimiter );

char *column_skip_spaces(
		char *string );

int column_string_to_array(
		char *str );

int column_array_to_string(
		char *str );

char *column_replace(
		char *source_destination, 
		char *new_data, 
		int column_offset );

LIST *column_list(
		char *string );

#endif
