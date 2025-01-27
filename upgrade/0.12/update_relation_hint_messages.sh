#!/bin/bash
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
	echo "$ . set_database" 1>&2
	exit 1
fi


(
cat << shell_all_done

update appaserver_column set hint_message = 'If a one-table has too many rows to effectively appear on the drilldown screen, then set this flag.' where column_name = 'omit_drilldown_yn';

update appaserver_column set hint_message = 'Appaserver provides an \'isa\' relation for a many-table that \'isa\' one-table. When an \'isa\' relationship exists, then the many-table inherits the columns of the one-table. Note: the many-table\'s primary keys must match in quantity the one-table\'s primary keys.' where column_name = 'relation_type_isa_yn';

update appaserver_column set hint_message = 'Pairing two related tables allows you to insert into the many-table at the same time you insert into the one-table.' where column_name = 'pair_one2m_order';

update appaserver_column set hint_message = 'Set this flag if you have a column in the many-table with the same name as a column in the one-table. Appaserver will copy the datum from the one-table when you insert into the many-table. The prime example is to insert the retail price of a sale by copying the retail price from the inventory.' where column_name = 'copy_common_columns_yn';

update relation set hint_message = 'Normally, the foreign keys are the same names as their corresponding primary keys. Moreover, they are in the same order as their corresponding primary keys. Frequently, the last foreign key may differ. If only the last foreign key differs, then set related_column. Infrequently, the foreign keys are not in the same order, or multiple foreign key names differ. If so, then populate FOREIGN_COLUMN.' where table_name = 'relation' and related_table = 'column' and related_column = 'related_column';

update appaserver_column set hint_message = 'When inserting new rows, you may want to preselect only the many-table rows that join to the one-table. When this flag is set, the edit screen appears like a spreadsheet, allowing you to easily insert an entire block of data at a time. ' where column_name = 'automatic_preselection_yn';

shell_all_done
) | sql.e
exit 0
