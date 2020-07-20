:
# appaserver_tz.sh
# ----------------

if [ "$DATABASE" = "" -a "$APPASERVER_DATABASE" = "" ]
then
	export DATABASE=template
fi

utc_offset=`echo "	select application_constant_value		\
			from application_constants			\
			where application_constant = 'utc_offset';"	|
			sql.e | sed 's/^-//'`

# -----------------------------------------
# TZ usually looks like EST5EDT or PST8PDT.
# But date takes XXX5XXX or XXX8XXX okay.
# -----------------------------------------
echo "XXX${utc_offset}XXX"

exit 0
