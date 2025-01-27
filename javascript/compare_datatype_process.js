// $APPASERVER_HOME/javascript/compare_datatype_process.js
// ---------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// ---------------------------------------------------------------

function post_change_compare_datatype( current_element )
{
	var anchor_station_datatype;
	var anchor_station_datatype_value;
	var original_compare_station_compare_datatype;
	var compare_station_compare_datatype;
	var compare_station_compare_datatype_populated;
	var station_datatype_groups;
	var station_datatype_groups_value;
	var new_station_datatype_group;

	anchor_station_datatype =
		timlib_get_element(
			"station|datatype_0" );
	anchor_station_datatype.disabled = 0;

	anchor_station_datatype_value =
		timlib_get_drop_down_element_value(
			anchor_station_datatype.options );

	original_compare_station_compare_datatype =
		timlib_get_element(
			"original_compare_station|compare_datatype_1" );
	original_compare_station_compare_datatype.disabled = 0;

	compare_station_compare_datatype_element =
		timlib_get_element( "compare_station|compare_datatype_1" );

	compare_station_compare_datatype_element.disabled = 0;

	station_datatype_groups =
		timlib_get_element( "station_datatype_group|login_name_0" );
	station_datatype_groups.disabled = 0;

	new_station_datatype_group =
		timlib_get_element( "from_new_station_datatype_group_0" );
	new_station_datatype_group.disabled = 0;

	compare_station_compare_datatype_populated =
		timlib_get_multi_select_drop_down_element_populated(
			compare_station_compare_datatype_element.options );

	station_datatype_groups_value =
		timlib_get_drop_down_element_value(
			station_datatype_groups.options );

	if ( anchor_station_datatype_value != undefined
	&&   anchor_station_datatype_value != 'undefined'
	&&   anchor_station_datatype_value != 'select' )
	{
		station_datatype_groups.disabled = 1;
	}
	else
	if ( compare_station_compare_datatype_populated )
	{
		station_datatype_groups.disabled = 1;
	}
	else
	if ( station_datatype_groups_value != undefined
	&&   station_datatype_groups_value != 'undefined'
	&&   station_datatype_groups_value != 'select' )
	{
		compare_station_compare_datatype_element.disabled = 1;
		original_compare_station_compare_datatype.disabled = 1;
		new_station_datatype_group.disabled = 1;
		anchor_station_datatype.disabled = 1;
	}
	else
	if ( new_station_datatype_group.value != undefined
	&&   new_station_datatype_group.value != '' )
	{
		station_datatype_groups.disabled = 1;
	}

	return true;
} // post_change_compare_datatype()

