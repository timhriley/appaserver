:
# -------------------------
# apt -y install ln-sensors
# sensors-detect
# -------------------------
sensors | grep 'Package id' | column.e 2,3
