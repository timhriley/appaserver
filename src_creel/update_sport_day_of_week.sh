:

select="fishing_purpose,census_date,interview_location,'day_of_week',''"
table="creel_census"
fishing_where="fishing_purpose = 'sport' and day_of_week is null"

#date_where="census_date like '%-01-01'"
date_where="1 = 1"

where="$fishing_where and $date_where"
key="fishing_purpose,census_date,interview_location"

select.sh "$select" $table "$where"					|
date2dayofweek.e '^' 1 4						|
while read record
do
	census_date=`echo $record | piece.e '^' 1`
	head=`echo $record | piece_inverse.e 4 '^'`
	day_of_week=`echo $record | piece.e '^' 4`

	year_number=`echo $census_date | piece.e '-' 0`
	month_number=`echo $census_date | piece.e '-' 1`
	day_number=`echo $census_date | piece.e '-' 2`
	day_of_week_record=$day_of_week

	if [ $day_of_week -eq 0 -o $day_of_week = 6 ]
	then
		day_of_week=1
	else
		day_of_week=2
	fi

	# Christmas Saturday
	# ------------------
	if [	$month_number -eq 12 -a					\
		$day_number -ge 24 -a					\
		$day_of_week_record -eq 5 ]
	then
		day_of_week=3
	fi

	# Christmas Sunday
	# ----------------
	if [	$month_number -eq 12 -a					\
		$day_number -ge 26 -a					\
		$day_of_week_record -eq 1 ]
	then
		day_of_week=3
	fi

	# Christmas Weekday
	# -----------------
	if [	$month_number -eq 12 -a					\
		$day_number -ge 25 -a					\
		$day_of_week -eq 2 ]
	then
		day_of_week=3
	fi

	# Independence Saturday
	# ---------------------
	if [	$month_number -eq 7 -a					\
		$day_number -ge 3 -a					\
		$day_of_week_record -eq 5 ]
	then
		day_of_week=3
	fi

	# Independence Sunday
	# -------------------
	if [	$month_number -eq 7 -a					\
		$day_number -ge 5 -a					\
		$day_of_week_record -eq 1 ]
	then
		day_of_week=3
	fi

	# Independence Weekday
	# --------------------
	if [	$month_number -eq 7 -a					\
		$day_number -ge 4 -a					\
		$day_of_week -eq 2 ]
	then
		day_of_week=3
	fi

	# New Years Saturday
	# ------------------
	if [	$month_number -eq 12 -a					\
		$day_number -ge 31 -a					\
		$day_of_week_record -eq 5 ]
	then
		day_of_week=3
	fi

	# New Years Sunday
	# ----------------
	if [	$month_number -eq 1 -a					\
		$day_number -ge 2 -a					\
		$day_of_week_record -eq 1 ]
	then
		day_of_week=3
	fi

	# New Years Weekday
	# -----------------
	if [	$month_number -eq 1 -a					\
		$day_number -ge 1 -a					\
		$day_of_week -eq 2 ]
	then
		day_of_week=3
	fi

	# MLK
	# ---
	if [	$month_number -eq 1 -a					\
		$year_number -ge 1986 -a				\
		$day_number -ge 15 -a					\
		$day_number -le 21 -a					\
		$day_of_week_record -eq 1 ]
	then
		day_of_week=3
	fi

	# Memorial pre Monday law
	# -----------------------
	if [	$month_number -eq 5 -a					\
		$year_number -le 1970 -a				\
		$day_number -eq 30 ]
	then
		day_of_week=3
	fi

	# Memorial post Monday law
	# ------------------------
	if [	$month_number -eq 5 -a					\
		$year_number -ge 1971 -a				\
		$day_number -ge 24 -a					\
		$day_of_week_record -eq 1 ]
	then
		day_of_week=3
	fi

	# Presidents' pre Monday law
	# --------------------------
	if [	$month_number -eq 2 -a					\
		$year_number -le 1971 -a				\
		$day_number -eq 22 ]
	then
		day_of_week=3
	fi

	# Presidents' post Monday law
	# ---------------------------
	if [	$month_number -eq 2 -a					\
		$year_number -ge 1972 -a				\
		$day_number -ge 15 -a					\
		$day_number -le 21 -a					\
		$day_of_week_record -eq 1 ]
	then
		day_of_week=3
	fi

	# Labor day
	# ---------
	if [	$month_number -eq 9 -a					\
		$day_number -le 7 -a					\
		$day_of_week_record -eq 1 ]
	then
		day_of_week=3
	fi

	# Veterans Holiday Law slice 
	# --------------------------
	if [	$year_number -eq 1971 -a				\
		$month_number -eq 10 -a					\
		$day_number -eq 25 -a					\
		$day_of_week -eq 2 ]
	then
		day_of_week=3
	fi

	if [	$year_number -eq 1972 -a				\
		$month_number -eq 10 -a					\
		$day_number -eq 23 -a					\
		$day_of_week -eq 2 ]
	then
		day_of_week=3
	fi

	if [	$year_number -eq 1973 -a				\
		$month_number -eq 10 -a					\
		$day_number -eq 22 -a					\
		$day_of_week -eq 2 ]
	then
		day_of_week=3
	fi

	if [	$year_number -eq 1974 -a				\
		$month_number -eq 10 -a					\
		$day_number -eq 28 -a					\
		$day_of_week -eq 2 ]
	then
		day_of_week=3
	fi

	if [	$year_number -eq 1975 -a				\
		$month_number -eq 10 -a					\
		$day_number -eq 27 -a					\
		$day_of_week -eq 2 ]
	then
		day_of_week=3
	fi

	if [	$year_number -eq 1976 -a				\
		$month_number -eq 10 -a					\
		$day_number -eq 25 -a					\
		$day_of_week -eq 2 ]
	then
		day_of_week=3
	fi

	if [	$year_number -eq 1977 -a				\
		$month_number -eq 10 -a					\
		$day_number -eq 24 -a					\
		$day_of_week -eq 2 ]
	then
		day_of_week=3
	fi

	# Veterans Non-weekend
	# --------------------
	if [ $year_number -lt 1971 -o $year_number -gt 1977 ]
	then
		if [	$month_number -eq 11 -a				\
			$day_number -eq 11 -a				\
			$day_of_week -eq 2 ]
		then
			day_of_week=3
		fi
	fi

	# Veterans Saturday
	# -----------------
	if [	$month_number -eq 11 -a					\
		$day_number -eq 10 -a					\
		$day_of_week_record -eq 5 -a				\
		$day_of_week -eq 1 ]
	then
		day_of_week=3
	fi

	# Veterans Sunday
	# ---------------
	if [	$month_number -eq 11 -a					\
		$day_number -eq 12 -a					\
		$day_of_week_record -eq 1 -a				\
		$day_of_week -eq 1 ]
	then
		day_of_week=3
	fi

	# Columbus day
	# ------------
	if [	$year_number -ge 1971 -a				\
		$month_number -eq 10 -a					\
		$day_number -ge 8 -a					\
		$day_number -le 14 -a					\
		$day_of_week_record -eq 1 ]
	then
		day_of_week=3
	fi

	# Thanksgiving
	# ------------
	if [	$month_number -eq 11 -a					\
		$day_number -ge 24 -a					\
		$day_of_week_record -eq 4 ]
	then
		day_of_week=3
	fi

	echo "${head}^${day_of_week}"					|
	update_statement.e table=$table key=$key carrot=y
done									|
cat

exit 0
