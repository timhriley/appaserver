:
find . -xdev -type f -print | xargs du -sk | sort -rn | head -25
