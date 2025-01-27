/* --------------------------------------------------------------	*/
/* $APPASERVER_HOME/library/bot_generated.h				*/
/* --------------------------------------------------------------	*/
/* No warranty and freely available software. Visit appaserver.org	*/
/* ---------------------------------------------------------------	*/

#ifndef BOT_GENERATED_H
#define BOT_GENERATED_H

#include "boolean.h"
#include "list.h"

#define BOT_GENERATED_DENY_PROCESS	"bot_generated.sh"
#define BOT_GENERATED_DIRECTORY		"/usr2/ufw"
#define BOT_GENERATED_SLEEP_SECONDS	60

typedef struct
{
	char *remote_ip_address;
	boolean appaserver_executable_exists_boolean;
	char *system_string;
	boolean yes_boolean;
	char *message;
} BOT_GENERATED;

/* Usage */
/* ----- */

/* Safely returns */
/* -------------- */
BOT_GENERATED *bot_generated_new(
		char *argv_0 );

/* Process */
/* ------- */
BOT_GENERATED *bot_generated_calloc(
		void );

/* Returns static memory */
/* --------------------- */
char *bot_generated_system_string(
		const char *bot_generated_deny_process,
		char *bot_generated_remote_ip_address );

/* Usage */
/* ----- */

/* Returns heap memory or null */
/* --------------------------- */
char *bot_generated_remote_ip_address(
		void );

/* Usage */
/* ----- */

/* Need to have user=www-data be in group=adm */
/* ------------------------------------------ */
boolean bot_generated_yes_boolean(
		char *bot_generated_system_string );

/* Returns static memory */
/* --------------------- */
char *bot_generated_message(
		char *argv_0,
		char *remote_ip_address );

#endif
