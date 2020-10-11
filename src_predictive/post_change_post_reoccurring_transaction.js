// src_predictive/post_change_post_reoccurring_transaction.js
// ----------------------------------------------------------------

function post_change_post_reoccurring_transaction( current_element )
{
	var element;
	var entity_street_address_split;
	var entity_street_address;
	var options;

	element = timlib_get_element( "full_name^street_address_0" );

	value = timlib_get_drop_down_element_value( element.options );

	if ( value == 'undefined' || value == 'select' )
		return true;

	entity_street_address_split = value.split( '^' );
	entity_street_address = entity_street_address_split[ 1 ];

	element = timlib_get_element( "rental_property_street_address_0" );
	element.disabled = 0;
	options = element.options;

        for( var i = 0; i < options.length; i++ )
        {
                if ( options[ i ].value == entity_street_address )
                {
			element.disabled = 1;
			timlib_set_drop_down_element_value( options, "Select" )
                }
        }
	return true;

} // post_change_post_reoccurring_transaction()

