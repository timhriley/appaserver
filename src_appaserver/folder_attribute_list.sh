#!/bin/sh
# -------------------------------------------------
# src_appaserver/folder_attribute_list.sh
# -------------------------------------------------
#
# This process is accessed from library/attribute.c 
# Freely available software: see Appaserver.org
# -------------------------------------------------

if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
fi

if [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "Error in $0: you must . set_project first." 1>&2
	exit 1
fi

if [ "$#" -eq 1 ]
then
	folder_where="folder_attribute.folder = '$1'"
else
	folder_where="1 = 1"
fi

order_clause=display_order,primary_key_index

echo "select folder_attribute.folder,				\
	     attribute.attribute,				\
	     attribute_datatype,				\
	     width, 						\
	     float_decimal_places,				\
	     primary_key_index,					\
	     display_order,					\
	     omit_insert_yn,					\
	     omit_insert_prompt_yn,				\
	     omit_update_yn,					\
	     hint_message,					\
	     attribute.post_change_javascript,			\
	     attribute.on_focus_javascript_function,		\
	     folder_attribute.additional_unique_index_yn,	\
	     folder_attribute.additional_index_yn,		\
	     folder_attribute.lookup_required_yn,		\
	     folder_attribute.insert_required_yn,		\
	     attribute.lookup_histogram_output_yn,		\
	     attribute.lookup_time_chart_output_yn,		\
	     attribute.appaserver_yn				\
      from	attribute, 					\
		folder_attribute 				\
      where folder_attribute.attribute = attribute.attribute	\
	and $folder_where					\
      order by ${order_clause};"				|
sql

exit 0
