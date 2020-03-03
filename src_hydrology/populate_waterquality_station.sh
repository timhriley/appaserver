:
# src_hydrology/populate_waterquality_station.sh
# ------------------------------------------------
# Freely available software. See appaserver.org
# ------------------------------------------------

echo "select station from station;"	|
sql.e waterquality			|
sort

exit 0
