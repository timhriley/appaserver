// $APPASERVER_HOME/src_predictive/post_change_journal_ledger.js
// ---------------------------------------------------------------
// No warranty and freely available software. Visit appaserver.org
// ---------------------------------------------------------------

function post_change_journal_ledger( state )
{
	var element_name;
	var debit_element;
	var debit_amount;
	var credit_element;
	var credit_amount;
	var i;
	var first_row_debit;
	var remaining;

	if ( state != 'insert' ) return true;

	// Get first row debit_amount
	// --------------------------
	element_name = 'debit_amount_1';
	debit_element = timlib_element( element_name );

	if ( debit_element == '' ) return false;

	debit_amount = debit_element.value;

	first_row_debit = ( debit_amount > 0 );

	// Get first row credit_amount
	// ---------------------------
	element_name = 'credit_amount_1';
	credit_element = timlib_element( element_name );

	if ( credit_element == '' ) return false;

	credit_amount = credit_element.value;

	if ( first_row_debit && credit_amount > 0 ) return false;

	if ( first_row_debit )
		remaining = debit_amount;
	else
		remaining = credit_amount;

	i = 2;

	while( true )
	{
		// Get next row debit_amount
		// -------------------------
		element_name = 'debit_amount_' + i;
		debit_element = timlib_element( element_name );

		if ( debit_element == '' ) break;

		debit_amount = debit_element.value;

		// Get next row credit_amount
		// --------------------------
		element_name = 'credit_amount_' + i;
		credit_element = timlib_element( element_name );

		if ( credit_element == '' ) break;

		credit_amount = credit_element.value;

		/* Subtract what's there. */
		/* ---------------------- */
		if ( first_row_debit )
		{
			if ( debit_amount > 0 ) return false;

			if ( credit_amount == 0 ) break;

			remaining -= credit_amount;
		}
		else
		{
			if ( credit_amount > 0 ) return false;

			if ( debit_amount == 0 ) break;

			remaining -= debit_amount;
		}

		i++;
	}

	if ( remaining <= 0 ) return true;

	if ( first_row_debit )
	{
		element_name = 'credit_amount_' + i;
		credit_element = timlib_element( element_name );
		credit_element.value = remaining;
	}
	else
	{
		element_name = 'debit_amount_' + i;
		debit_element = timlib_element( element_name );
		debit_element.value = remaining;
	}

	return true;
}

