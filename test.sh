# ./test.sh <zip_file_address>

# Defining colors
RED="\033[0;31m"
GREEN="\033[0;32m"
CYAN="\033[0;36m"
NC='\033[0m' # No Color

TEMP_DIR="bin"
EXECUTABLE_FILE="./EnsembleClassifier.out"
TESTCASE_DIR="../testcases"
INPUT_DIR="$TESTCASE_DIR/in"
OUTPUT_DIR="$TESTCASE_DIR/out"

mkdir -p $TEMP_DIR
unzip -qq $1 -d $TEMP_DIR
cd $TEMP_DIR
make

counter=0
for i in {0..9}
do
    rm -rf validation weight_vectors

    unzip -qq $INPUT_DIR/Assets_$i.zip
  
    if ! $EXECUTABLE_FILE validation weight_vectors | diff -r $OUTPUT_DIR/$i.out -; then
        echo -e "${RED}$i - Failed!${NC}"
    else
        echo -e "${GREEN}$i - Passed!${NC}"
        counter=$(( counter + 1 ));
    fi

done

cd ..
rm -rf $TEMP_DIR

echo -e "\n${CYAN}######## $counter test cases passed! ########${NC}"