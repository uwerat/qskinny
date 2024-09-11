#! /bin/sh

if [ $# -eq 0 ]; then
    echo "Usage $0 file ..."
    exit 1
fi

for file in $*
do
    base=`basename -s .svg $file`
    echo "${base}.svg -> qvg/${base}.qvg"
    svg2qvg ${base}.svg qvg/${base}.qvg
done

exit $status
