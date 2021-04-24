#!/bin/bash


# script core variables
DATASET_FILE=""
TARGET_DIR=""
NUM_FILES_PER_DIR=0
RECORDS=()
NUM_RECORDS=0
COUNTRIES=()
NUM_COUNTRIES=()
declare -A COUNTRY_TO_ID
declare -A RECORDS_PER_COUNTRY
ROUND_ROBINS=()
COUNTRY_ID=-1


####################################################################################################
# parse command line arguments while also checking for mistakes

# prompt to explain the input to the user
PROMPT="./create_infiles.sh inputFile input_dir numFilesPerDirectory"

# check if user wants to see the prompt
if [[ $1 == "-h" || $1 == "--help" ]]; then
    echo -e "\n$PROMPT\n"
    exit 0
fi

# start parsing command line arguments
if [[ $# != 3 ]]; then
    echo -e "\nERROR: Incorrect number of command line arguments.\n\nUsage: ${PROMPT}\n"
    exit 5
fi

# parse the dataset filepath
if [[ ! -e $1 ]]; then
    echo -e "\nERROR: Dataset file \"$1\" does not exist.\n"
    exit 1
fi
DATASET_FILE=$1

# parse the target directory path, and create it recursively if it does not exist
if [[ -e $2 ]] && [[ ! -d $2 ]]; then
    echo -e "\nERROR: The path to the input directory \"$2\" corresponds to an already existing non-directory entity.\n"
    exit 2
elif [[ ! -d $2 ]]; then
    mkdir -p $2
fi
TARGET_DIR=$2

# parse the number of files to be created per subdirectory
if [[ $3 -le 0 ]]; then
    echo -e "\nERROR: Number of files per sub-directory should be a positive integer.\n"
    exit 3
fi
NUM_FILES_PER_DIR=$3



####################################################################################################
# utility functions

read_dataset_file()
{
    RECORDS=()
    while IFS= read -r line; do
        RECORDS+=("$line")
    done < $DATASET_FILE
    NUM_RECORDS=${#RECORDS[@]}
}


get_countries()
{
    COUNTRIES=()
    NUM_COUNTRIES=0
    
    for (( _REC = 0; _REC < NUM_RECORDS; _REC++ )); do
        # tokenize record
        TOKENS=( ${RECORDS[_REC]} )
        NUM_TOKENS=${#TOKENS[@]}
        if (( NUM_TOKENS != 7 && NUM_TOKENS != 8 )); then
            continue
        fi

        # get the country and map it to it's ID
        COUNTRY=${TOKENS[3]}
        if [[ ! -v "COUNTRY_TO_ID[$COUNTRY]" ]]; then
            COUNTRIES+=($COUNTRY)
            COUNTRY_TO_ID[$COUNTRY]=$NUM_COUNTRIES
            RECORDS_PER_COUNTRY[$COUNTRY]=0
            NUM_COUNTRIES=$((NUM_COUNTRIES + 1))
        fi
        RECORDS_PER_COUNTRY[$COUNTRY]=$((${RECORDS_PER_COUNTRY[$COUNTRY]} + 1))
    done
}


create_countries_dirs_and_files()
{
    # for each country
    for (( C_ID = 0; C_ID < NUM_COUNTRIES; C_ID++ )); do

        # create directory
        COUNTRY=${COUNTRIES[$C_ID]}
        _PATH="$TARGET_DIR/$COUNTRY"
        if [[ -e $_PATH ]]; then
            rm -rf $_PATH
        fi
        mkdir $_PATH

        # create empty files in directory
        COUNTRY_RECS=${RECORDS_PER_COUNTRY[$COUNTRY]}
        NUM_FILES=$(( COUNTRY_RECS < NUM_FILES_PER_DIR ? COUNTRY_RECS : NUM_FILES_PER_DIR ))
        for (( F_ID = 1; F_ID <= NUM_FILES; F_ID++ )); do
            touch "$_PATH/$COUNTRY-$F_ID.txt"
        done
    done
}


write_data()
{
    # keep track of where the last record (per country) was written
    ROUND_ROBINS=("${COUNTRIES[@]/*/0}")

    # for every record
    for (( _REC = 0; _REC < NUM_RECORDS; _REC++ )); do
        # tokenize record
        RECORD=${RECORDS[_REC]}
        TOKENS=( $RECORD )
        NUM_TOKENS=${#TOKENS[@]}
        if (( NUM_TOKENS != 7 && NUM_TOKENS != 8 )); then
            echo "ERROR in record: ${RECORDS[_REC]}"
            continue
        fi

        # get the country and it's ID
        COUNTRY=${TOKENS[3]}
        COUNTRY_ID=${COUNTRY_TO_ID[$COUNTRY]}

        # construct the filepath where the current record will be placed
        F_ID=$(( ${ROUND_ROBINS[$COUNTRY_ID]} + 1 ))
        FILEPATH="$TARGET_DIR/$COUNTRY/$COUNTRY-$F_ID.txt"
        echo $RECORD >> $FILEPATH
        ROUND_ROBINS[$COUNTRY_ID]=$(( (${ROUND_ROBINS[$COUNTRY_ID]} + 1) % NUM_FILES_PER_DIR ))
    done
}



####################################################################################################
# main driver mechanism

read_dataset_file
get_countries
create_countries_dirs_and_files
write_data
