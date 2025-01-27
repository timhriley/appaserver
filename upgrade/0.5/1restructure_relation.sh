#!/bin/sh
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

attribute=folder_set_yn
echo "alter table relation drop $attribute;" | sql.e 2>/dev/null
echo "delete from folder_attribute where attribute = '$attribute';" | sql.e
echo "delete from attribute where attribute = '$attribute';" | sql.e

attribute=soft_relation_yn
echo "alter table relation drop $attribute;" | sql.e 2>/dev/null
echo "delete from folder_attribute where attribute = '$attribute';" | sql.e
echo "delete from attribute where attribute = '$attribute';" | sql.e

attribute=related_1tom_pair_yn
echo "alter table relation drop $attribute;" | sql.e 2>/dev/null
echo "delete from folder_attribute where attribute = '$attribute';" | sql.e
echo "delete from attribute where attribute = '$attribute';" | sql.e

attribute=pair_1tom_yn
echo "alter table relation drop $attribute;" | sql.e 2>/dev/null
echo "delete from folder_attribute where attribute = '$attribute';" | sql.e
echo "delete from attribute where attribute = '$attribute';" | sql.e

attribute=omit_mto1_drop_down_yn
echo "alter table relation drop $attribute;" | sql.e 2>/dev/null
echo "delete from folder_attribute where attribute = '$attribute';" | sql.e
echo "delete from attribute where attribute = '$attribute';" | sql.e

attribute=preprompt_drop_down_yn
echo "alter table relation drop $attribute;" | sql.e 2>/dev/null
echo "delete from folder_attribute where attribute = '$attribute';" | sql.e
echo "delete from attribute where attribute = '$attribute';" | sql.e

attribute=mto1_preprompt_drop_down_yn
echo "alter table relation drop $attribute;" | sql.e 2>/dev/null
echo "delete from folder_attribute where attribute = '$attribute';" | sql.e
echo "delete from attribute where attribute = '$attribute';" | sql.e

attribute=post_change_javascript_function
echo "alter table relation drop $attribute;" | sql.e 2>/dev/null
echo "delete from folder_attribute where attribute = '$attribute';" | sql.e
echo "delete from attribute where attribute = '$attribute';" | sql.e

attribute=omit_mto1_edit_drop_down_yn
echo "alter table relation drop $attribute;" | sql.e 2>/dev/null
echo "delete from folder_attribute where attribute = '$attribute';" | sql.e
echo "delete from attribute where attribute = '$attribute';" | sql.e

attribute=filter_drop_down_yn
echo "alter table relation drop $attribute;" | sql.e 2>/dev/null
echo "delete from folder_attribute where attribute = '$attribute';" | sql.e
echo "delete from attribute where attribute = '$attribute';" | sql.e

attribute=no_initial_capital_yn
echo "alter table relation drop $attribute;" | sql.e 2>/dev/null
echo "delete from folder_attribute where folder = 'relation' and attribute = '$attribute';" | sql.e

attribute=pair_1to1_yn
echo "alter table relation drop $attribute;" | sql.e 2>/dev/null
echo "delete from folder_attribute where attribute = '$attribute';" | sql.e
echo "delete from attribute where attribute = '$attribute';" | sql.e

attribute=prompt_mto1_recursive_yn
echo "alter table relation drop $attribute;" | sql.e 2>/dev/null
echo "delete from folder_attribute where attribute = '$attribute';" | sql.e
echo "delete from attribute where attribute = '$attribute';" | sql.e

old_attribute=pair_1tom_order
new_attribute=pair_one2m_order
echo "alter table relation change $old_attribute $new_attribute int;" | sql.e 2>/dev/null
echo "update folder_attribute set attribute = '$new_attribute' where attribute = '$old_attribute';" | sql.e
echo "update attribute set attribute = '$new_attribute' where attribute = '$old_attribute';" | sql.e

old_attribute=join_1tom_each_row_yn
new_attribute=join_one2m_each_row_yn
echo "alter table relation change $old_attribute $new_attribute char(1);" | sql.e 2>/dev/null
echo "update folder_attribute set attribute = '$new_attribute' where attribute = '$old_attribute';" | sql.e
echo "update attribute set attribute = '$new_attribute' where attribute = '$old_attribute';" | sql.e

old_attribute=omit_lookup_before_drop_down_yn
new_attribute=omit_drillthru_yn
echo "alter table relation change $old_attribute $new_attribute char(1);" | sql.e 2>/dev/null
echo "update folder_attribute set attribute = '$new_attribute' where attribute = '$old_attribute';" | sql.e
echo "update attribute set attribute = '$new_attribute' where attribute = '$old_attribute';" | sql.e

old_attribute=omit_1tom_detail_yn
new_attribute=omit_drilldown_yn
echo "alter table relation change $old_attribute $new_attribute char(1);" | sql.e 2>/dev/null
echo "update folder_attribute set attribute = '$new_attribute' where attribute = '$old_attribute';" | sql.e
echo "update attribute set attribute = '$new_attribute' where attribute = '$old_attribute';" | sql.e

echo "update folder_attribute set display_order = 1 where folder = 'relation' and attribute = 'pair_one2m_order';" | sql.e
echo "update folder_attribute set display_order = 2 where folder = 'relation' and attribute = 'omit_drillthru_yn';" | sql.e
echo "update folder_attribute set display_order = 3 where folder = 'relation' and attribute = 'omit_drilldown_yn';" | sql.e
echo "update folder_attribute set display_order = 4 where folder = 'relation' and attribute = 'relation_type_isa';" | sql.e
echo "update folder_attribute set display_order = 5 where folder = 'relation' and attribute = 'copy_common_attributes_yn';" | sql.e
echo "update folder_attribute set display_order = 6 where folder = 'relation' and attribute = 'automatic_preselection_yn';" | sql.e
echo "update folder_attribute set display_order = 7 where folder = 'relation' and attribute = 'drop_down_multi_select_yn';" | sql.e
echo "update folder_attribute set display_order = 8 where folder = 'relation' and attribute = 'join_one2m_each_row_yn';" | sql.e
echo "update folder_attribute set display_order = 9 where folder = 'relation' and attribute = 'ajax_fill_drop_down_yn';" | sql.e
echo "update folder_attribute set display_order = 10 where folder = 'relation' and attribute = 'hint_message';" | sql.e

exit 0
