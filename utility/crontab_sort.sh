:
# $APPASERVER_HOME/utility/crontab_sort.sh
# ---------------------------------------------------------------
# No warranty and freely available software. Visit appaserver.org
# ---------------------------------------------------------------

# Use new delimiter of "~" and hope it's not used.
# ------------------------------------------------

crontab -l					|
grep -v '^#'					|
grep -v '^$'					|
column2pipe.e 2 '~'				|
# -----------------------------------------
# If it runs every hour, then set to hour=0
# -----------------------------------------
sed 's/~\*~/~0~/1'				|
# ---------------------------
# Remove and additional times
# ---------------------------
sed 's/,[0-9][0-09]*~/~/'			|
# --------------------
# Remove leading zeros
# --------------------
sed 's/\(0\)\([0-9]\)/\2/'			|
# --------------------------------
# Swap minutes with hours for sort
# --------------------------------
piece.e '~' 1,0,2				|
# ----------------------------
# Remove leading zeros (again)
# ----------------------------
sed 's/\(0\)\([0-9]\)/\2/'			|
# ----------------
# Perform the sort
# ----------------
sort -t '~' -n -k 1,2				|
# ----------------------------
# Swap back minutes with hours
# ----------------------------
piece.e '~' 1,0,2				|
# ----------------------------------------
# Change the two delimiters back to spaces
# ----------------------------------------
sed 's/~/ /g'					|
cat


exit 0
