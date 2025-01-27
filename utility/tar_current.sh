:
stamp=`current_date_backward.sh`
base=`pwd | xargs basename`
tar_file=/tmp/${base}_${stamp}.tar.gz

cd ..
tar cvzf $tar_file $base

echo
echo "Created $tar_file"
