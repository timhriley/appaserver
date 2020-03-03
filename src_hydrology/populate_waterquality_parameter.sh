:
# src_hydrology/populate_waterquality_parameter.sh
# ------------------------------------------------
# Freely available software. See appaserver.org
# ------------------------------------------------

echo "select parameter from parameter;"	|
sql.e waterquality			|
sort

exit 0
