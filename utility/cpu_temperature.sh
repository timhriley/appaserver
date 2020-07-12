:
# -------------------------
# apt -y install ln-sensors
# sensors-detect
# -------------------------
sensors | egrep 'Physical id|Package id' | column.e 2,3
