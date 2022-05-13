#!/bin/sh
# ---------------------------------------------
# src_hydrology/datatype_cfs_list.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

table="datatype"
where="units = 'cfs'"

	echo "select concat( datatype, ' [', units, ']')	\
      	from $table		 				\
	where $where						\
      	order by datatype;" 					|
	sql.e

exit 0
