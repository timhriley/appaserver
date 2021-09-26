// $APPASERVER_HOME/javascript/keystrokes.js
// ---------------------------------------------
//
// This package is a set of functions that
// saves elements in forms for later retrieval.
// ---------------------------------------------
// Freely available software. See appaserver.org
// ---------------------------------------------

var non_multi_form_cookie_object = 0;
var multi_form_cookie_object = 0;
var non_multi_element_name_list;
var multi_element_name_list;

function keystrokes_multi_save(	form,
				cookie_key,
				multi_element_name_list_string,
				delimiter,
				move_left_right_index_delimiter,
				multi_select_remember_delimiter )
{
	if ( !multi_form_cookie_object )
	{
		multi_element_name_list =
			multi_element_name_list_string.split( delimiter );

		multi_form_cookie_object =
			new form_cookie(
				form,
				cookie_key,
				multi_element_name_list,
				0 /* not with_load */,
				1 /* is_multi */,
				move_left_right_index_delimiter,
				multi_select_remember_delimiter );
	}
	multi_form_cookie_object.store(
			form,
			multi_element_name_list,
			multi_select_remember_delimiter );
	return true;
}

function keystrokes_save(	form,
				cookie_key,
				non_multi_element_name_list_string,
				delimiter,
				move_left_right_index_delimiter )
{
	if ( !non_multi_form_cookie_object )
	{
		non_multi_element_name_list =
			non_multi_element_name_list_string.split( delimiter );

		non_multi_form_cookie_object =
			new form_cookie(
				form,
				cookie_key,
				non_multi_element_name_list,
				0 /* not with_load */,
				0 /* not is_multi */,
				move_left_right_index_delimiter );
	}
	non_multi_form_cookie_object.store( form, non_multi_element_name_list );
	return true;
}

function keystrokes_onload(	form,
				cookie_key,
				non_multi_element_name_list_string,
				delimiter,
				move_left_right_index_delimiter )
{
	var element_name;
	var element;
	var i;

	form_reset( form );

	non_multi_element_name_list =
		non_multi_element_name_list_string.split( delimiter );

	non_multi_form_cookie_object = new form_cookie(
				form,
				cookie_key,
				non_multi_element_name_list,
				1 /* with_load */,
				0 /* not is_multi */,
				move_left_right_index_delimiter );
	return true;
}

function keystrokes_multi_onload(
				form,
				cookie_key,
				multi_element_name_list_string,
				delimiter,
				move_left_right_index_delimiter,
				multi_select_remember_delimiter )
{
	var element_name;
	var element;
	var i;

	multi_element_name_list =
		multi_element_name_list_string.split( delimiter );

	multi_form_cookie_object = new form_cookie(
				form,
				cookie_key,
				multi_element_name_list,
				1 /* with_load */,
				1 /* is_multi */,
				move_left_right_index_delimiter,
				multi_select_remember_delimiter );
	return true;
}

