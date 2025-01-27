:
df -h | grep -v tmpfs | grep -v loop | grep -v udev

exit $?
