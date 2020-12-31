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

/* Structures */
/* ---------- */
typedef struct
{
	/* Input */
	/* ----- */
	char *event_name;
	char *event_date;
	char *event_time;
	char *venue_name;
	double ticket_price;
	char *season_name;
	int year;
	char *program_name;
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

EVENT *event_new(	char *event_name,
			char *event_date,
			char *event_time );

EVENT *event_fetch(	char *event_name,
			char *event_date,
			char *event_time,
			boolean fetch_program,
			boolean fetch_venue,
			boolean fetch_sale_list,
			boolean fetch_refund_list );

LIST *event_list(	void );

EVENT *event_calloc(
			void );

LIST *event_system_list(
			char *sys_string,
			boolean fetch_program,
			boolean fetch_venue,
			boolean fetch_sale_list,
			boolean fetch_refund_list );

EVENT *event_parse(
			char *input,
			boolean fetch_program,
			boolean fetch_venue,
			boolean fetch_sale_list,
			boolean fetch_refund_list );

char *event_sys_string(
			char *where );

/* Returns static memory */
/* --------------------- */
char *event_name_escape(
			char *event_name );

char *event_primary_where(
			char *event_name,
			char *event_date,
			char *envent_time );

EVENT *event_list_seek(	char *event_name,
			LIST *event_list );

EVENT *event_seek(
			char *event_name,
			LIST *event_list );

char *event_fetch_program_name(
			char *event_name,
			char *event_date,
			char *event_time );

LIST *event_name_list(
			LIST *event_list );

#endif

