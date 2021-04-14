#!/bin/bash
# ------------------------------------------------------
# $APPASERVER_HOME/src_appaserver/ajax_fill_drop_down.sh
# ------------------------------------------------------

if [ "$APPASERVER_DATABASE" = "" ]
then
	echo "Error in $0: you must .set_project first." 1>&2
	exit 1
fi

application=$APPASERVER_DATABASE

echo "$0 $*" 1>&2

if [ "$#" -ne 6 ]
then
	echo "Usage: $0 login_name role session one2m_folder mto1_folder select" 1>&2
	exit 1
fi

login_name=$1
role=$2
session=$3
one2m_folder=$4
mto1_folder=$5
select=$6

function output_script_header()
{
	application=$1
	login_name=$2
	role=$3
	one2m_folder=$4
	mto1_folder=$5

	title=`echo $one2m_folder | format_initial_capital.e`

cat << all_done

<script language="javascript" type="text/javascript">

function fork_ajax_window( destination_element_id )
{
	var my_window;
	var my_document;
	var html_string;

html_string = '<HTML>\n';
html_string += '<HEAD>\n';
html_string += '<link rel=stylesheet type="text/css" href="/appaserver/$application/style.css">'
html_string += '<TITLE>${title}</TITLE>\n';

html_string += '<script language="javascript" type="text/javascript">\n';
html_string += 'var http_request = new XMLHttpRequest();\n';
html_string += 'function listen_function()\n';
html_string += '{\n';
html_string += '	var ready;\n';
html_string += '	var status;\n';
html_string += '	var results;\n';
html_string += '	var mnemonic;\n';
html_string += '	var label;\n';
html_string += '	var e;\n';
html_string += '	var a;\n';
html_string += '	var i;\n';

html_string += '	ready = http_request.readyState;\n';
html_string += '	status = http_request.status;\n';
html_string += '	if ( status != 200 ) return;\n';
html_string += '	if ( ready != 4 ) return;\n';

html_string += '	results = http_request.responseText;\n';

html_string += '	e = window.opener.document.getElementById( \"';
html_string += destination_element_id;
html_string += '\" );\n';

html_string += '	a = results.split( "|" );\n';

html_string += '	mnemonic = a[ 0 ].split( "&" );\n';
html_string += '	label = a[ 1 ].split( "&" );\n';

html_string += '	for ( i = 0; i < mnemonic.length; i++ )\n';
html_string += '		e.options[ i ] = new Option( label[ i ], mnemonic[ i ] );\n';

html_string += '	e.options[ i ] = new Option( \"Select\", \"select\" );\n';

html_string += '	window.close()\n';
html_string += '	return true;\n';
html_string += '}\n';

html_string += 'function send_to_server()\n';
html_string += '{\n';
html_string += '	var action;\n';
html_string += '	var element;\n';
html_string += '	var value;\n';
html_string += '	element = document.getElementById( "${one2m_folder}" );\n';
html_string += '	value = element.value;\n';
html_string += '	action = "/cgi-bin/post_ajax_fill_drop_down?${application}+${login_name}+${role}+${session}+${mto1_folder}+${one2m_folder}+" + value;\n';
html_string += '	http_request.onreadystatechange = listen_function;\n';
html_string += '	http_request.open( "GET", action, true );\n';
html_string += '	http_request.send(null);\n';
html_string += '	return true;\n';
html_string += '}\n';

// Need to escape the 's' because script is a reserved word.
// ---------------------------------------------------------
html_string += '</\script>\n';

html_string += '</HEAD>\n';
html_string += '<BODY>\n';

html_string += '<form>\n';
html_string += '<h3>${title}</h3><br>\n';
html_string += '<select name="${one2m_folder}" id="${one2m_folder}">\n';

all_done

}


function output_one2m_folder()
{
	application=$1
	one2m_folder=$2
	select=$3

	echo "select $select from ${one2m_folder} order by $select;" | sql.e |
	while read results
do

	label=`echo $results | format_initial_capital.e`
	echo "html_string += '<option value=\"${results}\">${label}\n';"

done

	echo "html_string += '</select>\n';"

}


function output_script_footer()
{
	one2m_folder=$1
	mto1_folder=$2

cat << all_done
html_string += '<br>';
html_string += '<input type=button value="Submit" onClick="send_to_server()" />\n';
html_string += '</form>\n';

html_string += '</BODY>\n';
html_string += '</HTML>';

	my_window = window.open( "", "${one2m_folder}", "width=400,height=600" );
	my_document = my_window.document;
	my_document.write( html_string );
	my_document.close();

	return true;
}

</script>

all_done

}


output_script_header	$application		\
			"$login_name"		\
			"$role"			\
			$one2m_folder		\
			$mto1_folder

output_one2m_folder $application $one2m_folder "$select"
output_script_footer $one2m_folder $mto1_folder

exit 0
