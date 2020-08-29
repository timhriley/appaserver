#!/bin/bash
if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "Error in `basename.e $0 n`: you must first:" 1>&2
	echo "$ . set_database" 1>&2
	exit 1
fi

# Returns 0 if BANK_UPLOAD.bank_date exists
# -----------------------------------------
folder_attribute_exists.sh $application bank_upload bank_date

if [ $? -ne 0 ]
then
	exit 1
fi

(
table_name=`get_table_name $application reoccurring_transaction`
echo "alter table $table_name add feeder_phrase_ignore_yn char (1);" | sql.e
folder_attribute_table=`get_table_name $application folder_attribute`
echo "insert into $folder_attribute_table				\
	(folder,attribute,primary_key_index,display_order,		\
	 omit_insert_yn,omit_insert_prompt_yn,omit_update_yn,		\
	 additional_unique_index_yn,					\
	 additional_index_yn,						\
	 insert_required_yn,						\
	 lookup_required_yn )						\
	values								\
	('reoccurring_transaction','feeder_phrase_ignore_yn',null,4,null,null,null,null,null,null,null);" 	 |
sql.e
attribute_table=`get_table_name $application attribute`
echo "insert into $attribute_table					\
	(attribute,attribute_datatype,width,float_decimal_places,	\
	 hint_message,appaserver_yn,					\
	 post_change_javascript,					\
	 on_focus_javascript_function,					\
	 lookup_histogram_output_yn,					\
	 lookup_time_chart_output_yn )					\
	values								\
	('feeder_phrase_ignore_yn','text',1,0,'','n','','','n','n');"		 |
sql.e
) 2>&1 | grep -vi duplicate
exit 0
