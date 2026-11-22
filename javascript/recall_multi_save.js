// $APPASERVER_HOME/javascript/recall_multi_save.js
// ---------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// ---------------------------------------------------------------

class recall_multi_save {
constructor(
	cookie_key,
	samesite_string,
	secure_string,
	multi_element_name_delimited_string,
	recall_name_delimiter /* Probably ~ */,
	recall_equal_delimiter /* Probably == */,
	recall_block_delimiter /* Probably ~~ */,
	recall_multi_delimiter /* Probably && */,
	recall_multi_index_delimiter /* Probably | */ )
{
	var save_element_string;

	let multi_element_name_array =
		multi_element_name_delimited_string.split(
			recall_name_delimiter );

	this.cookie_multi_save_element_array =
		new cookie_multi_save_element_array(
			recall_equal_delimiter,
			recall_block_delimiter,
			recall_multi_delimiter,
			recall_multi_index_delimiter,
			multi_element_name_array );

	if ( this.cookie_multi_save_element_array.array.length )
	{
		this.cookie_multi_save =
			new cookie_multi_save(
				cookie_key,
				samesite_string,
				secure_string,
				recall_equal_delimiter,
				recall_multi_delimiter,
				this.cookie_multi_save_element_array.array
					/* cookie_multi_save_element_array */ );

		document.cookie =
			this.
				cookie_multi_save.
				cookie_save.
				cookie_string;
	}
}
}

