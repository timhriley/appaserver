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
		char *financial_institution_full_name,
		char *name_of_bank,
		char *checking_begin_date,
		double checking_begin_balance,
		char *mount_point )
{
	IMPORT_PREDICT *import_predict;

	if ( !application_name
	||   !session_key
	||   !login_name
	||   !role_name
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
			financial_institution_full_name,
			name_of_bank,
			checking_begin_date,
			checking_begin_balance,
			mount_point );

	if ( import_predict->import_predict_input->bank_missing_boolean )
		return import_predict;

	if ( import_predict->import_predict_input->date_missing_boolean )
		return import_predict;

	if ( import_predict->import_predict_input->balance_zero_boolean )
		return import_predict;

	if ( !import_predict->import_predict_input->entity_self )
		return import_predict;

	import_predict->
		import_predict_process =
			/* -------------- */
			/* Safely returns */
			/* -------------- */
			import_predict_process_new(
				ROLE_PROCESS_TABLE,
				ROLE_SUPERVISOR,
				import_predict->
					import_predict_input->
					bank_name );

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

	import_predict->entity_system_string =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		import_predict_entity_system_string(
			ENTITY_TABLE,
			ENTITY_FINANCIAL_INSTITUTION_TABLE,
			ENTITY_STREET_ADDRESS_UNKNOWN,
			name_of_bank,
			import_predict->import_predict_input->bank_name );

	import_predict->import_predict_transaction =
		import_predict_transaction_new(
			TRANSACTION_BEGIN_TIME,
			checking_begin_date,
			checking_begin_balance,
			import_predict->
				import_predict_input->
				entity_self->
				full_name,
			import_predict->
				import_predict_input->
				entity_self->
				street_address );

	if ( !import_predict->import_predict_transaction
	||   !import_predict->
		import_predict_transaction->
		transaction )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof( message ),
	"import_predict_transaction(%.2lf) returned empty or incomplete.",
			checking_begin_balance );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	import_predict->import_predict_passthru =
		/* -------------- */
		/* Safely returns */
		/* -------------- */
		import_predict_passthru_new(
			import_predict->
				import_predict_input->
				entity_self->
				full_name,
			import_predict->
				import_predict_input->
				entity_self->
				street_address );

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

	import_predict->trial_balance_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		import_predict_trial_balance_system_string(
			IMPORT_PREDICT_TRIAL_BALANCE_EXECUTABLE,
			session_key,
			login_name,
			role_name );

	import_predict->financial_position_system_string =
		/* --------------------- */
		/* Returns static memory */
		/* --------------------- */
		import_predict_financial_position_system_string(
			IMPORT_PREDICT_FINANCIAL_POSITION_EXECUTABLE,
			session_key,
			login_name,
			role_name );

	return import_predict;
}

