RVFS_ROOT_DIR=$1
RVFS_MOUNT_DIR=$2
DATA_DIR=$3

echo "Using "$RVFS_ROOT_DIR" as the Rvfs root directory and "\
$DATA_DIR" as the data directory to populate"

if [ -d $RVFS_ROOT_DIR ]
then
	echo "RVFS root dir "$RVFS_ROOT_DIR"  exist. Deleting ...."
	rm -Rf $RVFS_DIR
fi
echo "Creating RVFS root dir"
mkdir $RVFS_ROOT_DIR

echo "Mounting RVFS root to mount dir"
# mount the ROOT dir to the MOUNT dir

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
		;;
	"commit")
		;;
	esac
done

# find the efficiency of the system
