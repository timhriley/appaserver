:
get_folder_data	application=orderentry			 			folder=inventory			 			select="inventory_name,retail_price"		 			where_clause="1 = 1"		 			maxrows=10000 2>>/var/log/appaserver_orderentry.err		|
cat			|
tr '^' '^'		|
sed 's/\^/ [/1' 	|
sed 's/$/]/'		|
sed 's/|/\\|/g'
