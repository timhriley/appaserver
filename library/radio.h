/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/radio.h					*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef RADIO_H
#define RADIO_H

#include <unistd.h>
#include "boolean.h"
#include "list.h"

#define RADIO_CLASS		"radio_class"

#define RADIO_STATISTICS_VALUE		"radio_statistics"
#define RADIO_STATISTICS_DISPLAY	"statistics"

#define RADIO_GROUP_COUNT_VALUE		"radio_group_count"
#define RADIO_GROUP_COUNT_DISPLAY	"group_count"

#define RADIO_SPREADSHEET_VALUE		"radio_spreadsheet"
#define RADIO_SPREADSHEET_DISPLAY	"spreadsheet"

#define RADIO_INSERT_VALUE		"radio_insert"
#define RADIO_INSERT_DISPLAY		"insert"

#define RADIO_DELETE_VALUE		"radio_delete"
#define RADIO_DELETE_DISPLAY		"delete"

#define RADIO_GRACE_VALUE		"radio_grace"
#define RADIO_GRACE_DISPLAY		"gracechart"

#define RADIO_RCHART_VALUE		"radio_rchart"
#define RADIO_RCHART_DISPLAY		"Rchart"

#define RADIO_GOOGLE_VALUE		"radio_google"
#define RADIO_GOOGLE_DISPLAY		"googlechart"

#define RADIO_HISTOGRAM_VALUE		"radio_histogram"
#define RADIO_HISTOGRAM_DISPLAY		"histogram"

#define RADIO_SORT_VALUE		"radio_sort"
#define RADIO_SORT_DISPLAY		"sort_order"

#define RADIO_LOOKUP_VALUE		"radio_lookup"
#define RADIO_LOOKUP_DISPLAY		"lookup"

typedef struct
{
	char *radio_value;
	char *radio_display;
} RADIO_PAIR;

/* Usage */
/* ----- */
RADIO_PAIR *radio_pair_new(
			char *radio_value,
			char *radio_display );

/* Process */
/* ------- */
RADIO_PAIR *radio_pair_calloc(
			void );

typedef struct
{
	char *html;
} RADIO_VALUE;

/* Usage */
/* ----- */
RADIO_VALUE *radio_value_new(
			char *radio_list_name,
			char *radio_value_string,
			char *radio_display,
			char *initial_value );

/* Process */
/* ------- */
RADIO_VALUE *radio_value_calloc(
			void );

/* Returns heap memory */
/* ------------------- */
char *radio_value_html(	char *radio_list_name,
			char *radio_value_string,
			char *radio_display,
			char *initial_value );

typedef struct
{
	LIST *radio_value_list;
	char *html;
} RADIO_LIST;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
RADIO_LIST *radio_list_new(
			char *form_radio_list_name,
			LIST *radio_pair_list,
			char *initial_value );

/* Process */
/* ------- */
RADIO_LIST *radio_list_calloc(
			void );

/* Returns heap memory */
/* -------------------- */
char *radio_list_html(	LIST *radio_value_list );

#endif
