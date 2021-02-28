/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_education/event.h		*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#ifndef EVENT_H
#define EVENT_H

#include "boolean.h"
#include "list.h"
#include "transaction.h"
#include "venue.h"
#include "program.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define EVENT_TABLE			"event"

#define EVENT_PRIMARY_KEY		"program_name,"			\
					"event_date,"			\
					"event_time"

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	char *program_name;
	char *event_date;
	char *event_time;
	char *venue_name;
	char *street_address;
	double ticket_price;
	char *season_name;
	int year;
	char *revenue_account;
	PROGRAM *program;
	VENUE *venue;
	LIST *ticket_sale_list;
	LIST *ticket_refund_list;

	/* Process */
	/* ------- */
	int ticket_sale_count;
	double ticket_sale_total;
	int ticket_refund_count;
	double ticket_refund_total;
	int capacity_available;
} EVENT;

EVENT *event_new(	char *program_name,
			char *event_date,
			char *event_time );

EVENT *event_fetch(	char *program_name,
			char *event_date,
			char *event_time,
			boolean fetch_program,
			boolean fetch_venue );

EVENT *event_calloc(
			void );

LIST *event_system_list(
			char *system_string,
			boolean fetch_program,
			boolean fetch_venue );

EVENT *event_parse(
			char *input,
			boolean fetch_program,
			boolean fetch_venue );

char *event_system_string(
			char *where );

char *event_primary_where(
			char *program_name,
			char *event_date,
			char *event_time );

EVENT *event_list_seek(	char *program_name,
			LIST *event_list );

LIST *event_list_program_name_list(
			LIST *event_list );

void event_fetch_update(
			char *program_name,
			char *event_date,
			char *event_time );

void event_list_fetch_update(
			LIST *event_list );

/* Paypal */
/* ------ */
LIST *event_label_list(
			LIST *event_list );

/* Returns static memory */
/* --------------------- */
char *event_label_long_display(
			char *program_name,
			char *event_date,
			char *event_time );

/* Returns static memory */
/* --------------------- */
char *event_label_short_display(
			char *program_name,
			char *event_date,
			char *event_time );

EVENT *event_long_label_seek(
			char *event_label,
			LIST *event_list );

EVENT *event_short_label_seek(
			char *event_label,
			LIST *event_list );

#endif

