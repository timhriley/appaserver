#!/bin/bash
# $APPASERVER_HOME/src_system/graphviz_database_subschema.sh
# ----------------------------------------------------------

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
	echo "\$ . set_database" 1>&2
	exit 1
fi

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 subschema" 1>&2
	exit 1
fi

subschema="$1"

function select_attributes ()
{
	folder=$1

	primary_select="concat( '<tr><td>*</td><td align=left>', appaserver_column.column_name, '</td><td align=left>', column_datatype, ' ', width, ',', ifnull(float_decimal_places,0), '</td></tr>' )"

	attribute_select="concat( '<tr><td></td><td align=left>', appaserver_column.column_name, '</td><td align=left>', column_datatype, ' ', width, ',', ifnull(float_decimal_places,0), '</td></tr>' )"

	from="table_column,appaserver_column"

	join="table_column.column_name = appaserver_column.column_name"

	primary_where="table_column.table_name = '$folder' and ifnull(primary_key_index,0) <> 0"

	attribute_where="table_column.table_name = '$folder' and ifnull(primary_key_index,0) = 0"

	primary_order="primary_key_index"

	attribute_order="display_order"

	echo "	select $primary_select
		from $from
		where $primary_where and $join
		order by $primary_order;"		|
	sql						|
	sed 's/align=left/align="left"/g'		|
	sed "s/,0//"					|
	cat

	echo "	select $attribute_select
		from $from
		where $attribute_where and $join
		order by $attribute_order;"		|
	sql.e						|
	sed 's/align=left/align="left"/g'		|
	sed "s/,0//"					|
	cat

	echo "<hr/>"
}

function select_folder_count ()
{
	folder=$1

	if [ "$folder" = "application" ]
	then
		table="appaserver_application"
	elif [ "$folder" = "column" ]
	then
		table="appaserver_column"
	elif [ "$folder" = "table" ]
	then
		table="appaserver_table"
	else
		table="$folder"
	fi

	from="$table"

	select="count(*)"
	results=`echo "select $select from $from;" | sql.e`

	/bin/echo -n '<tr><td colspan="3">'
	echo "Rows: `commas_in_long.e $results`</td></tr>"
}

function select_attribute_flags ()
{
	folder=$1

	from="table_column"
	where="table_name = '$folder'"

	select="	column_name,
			omit_insert_prompt_yn,
			omit_insert_yn,
			additional_unique_index_yn,
			additional_index_yn,
			omit_update_yn,
			lookup_required_yn,
			insert_required_yn"

	echo "select $select from $from where $where;"	|
	sql.e						|
	while read record
	do
		attribute=`echo $record | piece.e '^' 0`
		omit_insert_prompt_yn=`echo $record | piece.e '^' 1`
		omit_insert_yn=`echo $record | piece.e '^' 2`
		additional_unique_index_yn=`echo $record | piece.e '^' 3`
		additional_index_yn=`echo $record | piece.e '^' 4`
		omit_update_yn=`echo $record | piece.e '^' 5`
		lookup_required_yn=`echo $record | piece.e '^' 6`
		insert_required_yn=`echo $record | piece.e '^' 7`

		if [ "$omit_insert_prompt_yn" = "y" ]
		then
			/bin/echo -n '<tr><td colspan="3">'
			/bin/echo "$attribute omit_insert_prompt</td></tr>"
		fi

		if [ "$omit_insert_yn" = "y" ]
		then
			/bin/echo -n '<tr><td colspan="3">'
			/bin/echo "$attribute omit_insert</td></tr>"
		fi

		if [ "$additional_unique_index_yn" = "y" ]
		then
			/bin/echo -n '<tr><td colspan="3">'
			/bin/echo "$attribute additional_unique_index</td></tr>"
		fi

		if [ "$additional_index_yn" = "y" ]
		then
			/bin/echo -n '<tr><td colspan="3">'
			/bin/echo "$attribute additional_index</td></tr>"
		fi

		if [ "$omit_update_yn" = "y" ]
		then
			/bin/echo -n '<tr><td colspan="3">'
			/bin/echo "$attribute omit_update</td></tr>"
		fi

		if [ "$lookup_required_yn" = "y" ]
		then
			/bin/echo -n '<tr><td colspan="3">'
			/bin/echo "$attribute lookup_required</td></tr>"
		fi

		if [ "$insert_required_yn" = "y" ]
		then
			/bin/echo -n '<tr><td colspan="3">'
			/bin/echo "$attribute insert_required</td></tr>"
		fi
	done
}

function select_folder_flags ()
{
	folder=$1

	from="appaserver_table"
	where="table_name = '$folder'"

	select="	drillthru_yn,
			no_initial_capital_yn,
			populate_drop_down_process,
			post_change_process,
			post_change_javascript,
			html_help_file_anchor"

	record=`echo "select $select from $from where $where;" | sql.e`

	drillthru_yn=`echo $record | piece.e '^' 0`
	no_initial_capital_yn=`echo $record | piece.e '^' 1`
	populate_drop_down_process=`echo $record | piece.e '^' 2`
	post_change_process=`echo $record | piece.e '^' 3`
	post_change_javascript=`echo $record | piece.e '^' 4`
	html_help_file_anchor=`echo $record | piece.e '^' 5`

	if [ "$drillthru_yn" = "y" ]
	then
		echo '<tr><td colspan="3">drillthru</td></tr>'
	fi

	if [ "$no_initial_capital_yn" = "y" ]
	then
		echo '<tr><td colspan="3">no_initial_capital</td></tr>'
	fi

	if [ "$populate_drop_down_process" != "" ]
	then
		/bin/echo -n '<tr><td colspan="3">'
		echo "${populate_drop_down_process}</td></tr>"
	fi

	if [ "$post_change_process" != "" ]
	then
		/bin/echo -n '<tr><td colspan="3">'
		echo "${post_change_process}</td></tr>"
	fi

	if [ "$post_change_javascript" != "" ]
	then
		/bin/echo -n '<tr><td colspan="3">'
		echo "${post_change_javascript}</td></tr>"
	fi

	if [ "$html_help_file_anchor" != "" ]
	then
		/bin/echo -n '<tr><td colspan="3">'
		echo "${html_help_file_anchor}</td></tr>"
	fi

}

