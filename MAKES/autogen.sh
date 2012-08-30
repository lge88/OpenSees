BUILD_DIR=${1:-Build}
MKDEF_FILE=${2:-Makefile.def.LINUX-CENTOS-5.8-DEBUG}
# BUILD_DIR=build-test1
# MKDEF_FILE=Makefile.def.LINUX-CENTOS-5.8-DEBUG
SAVED_DIR=$(pwd)
mkdir -p ${BUILD_DIR}
mkdir -p ${BUILD_DIR}/lib
mkdir -p ${BUILD_DIR}/bin
cd ..
find . \( -name "Makefile*" -o -name "*.sh" \) -not -wholename "*MAKES*" | cpio -pmdv ${SAVED_DIR}/${BUILD_DIR}
# cp $(SAVED_DIR)/Makefile.def.LINUX-CENTOS-5.8-DEBUG $(SAVED_DIR)/$(BUILD_DIR)/Makefile.def
s=${SAVED_DIR}/${BUILD_DIR};
cat ${SAVED_DIR}/${MKDEF_FILE} | sed "s|\(BUILD_DIR\s\+\)=.*|\1= $s|" > $s/Makefile.def
# cat ./BUILD/Makefile.def | sed "s/\(BUILD_DIR\s\+\)=.*/\1= $\{PWD\}/"
