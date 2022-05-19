/* $APPASERVER_HOME/library/appaserver.h				*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef APPASERVER_H
#define APPASERVER_H

/* Constants */
/* --------- */
#define APPASERVER_FROM_STARTING_LABEL		"from_"
#define APPASERVER_TO_STARTING_LABEL		"to_"
#define APPASERVER_DROP_DOWN_ORIGINAL_LABEL	"original_"
#define APPASERVER_LOGIN_NAME_ATTRIBUTE_NAME	"login_name"

#define APPASERVER_RELATION_OPERATOR_PREFIX	"relation_operator_"
#define APPASERVER_EQUAL			"equal"
#define APPASERVER_NOT_EQUAL			"not_equal"
#define APPASERVER_LESS_THAN			"less_than"
#define APPASERVER_LESS_THAN_EQUAL_TO		"less_than_equal_to"
#define APPASERVER_GREATER_THAN			"greater_than"
#define APPASERVER_GREATER_THAN_EQUAL_TO	"greater_than_equal_to"
#define APPASERVER_BETWEEN			"between"
#define APPASERVER_BEGINS			"begins"
#define APPASERVER_CONTAINS			"contains"
#define APPASERVER_NOT_CONTAINS			"not_contains"
#define APPASERVER_OR				"or_,"
#define APPASERVER_NULL				"is_empty"
#define APPASERVER_NOT_NULL			"not_empty"

#define APPASERVER_INSERT_STATE			"insert"
#define APPASERVER_UPDATE_STATE			"update"
#define APPASERVER_LOOKUP_STATE			"lookup"
#define APPASERVER_VIEWONLY_STATE		"viewonly"
#define APPASERVER_DELETE_STATE			"delete"
#define APPASERVER_PREDELETE_STATE		"predelete"

#define APPASERVER_QUERY_TABLE_EDIT_MAX_ROWS	500

/* To retire */
/* --------- */
#define QUERY_FROM_STARTING_LABEL		"from_"
#define QUERY_TO_STARTING_LABEL			"to_"
#define QUERY_DROP_DOWN_ORIGINAL_STARTING_LABEL	"original_"
#define QUERY_RELATION_STARTING_LABEL		"relation_operator_"
#define QUERY_LOGIN_NAME_ATTRIBUTE_NAME		"login_name"

#define QUERY_EQUAL				"equal"
#define QUERY_NOT_EQUAL				"not_equal"
#define QUERY_LESS_THAN				"less_than"
#define QUERY_LESS_THAN_EQUAL_TO		"less_than_equal_to"
#define QUERY_GREATER_THAN			"greater_than"
#define QUERY_GREATER_THAN_EQUAL_TO		"greater_than_equal_to"
#define QUERY_BETWEEN				"between"
#define QUERY_BEGINS				"begins"
#define QUERY_CONTAINS				"contains"
#define QUERY_NOT_CONTAINS			"not_contains"
#define QUERY_OR				"or_,"
#define QUERY_NULL				"is_empty"
#define QUERY_NOT_NULL				"not_empty"

#endif

