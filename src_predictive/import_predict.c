/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/import_predict.c			*/
/* -------------------------------------------------------------------- */
/* No warranty and freely available software. Visit appaserver.org	*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "String.h"
#include "appaserver_error.h"
#include "appaserver.h"
#include "application_clone.h"
#include "folder.h"
#include "float.h"
#include "date.h"
#include "insert.h"
#include "entity.h"
#include "security.h"
#include "export_table.h"
#include "account.h"
#include "feeder.h"
#include "insert.h"
#include "role.h"
#include "role_folder.h"
#include "folder.h"
#include "folder_attribute.h"
#include "application_create.h"
#include "journal.h"
#include "role.h"
#include "menu.h"
#include "import_predict.h"

IMPORT_PREDICT *import_predict_new(
		const char *import_predict_sqlgz,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		boolean bank_of_america_boolean,
		char *bank_full_name,
		char *your_full_name,
		char *mount_point )
{
	IMPORT_PREDICT *import_predict;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
	||   !your_full_name
	||   !mount_point )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	import_predict = import_predict_calloc();

	import_predict->import_predict_input =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		import_predict_input_new(
			import_predict_sqlgz,
			application_name,
			bank_of_america_boolean,
			mount_point );

	if ( !import_predict->import_predict_input->entity_self )
		return import_predict;

	import_predict->system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		import_predict_system_string(
			import_predict->
				import_predict_input->
				filespecification );

	import_predict->delete_role_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		import_predict_delete_role_system_string(
			IMPORT_PREDICT_PROCESS_NAME,
			ROLE_PROCESS_TABLE,
			ROLE_SUPERVISOR );

	import_predict->menu_href_string =
		/* ----------------------------- */
		/* Returns static memory or null */
		/* ----------------------------- */
		import_predict_menu_href_string(
			MENU_EXECUTABLE,
			application_name,
			session_key,
			login_name,
			role_name,
			import_predict->
				import_predict_input->
				application_menu_horizontal_boolean );

	if ( !import_predict->menu_href_string )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"import_predict_menu_href_string() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	import_predict->menu_anchor_tag =
		/* ----------------------------- */
		/* Returns static memory or null */
		/* ----------------------------- */
		import_predict_menu_anchor_tag(
			FRAMESET_PROMPT_FRAME,
			import_predict->menu_href_string );

	if ( !import_predict->menu_anchor_tag )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"import_predict_menu_anchor_tag() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	import_predict->bank_update_name =
		/* -------------------------------- */
		/* Returns either parameter or null */
		/* -------------------------------- */
		import_predict_bank_update_name(
			FEEDER_INSTALL_BANK_OF_AMERICA,
			bank_full_name,
			import_predict->import_predict_input->financial_enum );

	if ( import_predict->bank_update_name )
	{
		import_predict->entity_bank_financial_update_statement =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			import_predict_update_statement(
				ENTITY_FINANCIAL_INSTITUTION_TABLE,
				FEEDER_INSTALL_FINANCIAL_PLACEHOLDER,
				import_predict->bank_update_name );

		import_predict->entity_bank_update_statement =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			import_predict_update_statement(
				ENTITY_TABLE,
				FEEDER_INSTALL_FINANCIAL_PLACEHOLDER,
				import_predict->bank_update_name );
	}

	if ( import_predict->import_predict_input->appaserver_user_boolean )
	{
		import_predict->entity_user_update_statement =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			import_predict_update_statement(
				ENTITY_TABLE,
				APPLICATION_CLONE_NAME_PLACEHOLDER,
				your_full_name /* update_name */ );

		import_predict->entity_self_user_update_statement =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			import_predict_update_statement(
				ENTITY_SELF_TABLE,
				APPLICATION_CLONE_NAME_PLACEHOLDER,
				your_full_name /* update_name */ );

		import_predict->appaserver_user_update_statement =
			/* --------------------------- */
			/* Returns heap memory or null */
			/* --------------------------- */
			import_predict_update_statement(
				APPASERVER_USER_TABLE,
				APPLICATION_CLONE_NAME_PLACEHOLDER,
				your_full_name /* update_name */ );
	}

	return import_predict;
}

char *import_predict_system_string( char *filename )
{
	static char system_string[ 256 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
		"zcat %s |"
		"mysqldump_sed.sh |"
		"sql.e 2>&1 |"
		"grep -vi duplicate",
		filename );

	return system_string;
}

boolean import_predict_input_exists_boolean(
		const char *transaction_table )
{
	return
	(boolean)(unsigned int)(long)
	folder_where_fetch(
		(char *)transaction_table /* folder_name */,
		(LIST *)0 /* role_attribute_exclude_lookup_name_list */,
		0 /* not fetch_folder_attribute_list */,
		0 /* not fetch_attribute */ );
}

char *import_predict_input_filespecification(
		const char *import_predict_sqlgz,
		char *mount_point )
{
	char filespecification[ 128 ];

	snprintf(
		filespecification,
		sizeof ( filespecification ),
		"%s/src_predictive/%s",
		mount_point,
		import_predict_sqlgz );

	return strdup( filespecification );
}

boolean import_predict_input_template_boolean(
		const char *application_create_template_name,
		char *application_name )
{
	return
	( strcmp(
		application_create_template_name,
		application_name ) == 0 );
}

