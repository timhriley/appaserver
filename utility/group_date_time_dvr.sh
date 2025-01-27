:
# utility/group_date_time_dvr.sh
# ------------------------------

group_date_time.e	value_offset=2 				\
			delimiter='|' 				\
			function=percent_change_base_first 	\
<<- done
	2000-01-01|0000|2.0
	2000-01-01|0000|3.0
	2000-01-01|0010|2.3
	2000-01-01|0020|2.0
	2000-01-01|0020|3.0
	2000-01-01|0020|3.5
	2000-01-01|0020|3.5
done

exit 0
