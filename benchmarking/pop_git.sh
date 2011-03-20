GIT_DIR=$1
DATA_DIR=$2

echo "Using "$GIT_DIR" as the git directory and "\
$DATA_DIR" as the data directory to populate"
USAGE="sript git_dir mount_dir"

if [ -z $GIT_DIR ]
then
	echo $USAGE
	exit 1
fi
	
if [ -z $DATA_DIR ]
then
	echo $USAGE
	exit 1
fi

if [ -d $GIT_DIR ]
then
	echo "Git dir "$GIT_DIR"  exist. Deleting ...."
	rm -Rf $GIT_DIR
fi

echo "Creating git dir"
mkdir $GIT_DIR

echo "Initializing git .... "
cd $GIT_DIR
git init
cd -

OPS=`cat INDEX`


for o in $OPS
do
	echo $o
	FILE=`echo $o | cut -d \: -f 1`
	OP=`echo $o | cut -d \: -f 2`
	# tell what you are about to do
	echo $OP" the file "$FILE

	case $OP in
	"add")
		cp $DATA_DIR/$FILE $GIT_DIR/$FILE
		cd $GIT_DIR
		git add $GIT_DIR/$FILE
		git commit -a -m \"$FILE-$OP\"
		cd -
		;;
	"commit")
		FILE_PART=`echo $FILE| cut -d \. -f 1`.base
		cp $DATA_DIR/$FILE $GIT_DIR/$FILE_PART
		cd $GIT_DIR
		git commit -a -m \"$FILE-$OP\"
		cd -
		;;
	esac
done

# find the efficiency of the system

TOTAL_USAGE=`du -c $GIT_DIR | tail -n 1 | cut -f 1`

USEFUL_USAGE=`du -c $GIT_DIR --exclude=*\.git* | tail -n 1 | cut -f 1`

echo "The total usage is( in bytes ): "$TOTAL_USAGE
echo "The usefule usage is( in bytes ): "$USEFUL_USAGE

