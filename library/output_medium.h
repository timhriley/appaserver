/* $APPASERVER_HOME/library/output_medium.h 				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef OUTPUT_MEDIUM_H
#define OUTPUT_MEDIUM_H

#include "list.h"
#include "boolean.h"
#include "dictionary.h"

#define OUTPUT_MEDIUM_STDOUT_STRING	"stdout"
#define OUTPUT_MEDIUM_GRACECHART_STRING	"gracechart"

enum output_medium {	output_medium_stdout,
			text_file,
			table,
			spreadsheet,
			gracechart,
			googlechart };

/* Usage */
/* ----- */
enum output_medium output_medium_get(
			char *output_medium_string );

/* Process */
/* ------- */

/* Usage */
/* ----- */
enum output_medium output_medium_dictionary_extract(
			DICTIONARY *dictionary );

/* Process */
/* ------- */

#endif
