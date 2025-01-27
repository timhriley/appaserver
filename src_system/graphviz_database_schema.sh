#!/bin/bash
# $APPASERVER_HOME/src_system/graphviz_database_schema.sh
# -------------------------------------------------------

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

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 process appaserver_yn" 1>&2
	exit 1
fi

process_name="$1"
appaserver_yn="$2"

if [ "$appaserver_yn" = "" -o "$appaserver_yn" = "appaserver_yn" ]
then
	appaserver_yn="n"
fi

generate_subschema ()
{
	subschema=$1

	subschema_file=`echo $subschema | sed 's/\&/and/g'`
	subschema_display=`echo $subschema | format_initial_capital.e`

	graphviz_file="${process_name}_${subschema_file}$$.gv"
	postscript_file="${process_name}_${subschema_file}$$.ps"
	postscript_prompt_file=/appaserver_data/$application/${postscript_file}
	graphviz_prompt_file=/appaserver_data/$application/${graphviz_file}

	graphviz_database_subschema.sh "$subschema" > $graphviz_file

	echo "Current directory=`pwd`" 1>&2
	echo "dot -Tps $graphviz_file -o $postscript_file" 1>&2

	dot -Tps $graphviz_file -o $postscript_file

	echo "<p><a href=$postscript_prompt_file>Subschema: ${subschema_display}</a>"
	# echo "<p><a href=$graphviz_prompt_file>Press to transmit ${subschema} graphviz file.</a>"
}

# main
# ----
if [ "$appaserver_yn" = "y" ]
then
	role=system
else
	role=supervisor
fi

where="exists( select subschema from appaserver_table, role_table where appaserver_table.subschema = subschema.subschema and appaserver_table.table_name = role_table.table_name and role_table.role = '$role' )"

echo "select subschema from subschema where $where;"	|
sql.e							|
while read subschema
do
	generate_subschema $subschema
done

#generate_subschema "null"

exit 0
