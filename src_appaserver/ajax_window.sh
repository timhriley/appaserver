#!/bin/bash
# ---------------------------------------------------------------
# $APPASERVER_HOME/src_appaserver/ajax_window.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

if [ "$APPASERVER_DATABASE" != "" ]
then
	application_name=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application_name=$DATABASE
fi

if [ "$application_name" = "" ]
then
	echo "Error in `basename.e $0 n`: you must first:" 1>&2
	echo "\$ . set_database" 1>&2
	exit 1
fi

echo "`basename.e $0 n` $*" 1>&2

if [ "$#" -ne 8 ]
then
	echo "Usage: `basename.e $0 n` post_executable session login role form_foler ajax_folder popup_widget select_primary_key" 1>&2
	exit 1
fi

post_executable=$1
session_key=$2
login_name=$3
role_name=$4
form_folder=$5
ajax_folder=$6
popup_widget=$7
select_primary_key="$8"

function output_script_header()
{
	post_executable=$1
	application_name=$2
	session_key=$3
	login_name=$4
	role_name=$5
	form_folder=$6
	ajax_folder=$7
	popup_widget=$8

	title=`echo $ajax_folder | format_initial_capital.e`

cat << all_done

<script language="javascript" type="text/javascript">

function fork_${ajax_folder}_ajax_window( destination_element )
{
	var my_window;
	var my_document;
	var html_string;

html_string = '<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\">\n';
html_string += '<html xmlns=http://www.w3.org/1999/xhtml>\n';
html_string += '<HEAD>\n';
html_string += '<link rel=stylesheet type="text/css" href="/appaserver/template/style.css">'
html_string += '<TITLE>${title}</TITLE>\n';

html_string += '<script language="javascript" type="text/javascript">\n';
html_string += 'var global_request;\n';
html_string += 'function listen_${ajax_folder}_method()\n';
html_string += '{\n';
html_string += '	var result;\n';
html_string += '	var local_destination_element;\n';
html_string += '	var options_length;\n';
html_string += '	var value;\n';
html_string += '	var mnemonic;\n';
html_string += '	var label;\n';
html_string += '	var first_option_value = null;\n';
html_string += '	var i;\n';
html_string += '	if ( global_request.readyState != 4 ) return false;\n';
html_string += '	result = global_request.responseText;\n';
html_string += '	global_request.open( "DELETE", null );\n';
html_string += '	local_destination_element = window.opener.document.getElementById( \"';
html_string += destination_element;
html_string += '\" );\n';

// Synchronize with AJAX_SERVER_FORMAT_DELIMITER
// ---------------------------------------------
html_string += '	value = result.split( "|" );\n';

// Synchronize with AJAX_SERVER_DATA_DELIMITER
// -------------------------------------------
html_string += '	mnemonic = value[ 0 ].split( "@&" );\n';
html_string += '	label = value[ 1 ].split( "@&" );\n';

html_string += '	options_length = \n';
html_string += '	local_destination_element.options.length;\n';
html_string += '	if ( options_length >= 1 )\n';
html_string += '	{\n';
html_string += '		first_option_value = local_destination_element.options[ 0 ].value;\n';
html_string += '	}\n';
html_string += '	for ( i = 0; i < mnemonic.length; i++ )\n';
html_string += '	{\n';
html_string += '		if ( first_option_value != null\n';
html_string += '		&&   mnemonic[ i ] == first_option_value )\n';
html_string += '		{\n';
html_string += '			options_length--;\n';
html_string += '			continue;\n';
html_string += '		}\n';
html_string += '		local_destination_element.options[ i + options_length ] = \n';
html_string += '			new Option( label[ i ], mnemonic[ i ] );\n';
html_string += '	}\n';
html_string += '	window.close();\n';
html_string += '	return true;\n';
html_string += '}\n';

html_string += 'function send_${ajax_folder}_to_server()\n';
html_string += '{\n';
html_string += '	var source_element;\n';
html_string += '	var value;\n';
html_string += '	var action;\n';
html_string += '	source_element = \n';
html_string += '	document.getElementById( "${popup_widget}" );\n';
html_string += '	value = source_element.value;\n';

html_string += '	action = "/cgi-bin/${post_executable}?${application_name}+${session_key}+${login_name}+${role_name}+${form_folder}+${ajax_folder}+" + value;\n';

html_string += '	global_request = new XMLHttpRequest();\n';
html_string += '	global_request.open( "GET", action, true );\n';
html_string += '	global_request.onreadystatechange = listen_${ajax_folder}_method;\n';
html_string += '	global_request.send(null);\n';
html_string += '	return true;\n';
html_string += '}\n';

// Need to escape the 's' because 'script' is a reserved word.
// -----------------------------------------------------------
html_string += '</\script>\n';

html_string += '</HEAD>\n';
html_string += '<BODY>\n';

html_string += '<form>\n';
html_string += '<h3>${title}</h3><br>\n';
html_string += '<select name="${popup_widget}" id="${popup_widget}">\n';

all_done
}

function output_ajax_folder()
{
	ajax_folder=$1
	select_primary_key="$2"

	select.sh "$select_primary_key" $ajax_folder '' select |
	while read result
	do
		label=`echo $result | format_initial_capital.e`
		echo "html_string += '<option value=\"$result\">$label\n';"
	done

	echo "html_string += '</select>\n';"
}

function output_script_footer()
{
	ajax_folder=$1

cat << all_done
html_string += '<br>';
html_string += '<input type=button value="Submit" onClick="send_${ajax_folder}_to_server()" />\n';
html_string += '</form>\n';

html_string += '</BODY>\n';
html_string += '</HTML>';

	my_window = window.open( "", "${ajax_folder}", "width=400,height=450" );
	my_document = my_window.document;
	my_document.write( html_string );
	my_document.close();

	return true;
}

</script>

all_done

}

output_script_header	$post_executable		\
			$application_name		\
			$session_key			\
			$login_name			\
			$role_name			\
			$form_folder			\
			$ajax_folder			\
			$popup_widget

output_ajax_folder $ajax_folder "$select_primary_key"

output_script_footer $ajax_folder

exit 0
