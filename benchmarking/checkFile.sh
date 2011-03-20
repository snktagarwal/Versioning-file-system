if [ `file -b $1 | cut -d \  -f 1` = 'ASCII' ]
then
exit 0
fi
exit 1
