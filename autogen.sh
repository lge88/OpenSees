SAVED_DIR=$(pwd)
BUILD_DIR=${1:-BUILD/debug}
MKDEF_FILE=${2:-Makefile.def.LINUX-CENTOS-5.8-DEBUG}
MAKES_DIR=${SAVED_DIR}/MAKES
mkdir -p ${BUILD_DIR}
mkdir -p ${BUILD_DIR}/lib
mkdir -p ${BUILD_DIR}/bin
find . \( -name "Makefile*" -o -name "OSver*.sh" \) -not -wholename "*MAKES*" | cpio -pmdv ${SAVED_DIR}/${BUILD_DIR}
s=${SAVED_DIR}/${BUILD_DIR};
cat ${MAKES_DIR}/${MKDEF_FILE} | sed "s|\(BUILD_DIR\s\+\)=.*|\1= $s|" > $s/Makefile.def
