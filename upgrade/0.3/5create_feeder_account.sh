#!/bin/sh
#create feeder_account

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

# Returns 0 if FEEDER_PHRASE.feeder_phrase exists
# -----------------------------------------------
folder_attribute_exists.sh $application feeder_phrase feeder_phrase

if [ $? -ne 0 ]
then
	exit 0
fi

folder="folder"
relation="relation"
attribute="attribute"
folder_attribute="folder_attribute"
role_folder="role_folder"
row_security_role_update="row_security_role_update"
folder_row_level_restrictions="folder_row_level_restrictions"
subschema="subschema"
role_operation="role_operation"
process="process"
role="role"

(
cat << all_done
delete from $folder where folder = 'feeder_account';
insert into $folder (folder,form,insert_rows_number,subschema,populate_drop_down_process,post_change_process,post_change_javascript,lookup_before_drop_down_yn,notepad,no_initial_capital_yn,create_view_statement,lookup_email_output_yn,html_help_file_anchor,exclude_application_export_yn,appaserver_yn,data_directory,index_directory) values ('feeder_account','table','5','feeder',null,null,null,null,null,null,null,null,null,null,null,null,null);
delete from $relation where folder = 'feeder_account';
insert into $relation (folder,related_folder,related_attribute,pair_1tom_order,omit_1tom_detail_yn,prompt_mto1_recursive_yn,relation_type_isa_yn,copy_common_attributes_yn,omit_lookup_before_drop_down_yn,automatic_preselection_yn,ajax_fill_drop_down_yn,drop_down_multi_select_yn,join_1tom_each_row_yn,hint_message) values ('feeder_account','account','feeder_account',null,null,null,'y',null,null,null,null,null,null,null);
delete from $attribute where attribute = 'feeder_account';
insert into $attribute (attribute,attribute_datatype,width,float_decimal_places,post_change_javascript,hint_message,on_focus_javascript_function,lookup_histogram_output_yn,lookup_time_chart_output_yn,appaserver_yn) values ('feeder_account','text','30',null,null,'External accounts like credit card and Pay Pal that feed in transactions.',null,null,null,null);
delete from $folder_attribute where folder = 'feeder_account';
insert into $folder_attribute (folder,attribute,primary_key_index,display_order,omit_insert_yn,omit_insert_prompt_yn,additional_unique_index_yn,additional_index_yn,omit_update_yn,lookup_required_yn,insert_required_yn) values ('feeder_account','feeder_account','1',null,null,null,null,null,null,null,null);
insert into $role_folder (folder,role,permission) values ('feeder_account','supervisor','insert');
insert into $role_folder (folder,role,permission) values ('feeder_account','supervisor','update');
insert into $role (role,folder_count_yn,override_row_restrictions_yn) values ('supervisor','y','y');
insert into $subschema (subschema) values ('feeder');
insert into $role_operation (folder,role,operation) values ('feeder_account','supervisor','delete');
insert into $role_operation (folder,role,operation) values ('feeder_account','supervisor','detail');
all_done
) | sql.e 2>&1 | grep -iv duplicate

table_name=feeder_account
echo "drop table if exists $table_name;" | sql.e
echo "create table $table_name (feeder_account char (30) not null) engine MyISAM;" | sql.e
echo "create unique index $table_name on $table_name (feeder_account);" | sql.e

select="account"
from="account"
where="account = 'bank_of_america_checking' or account = 'chase_checking' or account = 'bank_of_america_credit_card'"

select.sh "$select" $from "$where"					 |
insert_statement table=feeder_account field=feeder_account delimiter="^" |
sql.e									 |
cat

exit 0
