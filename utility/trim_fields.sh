:
# trim_fields.sh
# --------------
# This is a wrapper script for trim_fields.awk
# --------------------------------------------

(
echo one 
echo two 
) | awk -f trim_fields.awk 
