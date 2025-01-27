/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/library/fork_control.h				*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software: see Appaserver.org	*/
/* -------------------------------------------------------------------- */

#ifndef FORK_CONTROL_H
#define FORK_CONTROL_H

#include <unistd.h>
#include "list.h"
#include "boolean.h"

enum fork_control_process_state {
				fork_control_process_initial,
				fork_control_process_executing,
				fork_control_process_done };

typedef struct
{
	char *command_line;
	enum fork_control_process_state fork_control_process_state;
	pid_t running_process_id;
} FORK_CONTROL_PROCESS;

/* Usage */
/* ----- */
FORK_CONTROL_PROCESS *fork_control_process_new(
				char *command_line );

/* Process */
/* ------- */
FORK_CONTROL_PROCESS *fork_control_process_calloc(
				void );

/* Usage */
/* ----- */
void fork_control_process_set(
			LIST *fork_control_process_list /* in/out */,
			char *command_line );

/* Usage */
/* ----- */
FORK_CONTROL_PROCESS *fork_control_process_next(
			LIST *fork_control_process_list );

/* Usage */
/* ----- */
void fork_control_process_execute(
			enum fork_control_process_state *
				fork_control_process_state
					/* out */,
			pid_t *running_process_id
					/* out */,
			char *command_line );

/* Usage */
/* ----- */
boolean fork_control_process_pending_boolean(
			LIST *fork_control_process_list );

/* Usage */
/* ----- */
FORK_CONTROL_PROCESS *fork_control_process_seek(
			LIST *fork_control_process_list,
			pid_t done_process_id );

/* Public */
/* ------ */
void fork_control_process_list_display(
			LIST *fork_control_process_list );

typedef struct
{
	int parallel_count;
	LIST *fork_control_process_list;
} FORK_CONTROL;

/* Usage */
/* ----- */
FORK_CONTROL *fork_control_new(
			int parallel_count );

/* Process */
/* ------- */
FORK_CONTROL *fork_control_calloc(
			void );

/* Driver */
/* ------ */
void fork_control_execute(
			int parallel_count,
			LIST *fork_control_process_list );
#endif
