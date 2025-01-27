#!/bin/sh
# -------------------------------------------------
# src_appaserver/attribute_record_list.sh
# -------------------------------------------------
#
# This process is accessed from library/attribute.c 
# Freely available software: see Appaserver.org
# -------------------------------------------------

# echo "Starting: $0 $*" 1>&2

if [ "$#" -lt 2 ]
then
	echo \
"Usage: $0 application folder [delimiter]" 1>&2
	exit 1
fi

application=$1
folder=$2

if [ "$#" -eq 3 ]
then
	delimiter=$3
else
	delimiter="|"
fi

attribute=attribute
folder_attribute=folder_attribute

if [ "$folder" = "" ]
then
	echo "select '',						\
		     $attribute.attribute,				\
		     attribute_datatype,				\
		     width, 						\
		     float_decimal_places,				\
		     0,							\
		     0,							\
		     'n',						\
		     'n',						\
		     'n',						\
		     hint_message,					\
		     post_change_javascript,				\
		     on_focus_javascript_function,			\
		     'n',						\
		     'n',						\
		     'n',						\
		     'n',						\
		     'n',						\
		     'n',						\
		     'n'						\
      	from $attribute;"						|
	sql.e "$delimiter"
else
	order_clause=display_order,primary_key_index

	echo "select $folder_attribute.folder,				\
		     $attribute.attribute,				\
		     attribute_datatype,				\
		     width, 						\
		     float_decimal_places,				\
		     primary_key_index,					\
		     display_order,					\
		     omit_insert_yn,					\
		     omit_insert_prompt_yn,				\
		     omit_update_yn,					\
		     hint_message,					\
		     $attribute.post_change_javascript,			\
		     $attribute.on_focus_javascript_function,		\
		     $folder_attribute.additional_unique_index_yn,	\
		     $folder_attribute.additional_index_yn,		\
		     $folder_attribute.lookup_required_yn,		\
		     $folder_attribute.insert_required_yn,		\
		     $attribute.lookup_histogram_output_yn,		\
		     $attribute.lookup_time_chart_output_yn,		\
		     $attribute.appaserver_yn				\
	      from	$attribute, 					\
			$folder_attribute 				\
	      where $folder_attribute.attribute = $attribute.attribute	\
	      order by ${order_clause};"				|
	sql.e "$delimiter"
fi

exit 0