IMPORT_PREDICT *import_predict_calloc( void )
{
	IMPORT_PREDICT *import_predict;

	if ( ! ( import_predict =
			calloc( 1,
				sizeof ( IMPORT_PREDICT ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return import_predict;
}

char *import_predict_delete_role_system_string(
		const char *import_predict_process_name,
		const char *role_process_table,
		const char *role_supervisor )
{
	static char system_string[ 256 ];

	snprintf(
		system_string,
		sizeof ( system_string ),
	"echo \"delete from %s where role = '%s' and process = '%s';\" | "
		"tee_appaserver.sh | "
		"sql.e",
		role_process_table,
		role_supervisor,
		import_predict_process_name );

	return system_string;
}

char *import_predict_menu_href_string(
		const char *menu_executable,
		char *application_name,
		char *session_key,
		char *login_name,
		char *role_name,
		boolean application_menu_horizontal_boolean )
{
	static char href_string[ 256 ];

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( !application_menu_horizontal_boolean ) return NULL;

	snprintf(
		href_string,
		sizeof ( href_string ),
		"%s?%s+%s+%s+%s+y+%c",
		menu_executable,
		application_name,
		session_key,
		login_name,
		role_name,
		'y' /* content_type_yn */ );

	return href_string;
}

char *import_predict_menu_anchor_tag(
		const char *frameset_prompt_frame,
		char *href_string )
{
	static char anchor_tag[ 512 ];

	if ( !href_string ) return NULL;

	snprintf(
		anchor_tag,
		sizeof ( anchor_tag ),
		"<a target=%s href=\"%s\">Refresh Menu</a>",
		frameset_prompt_frame,
		href_string );

	return anchor_tag;
}

IMPORT_PREDICT_INPUT *import_predict_input_new(
		const char *import_predict_sqlgz,
		char *application_name,
		boolean bank_of_america_boolean,
		char *mount_point )
{
	IMPORT_PREDICT_INPUT *import_predict_input;

	if ( !application_name
	||   !mount_point )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	import_predict_input = import_predict_input_calloc();

	import_predict_input->template_boolean =
		import_predict_input_template_boolean(
			APPLICATION_TEMPLATE_NAME,
			application_name );

	import_predict_input->exists_boolean =
		import_predict_input_exists_boolean(
			TRANSACTION_TABLE );

	import_predict_input->entity_self =
		entity_self_fetch(
			entity_contact_key_boolean(
				ENTITY_TABLE,
				ENTITY_CONTACT_KEY_COLUMN ),
			0 /* not fetch_entity_boolean */ );

	if ( !import_predict_input->entity_self )
		return import_predict_input;

	import_predict_input->filespecification =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		import_predict_input_filespecification(
			import_predict_sqlgz,
			mount_point );

	import_predict_input->application_menu_horizontal_boolean =
		application_menu_horizontal_boolean(
		       application_name );

	import_predict_input->financial_enum =
		import_predict_input_financial_enum(
			ENTITY_FINANCIAL_INSTITUTION_TABLE,
			FEEDER_INSTALL_FINANCIAL_PLACEHOLDER,
			bank_of_america_boolean );

	import_predict_input->appaserver_user_boolean =
		import_predict_input_appaserver_user_boolean(
			APPLICATION_CLONE_NAME_PLACEHOLDER,
			import_predict_input->entity_self->full_name );

	return import_predict_input;
}

IMPORT_PREDICT_INPUT *import_predict_input_calloc( void )
{
	IMPORT_PREDICT_INPUT *import_predict_input;

	if ( ! ( import_predict_input =
			calloc( 1,
				sizeof ( IMPORT_PREDICT_INPUT ) ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return import_predict_input;
}


enum import_predict_input_financial_enum import_predict_input_financial_enum(
		const char *entity_financial_institution_table,
		const char *feeder_install_financial_institution_placeholder,
		boolean bank_of_america_boolean )
{
	boolean table_boolean;
	char *full_name;

	table_boolean = appaserver_table_boolean(
		(char *)entity_financial_institution_table );

	if ( table_boolean ) return predict_financial_no_update;

	full_name =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		entity_fetch_full_name(
			entity_financial_institution_table
				/* entity_table */,
			ENTITY_FULL_NAME_COLUMN );

	if ( string_strcmp(
		(char *)feeder_install_financial_institution_placeholder,
		full_name ) != 0 )
	{
		return predict_financial_no_update;
	}

	if ( bank_of_america_boolean )
		return predict_financial_bank_of_america;
	else
		return predict_financial_other_bank_name;
}

char *import_predict_update_statement(
		const char *table_name,
		const char *placeholder,
		char *update_name )
{
	char update_statement[ 1024 ];

	if ( !update_name ) return NULL;

	snprintf(
		update_statement,
		sizeof ( update_statement ),
		"update %s set full_name = '%s' where full_name = .%s.;",
		table_name,
		update_name,
		placeholder );

	return strdup( update_statement );
}

char *import_predict_bank_update_name(
		const char *bank_of_america,
		char *bank_full_name,
		enum import_predict_input_financial_enum financial_enum )
{
	if ( financial_enum == predict_financial_no_update )
		return NULL;
	else
	if ( financial_enum == predict_financial_bank_of_america )
		return (char *)bank_of_america;
	else
		return bank_full_name;
}

boolean import_predict_input_appaserver_user_boolean(
		const char *application_clone_name_placeholder,
		char *full_name )
{
	return
	( string_strcmp(
		(char *)application_clone_name_placeholder,
		full_name ) == 0 );
}
