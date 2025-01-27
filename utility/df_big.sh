:
# df_big.sh
# ---------
#
# -- Tim Riley
# ------------
df -b -e 			|
grep kilobytes 			|
# -------------------------	|
# Isolate local filesystems	|
# -------------------------	|
grep -v ':/'			|
grep -v '(/export'		|
#				|
# ---------------		|
# Clean things up		|
# ---------------		|
sed 's/)://'			|
sed 's/(//'			|
sort -r -n -k 3 		|
cat