function select_edges ()
{
	subschema="$1"

	if [ "$subschema" = "null" ]
	then
		subschema_where="appaserver_table.subschema is null"
	else
		subschema_where="appaserver_table.subschema = '$subschema'"
	fi

	join="appaserver_table.table_name = relation.table_name"
	related_folder_select="relation.related_table"
	from="appaserver_table,relation"
	where="$subschema_where and $join"

	select="	relation.table_name,
			related_table,
			related_column,
			pair_one2m_order,
			relation_type_isa_yn,
			copy_common_columns_yn,
			automatic_preselection_yn,
			drop_down_multi_select_yn,
			join_one2m_each_row_yn,
			omit_drillthru_yn,
			ajax_fill_drop_down_yn"

	echo "select $select from $from where $where;"			|
	sql.e								|
	while read record
	do
		folder=`echo $record | piece.e '^' 0`
		related_folder=`echo $record | piece.e '^' 1`
		related_attribute=`echo $record | piece.e '^' 2`
		pair_one2m_order=`echo $record | piece.e '^' 3`
		relation_type_isa_yn=`echo $record | piece.e '^' 4`
		copy_common_attributes_yn=`echo $record | piece.e '^' 5`
		automatic_preselection_yn=`echo $record | piece.e '^' 6`
		drop_down_multi_select_yn=`echo $record | piece.e '^' 7`
		join_one2m_each_row_yn=`echo $record | piece.e '^' 8`
		omit_drillthru_yn=`echo $record | piece.e '^' 9`
		ajax_fill_drop_down_yn=`echo $record | piece.e '^' 10`

		/bin/echo -n "$folder -> $related_folder [label="
		/bin/echo -n '"'

		if [ "$related_attribute" != "null" ]
		then
			/bin/echo -n "$related_attribute"
		fi

		if [ "$pair_one2m_order" != "" -a "$pair_one2m_order" != "0" ]
		then
			/bin/echo -n "\\npair-${pair_one2m_order}"
		fi

		if [ "$relation_type_isa_yn" = "y" ]
		then
			/bin/echo -n "\\nisa"
		fi

		if [ "$copy_common_attributes_yn" = "y" ]
		then
			/bin/echo -n "\\ncopy"
		fi

		if [ "$automatic_preselection_yn" = "y" ]
		then
			/bin/echo -n "\\nautomatic"
		fi

		if [ "$drop_down_multi_select_yn" = "y" ]
		then
			/bin/echo -n "\\nmulti"
		fi

		if [ "$join_one2m_each_row_yn" = "y" ]
		then
			/bin/echo -n "\\njoin"
		fi

		if [ "$omit_drillthru_yn" = "y" ]
		then
			/bin/echo -n "\\nomit-drillthru"
		fi

		if [ "$ajax_fill_drop_down_yn" = "y" ]
		then
			/bin/echo -n "\\najax"
		fi

		/bin/echo '"];'
	done

}

function select_nodes ()
{
	subschema="$1"

	if [ "$subschema" = "null" ]
	then
		subschema_where="appaserver_table.subschema is null"
	else
		subschema_where="appaserver_table.subschema = '$subschema'"
	fi

	join="appaserver_table.table_name = relation.table_name"
	folder_select="appaserver_table.table_name"
	related_folder_select="relation.related_table"
	from="appaserver_table,relation"
	where="$subschema_where and $join"

	(
	echo "select $folder_select from $from where $where;"		|
	sql.e								;
	echo "select $related_folder_select from $from where $where;"	|
	sql.e
	)								|
	sort -u								|
	while read folder
	do
		echo "$folder [label=<"
		echo '<table border="0" cellborder="0" cellspacing="0">'
	
		/bin/echo -n '<tr><td colspan="3">'
		/bin/echo -n "$folder"
		/bin/echo "</td></tr><hr/>"
	
		select_attributes "$folder"
		select_folder_flags "$folder"
		select_attribute_flags "$folder"
		select_folder_count "$folder"
	
		echo "</table>>];"
		echo ""
	done
}

function select_subschema ()
{
	application=$1
	subschema="$2"

	formatted_application=`echo $application | format_initial_capital.e`

	if [ "$subschema" = "null" ]
	then
		subschema_title="$formatted_application"
	else
		formatted_subschema=`echo $subschema | format_initial_capital.e`
		subschema_title="$formatted_application $formatted_subschema Subschema"
	fi

subschema_display=`echo $subschema | sed 's/\&/and/g'`

echo "digraph $subschema_display {"
echo "node [shape=box];"
echo "rankdir=BT"
echo 'labelloc="t";'
/bin/echo -n 'label="'
/bin/echo -n "$subschema_title"
/bin/echo '";'

select_nodes "$subschema"
select_edges "$subschema"

echo "}"

}

# main
# ----
select_subschema "$application" "$subschema"

exit 0
