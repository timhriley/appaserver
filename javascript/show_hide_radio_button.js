function show_hide_radio_button( element )
{
	if ( element.value == undefined || element.value == '' )
		$('.lookup_option_radio_button').show();
	else
		$('.lookup_option_radio_button').hide();
	return true;
}
