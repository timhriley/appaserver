:
# $APPASERVER_HOME/utility/graphviz_render.sh
# ---------------------------------------------
# Freely available software: see Appaserver.org
# ---------------------------------------------


if [ "$#" -ne 1 ]
then
	echo "Usage: $0 filename_label" 1>&2
	exit 1
fi

filename_label=$1

dot -Tps ${filename_label}.gv -o ${filename_label}.ps
#ps2pdf ${filename_label}.ps ${filename_label}.pdf

exit 0