IMPORT_PREDICT_TRANSACTION *import_predict_transaction_new(
		const char *transaction_begin_time,
		char *checking_begin_date,
		double checking_begin_balance,
		char *full_name,
		char *street_address )
{
	IMPORT_PREDICT_TRANSACTION *import_predict_transaction;

	if ( !checking_begin_date
	||   !*checking_begin_date
	||   float_virtually_same( checking_begin_balance, 0.0 )
	||   !full_name
	||   !street_address )
	{
		char message[ 128 ];

		sprintf(message, "parameter is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	import_predict_transaction = import_predict_transaction_calloc();

	import_predict_transaction->date_time =
		import_predict_transaction_date_time(
			transaction_begin_time,
			checking_begin_date );

	import_predict_transaction->transaction =
		transaction_new(
			full_name,
			street_address,
			import_predict_transaction->date_time );

	import_predict_transaction->
		transaction->
		transaction_amount =
			checking_begin_balance;

	import_predict_transaction->
		transaction->
		memo = TRANSACTION_OPEN_MEMO;

	import_predict_transaction->
		transaction->
		journal_list =
			import_predict_transaction_journal_list(
				IMPORT_PREDICT_CASH_ACCOUNT,
				IMPORT_PREDICT_EQUITY_ACCOUNT,
				checking_begin_balance,
				0 /* not fetch_account_boolean */ );

	return import_predict_transaction;
}

LIST *import_predict_transaction_journal_list(
		const char *import_predict_cash_account,
		const char *import_predict_equity_account,
		double checking_begin_balance,
		boolean fetch_account_boolean )
{
	LIST *journal_list = list_new();
	JOURNAL *journal;

	if ( float_virtually_same( checking_begin_balance, 0.0 ) )
	{
		char message[ 128 ];

		sprintf(message, "checking_begin_balance is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	journal =
		journal_new(
			(char *)0 /* full_name */,
			(char *)0 /* street_address */,
			(char *)0 /* transaction_date_time */,
			(char *)import_predict_cash_account );

	if ( fetch_account_boolean )
	{
		journal->account =
			account_fetch(
				journal->account_name,
				1 /* fetch_subclassification */,
				1 /* fetch_element */ );
	}

	journal->debit_amount = checking_begin_balance;
	list_set( journal_list, journal );

	journal =
		journal_new(
			(char *)0 /* full_name */,
			(char *)0 /* street_address */,
			(char *)0 /* transaction_date_time */,
			(char *)import_predict_equity_account );

	if ( fetch_account_boolean )
	{
		journal->account =
			account_fetch(
				journal->account_name,
				1 /* fetch_subclassification */,
				1 /* fetch_element */ );
	}

	journal->credit_amount = checking_begin_balance;
	list_set( journal_list, journal );

	return journal_list;
}

boolean import_predict_input_balance_zero_boolean(
		double checking_begin_balance )
{
	if ( float_virtually_same(
		checking_begin_balance,
		0.0 ) )
	{
		return 1;
	}
	else
	{
		return 0;
	}
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

char *import_predict_transaction_date_time(
		const char *transaction_begin_time,
		char *checking_begin_date )
{
	static char transaction_date_time[ 32 ];

	if ( !checking_begin_date
	||   !*checking_begin_date )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"checking_begin_date is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		transaction_date_time,
		sizeof ( transaction_date_time ),
		"%s %s",
		checking_begin_date,
		transaction_begin_time );

	return transaction_date_time;
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

IMPORT_PREDICT_TRANSACTION *import_predict_transaction_calloc( void )
{
	IMPORT_PREDICT_TRANSACTION *import_predict_transaction;

	if ( ! ( import_predict_transaction =
			calloc( 1,
				sizeof ( IMPORT_PREDICT_TRANSACTION ) ) ) )
	{
		char message[ 128 ];

		sprintf(message, "calloc() returned empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return import_predict_transaction;
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

LIST *import_predict_map_list( void )
{
	LIST *list = list_new();

	list_set(
		list,
		import_predict_map_new(
			"bank_of_america",
			"BofA_checking_upload" ) );

	list_set(
		list,
		import_predict_map_new(
			"JP_morgan_chase",
			"chase_checking_upload" ) );

	return list;
}

IMPORT_PREDICT_MAP *import_predict_map_new(
		char *financial_institution_full_name,
		char *upload_process_name )
{
	IMPORT_PREDICT_MAP *import_predict_map;

	if ( !financial_institution_full_name
	||   !upload_process_name )
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

	import_predict_map = import_pedict_map_calloc();

	import_predict_map->
		financial_institution_full_name =
			financial_institution_full_name;

	import_predict_map->
		upload_process_name =
			upload_process_name;

	return import_predict_map;
}

IMPORT_PREDICT_MAP *import_pedict_map_calloc( void )
{
	IMPORT_PREDICT_MAP *import_pedict_map;

	if ( ! ( import_pedict_map =
			calloc( 1,
				sizeof ( IMPORT_PREDICT_MAP ) ) ) )
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

	return import_pedict_map;
}

char *import_predict_map_process_name(
		char *bank_name,
		LIST *import_predict_map_list )
{
	IMPORT_PREDICT_MAP *import_predict_map;

	if ( !bank_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"bank_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( ( import_predict_map =
			import_predict_map_seek(
				import_predict_map_list,
				bank_name ) ) )
	{
		return
		import_predict_map->upload_process_name;
	}
	else
	{
		return IMPORT_PREDICT_MAP_PROCESS_NAME;
	}
}

IMPORT_PREDICT_MAP *import_predict_map_seek(
		LIST *import_predict_map_list,
		char *bank_name )
{
	IMPORT_PREDICT_MAP *import_predict_map;

	if ( !bank_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"bank_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( list_rewind( import_predict_map_list ) )
	do {
		import_predict_map =
			list_get(
				import_predict_map_list );

		if ( strcmp(
			import_predict_map->financial_institution_full_name,
			bank_name ) == 0 )
		{
			return import_predict_map;
		}

	} while ( list_next( import_predict_map_list ) );

	return NULL;
}

IMPORT_PREDICT_PROCESS *import_predict_process_new(
		const char *role_process_table,
		const char *role_supervisor,
		char *bank_name )
{
	IMPORT_PREDICT_PROCESS *import_predict_process;

	if ( !bank_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"bank_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	import_predict_process = import_pedict_process_calloc();

	import_predict_process->
		import_predict_map_list =
			import_predict_map_list();

	import_predict_process->
		import_predict_map_process_name =
			/* -------------------------------------------- */
			/* Returns component of import_predict_map_list */
			/* -------------------------------------------- */
			import_predict_map_process_name(
				bank_name,
				import_predict_process->
					import_predict_map_list );

	import_predict_process->insert_system_string =
		import_predict_process_insert_system_string(
			role_process_table,
			role_supervisor,
			import_predict_process->
				import_predict_map_process_name );

	return import_predict_process;
}

IMPORT_PREDICT_PROCESS *import_pedict_process_calloc( void )
{
	IMPORT_PREDICT_PROCESS *import_pedict_process;

	if ( ! ( import_pedict_process =
			calloc(
				1,
				sizeof ( IMPORT_PREDICT_PROCESS ) ) ) )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"calloc() returned is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	return import_pedict_process;
}

char *import_predict_process_insert_system_string(
		const char *role_process_table,
		const char *role_supervisor,
		char *import_predict_map_process_name )
{
	static char system_string[ 256 ];

	if ( !import_predict_map_process_name )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"import_predict_map_process_name is empty." );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	snprintf(
		system_string,
		sizeof ( system_string ),
		"echo \"insert into %s (role,process ) values ('%s','%s');\" |"
		"sql.e",
		role_process_table,
		role_supervisor,
		import_predict_map_process_name );

	return system_string;
}

char *import_predict_input_bank_name(
		char *full_name,
		char *name_of_bank )
{
	if ( full_name
	&&   *full_name
	&&   strcmp(
		full_name,
		"full_name" ) != 0 )
	{
		return full_name;
	}
	else
	if ( name_of_bank
	&&   *name_of_bank
	&&   strcmp(
		name_of_bank,
		"name_of_bank" ) != 0 )
	{
		return name_of_bank;
	}
	else
	{
		return NULL;
	}
}

boolean import_predict_input_bank_missing_boolean(
		const char *security_forbid_character_string,
		char *bank_name )
{
	if ( !bank_name ) return 1;

	if ( security_forbid_boolean(
		security_forbid_character_string,
		bank_name /* string */ ) )
	{
		return 1;
	}

	return 0;
}

char *import_predict_entity_system_string(
		const char *entity_table,
		const char *entity_financial_institution_table,
		const char *entity_street_address_unknown,
		char *name_of_bank,
		char *import_predict_bank_name )
{
	char system_string[ 1024 ];
	LIST *insert_datum_list;
	char *insert_entity_sql_statement;
	char *insert_financial_institution_sql_statement;

	if ( string_strcmp(
		name_of_bank,
		import_predict_bank_name ) != 0 )
	{
		return NULL;
	}

	insert_datum_list = list_new();

	list_set(
		insert_datum_list,
		insert_datum_new(
			"full_name" /* attribute_name */,
			name_of_bank /* datum */,
			1 /* primary_key_index */,
			0 /* not attribute_is_number */ ) );

	list_set(
		insert_datum_list,
		insert_datum_new(
			"street_address" /* attribute_name */,
			(char *)entity_street_address_unknown /* datum */,
			2 /* primary_key_index */,
			0 /* not attribute_is_number */ ) );

	insert_entity_sql_statement =
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		insert_datum_sql_statement(
			(char *)entity_table,
			insert_datum_list );

	insert_financial_institution_sql_statement =
		insert_datum_sql_statement(
			(char *)entity_financial_institution_table,
			insert_datum_list );

	snprintf(
		system_string,
		sizeof ( system_string ),
		"echo \"%s\n%s\" | sql.e",
		insert_entity_sql_statement,
		insert_financial_institution_sql_statement );

	free( insert_entity_sql_statement );
	free( insert_financial_institution_sql_statement );

	return strdup( system_string );
}

boolean import_predict_input_date_missing_boolean(
		const char *security_forbid_character_string,
		char *checking_begin_date )
{
	if ( !checking_begin_date
	||   !*checking_begin_date
	||   strcmp( checking_begin_date, "checking_begin_date" ) == 0 )
	{
		return 1;
	}

	if ( security_forbid_boolean(
		security_forbid_character_string,
		checking_begin_date /* string */ ) )
	{
		return 1;
	}

	return 0;
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

char *import_predict_trial_balance_system_string(
		const char *import_predict_trial_balance_executable,
		char *session_key,
		char *login_name,
		char *role_name )
{
	static char system_string[ 256 ];

	if ( !session_key
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

	snprintf(
		system_string,
		sizeof ( system_string ),
		"%s %s %s %s trial_balance as_of_date 0 omit table",
		import_predict_trial_balance_executable,
		session_key,
		login_name,
		role_name );

	return system_string;
}

char *import_predict_financial_position_system_string(
		const char *import_predict_financial_position_executable,
		char *session_key,
		char *login_name,
		char *role_name )
{
	static char system_string[ 256 ];

	if ( !session_key
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

	snprintf(
		system_string,
		sizeof ( system_string ),
		"%s %s %s %s financial_position as_of_date 0 omit table",
		import_predict_financial_position_executable,
		session_key,
		login_name,
		role_name );

	return system_string;
}

IMPORT_PREDICT_PASSTHRU *import_predict_passthru_new(
		char *entity_self_full_name,
		char *entity_self_street_address )
{
	IMPORT_PREDICT_PASSTHRU *import_predict_passthru;

	if ( !entity_self_full_name
	||   !entity_self_street_address )
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

	import_predict_passthru = import_pedict_passthru_calloc();

	import_predict_passthru->card_insert_statement =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		import_predict_passthru_insert_statement(
			FEEDER_PHRASE_TABLE,
			IMPORT_PREDICT_PASSTHRU_ACCOUNT,
			IMPORT_PREDICT_CARD_FEEDER_PHRASE,
			entity_self_full_name,
			entity_self_street_address );

	import_predict_passthru->cash_insert_statement =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		import_predict_passthru_insert_statement(
			FEEDER_PHRASE_TABLE,
			IMPORT_PREDICT_PASSTHRU_ACCOUNT,
			IMPORT_PREDICT_CASH_FEEDER_PHRASE,
			entity_self_full_name,
			entity_self_street_address );

	import_predict_passthru->
		feeder_phrase_insert_statement_list =
			list_new();

	list_set(
		import_predict_passthru->feeder_phrase_insert_statement_list,
		import_predict_passthru->card_insert_statement );

	list_set(
		import_predict_passthru->feeder_phrase_insert_statement_list,
		import_predict_passthru->cash_insert_statement );

	return import_predict_passthru;
}

IMPORT_PREDICT_PASSTHRU *import_pedict_passthru_calloc(
		void )
{
	IMPORT_PREDICT_PASSTHRU *import_predict_passthru;

	if ( ! ( import_predict_passthru =
			calloc( 1,
				sizeof ( IMPORT_PREDICT_PASSTHRU ) ) ) )
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

	return import_predict_passthru;
}

char *import_predict_passthru_insert_statement(
		const char *feeder_phrase_table,
		const char *import_predict_passthru_account,
		const char *feeder_phrase,
		char *entity_self_full_name,
		char *entity_self_street_address )
{
	LIST *insert_datum_list = list_new();

	if ( !entity_self_full_name
	||   !entity_self_street_address )
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

	list_set(
		insert_datum_list,
		insert_datum_new(
			"feeder_phrase" /* attribute_name */,
			(char *)feeder_phrase /* datum */,
			1 /* primary_key_index */,
			0 /* not attribute_is_number */ ) );

	list_set(
		insert_datum_list,
		insert_datum_new(
			"nominal_account" /* attribute_name */,
			(char *)import_predict_passthru_account /* datum */,
			0 /* primary_key_index */,
			0 /* not attribute_is_number */ ) );

	list_set(
		insert_datum_list,
		insert_datum_new(
			"full_name" /* attribute_name */,
			entity_self_full_name /* datum */,
			0 /* primary_key_index */,
			0 /* not attribute_is_number */ ) );

	list_set(
		insert_datum_list,
		insert_datum_new(
			"street_address" /* attribute_name */,
			entity_self_street_address /* datum */,
			0 /* primary_key_index */,
			0 /* not attribute_is_number */ ) );

	return
	/* --------------------------- */
	/* Returns heap memory or null */
	/* --------------------------- */
	insert_folder_sql_statement_string(
		(char *)feeder_phrase_table,
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		insert_datum_attribute_name_list_string(
			insert_datum_list ),
		/* --------------------------- */
		/* Returns heap memory or null */
		/* --------------------------- */
		insert_datum_value_list_string(
			insert_datum_list ) );
}

IMPORT_PREDICT_INPUT *import_predict_input_new(
		const char *import_predict_sqlgz,
		char *application_name,
		char *financial_institution_full_name,
		char *name_of_bank,
		char *checking_begin_date,
		double checking_begin_balance,
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

	import_predict_input->bank_name =
		/* -------------------------------- */
		/* Returns either parameter or null */
		/* -------------------------------- */
		import_predict_input_bank_name(
			financial_institution_full_name,
			name_of_bank );

	import_predict_input->bank_missing_boolean =
		import_predict_input_bank_missing_boolean(
			SECURITY_FORBID_CHARACTER_STRING,
			import_predict_input->bank_name );

	if ( import_predict_input->bank_missing_boolean )
		return import_predict_input;

	import_predict_input->date_missing_boolean =
		import_predict_input_date_missing_boolean(
			SECURITY_FORBID_CHARACTER_STRING,
			checking_begin_date );

	if ( import_predict_input->date_missing_boolean )
		return import_predict_input;

	import_predict_input->date_recent_boolean =
		import_predict_input_date_recent_boolean(
			IMPORT_PREDICT_INPUT_DAYS_AGO,
			checking_begin_date,
			/* ------------------- */
			/* Returns heap memory */
			/* ------------------- */
			date_now_yyyy_mm_dd( date_utc_offset() ) );

	import_predict_input->balance_zero_boolean =
		import_predict_input_balance_zero_boolean(
			checking_begin_balance );

	if ( import_predict_input->balance_zero_boolean )
		return import_predict_input;

	import_predict_input->entity_self =
		entity_self_fetch(
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

	import_predict_input->appaserver_error_filename =
		/* ------------------- */
		/* Returns heap memory */
		/* ------------------- */
		appaserver_error_filename(
			application_name );

	import_predict_input->application_menu_horizontal_boolean =
		application_menu_horizontal_boolean(
		       application_name );

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

boolean import_predict_input_date_recent_boolean(
		const int import_predict_input_days_ago,
		char *checking_begin_date,
		char *date_now_yyyy_mm_dd )
{
	int days_between;

	if ( !checking_begin_date
	||   !date_now_yyyy_mm_dd )
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

	days_between =
		date_days_between(
			checking_begin_date /* early_date_string */,
			date_now_yyyy_mm_dd /* late_date_string */ );

{
char message[ 65536 ];
sprintf( message, "%s/%s()/%d: days_between = %d\n",
__FILE__,
__FUNCTION__,
__LINE__,
days_between );
msg( (char *)0, message );
}
	if ( days_between < 0 )
	{
		char message[ 128 ];

		snprintf(
			message,
			sizeof ( message ),
			"checking begin date is in the future %s.",
			checking_begin_date );

		appaserver_error_stderr_exit(
			__FILE__,
			__FUNCTION__,
			__LINE__,
			message );
	}

	if ( days_between < import_predict_input_days_ago )
		return 1;
	else
		return 0;
}

