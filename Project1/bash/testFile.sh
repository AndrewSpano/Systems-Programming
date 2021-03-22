#!/bin/bash

# script constants
DEFAULT_TARGET_FILEPATH="./tests/inputFile.txt"
MIN_NAME_SURNAME_LEN=3
MAX_NAME_SURNAME_LEN=12
CHARACTERS=(a b c d e f g h i j k l m n o p q r s t u v w x y z
            A B C D E F G H I J K L M N O P Q R S T U V W X Y Z)
NUM_CHARACTERS=${#CHARACTERS[@]}
MIN_AGE=1
MAX_AGE=120
MIN_DAY=1
MAX_DAY=30
MIN_MONTH=1
MAX_MONTH=12
MIN_YEAR=0
MAX_YEAR=2021
MAX_DUPLICATES=10

# script core variables
VIRUSES_FILE=""
COUNTRIES_FILE=""
NUM_LINES=0
DUPLICATES_ALLOWED=""
VIRUSES=()
NUM_VIRUSES=-1
COUNTRIES=()
NUM_COUNTRIES=-1
CITIZEN_ID=0
NAME=""
OG_NAME=""
SURNAME=""
COUNTRY=""
AGE=""
VIRUS=""
_STATUS=""
STATUS=""
DATE=""

# random used for deciding whether an entry will be vaccinated or not
RANDOM=$$


####################################################################################################
# parse command line arguments while also checking for mistakes

# prompt to explain the input to the user
PROMPT="./testFile.sh virusesFile countriesFile numLines duplicatesAllowed"

# check if user wants to see the prompt
if [[ $1 == "-h" || $1 == "--help" ]]; then
  echo -e "\n$PROMPT\n"
  exit 0
fi

# start parsing command line arguments
if [[ $# != 4 ]]; then
  echo -e "\nERROR: Incorrect number of command line arguments.\n\nUsage: ${PROMPT}\n"
  exit 5
fi

# parse the viruses filepath
if [[ ! -e $1 ]]; then
  echo -e "\nERROR: Viruses file $1 does not exist.\n"
  exit 1
fi
VIRUSES_FILE=$1

# parse the countries filepath
if [[ ! -e $2 ]]; then
  echo -e "\nERROR: Countries file $2 does not exist.\n"
  exit 2
fi
COUNTRIES_FILE=$2

# parse the number of lines which the created datafile will have
if [[ $3 -le 0 ]]; then
  echo -e "\nERROR: Number of lines should be a positive integer.\n"
  exit 3
fi
NUM_LINES=$3

# parse the flag that indicates whether duplicate records should be created
DUPLICATES_ALLOWED=$4



####################################################################################################
# utility functions

# reads the countries file and stores the names of the countries in the $COUNTRIES variable
read_countries_file()
{
  COUNTRIES=()
  while read line; do
    COUNTRIES+=($line)
  done < $COUNTRIES_FILE
  NUM_COUNTRIES=${#COUNTRIES[@]}
}

# reads the virus file and stores the names of the viruses in the $VIRUSES variable
read_viruses_file()
{
  VIRUSES=()
  while read line; do
    VIRUSES+=($line)
  done < $VIRUSES_FILE
  NUM_VIRUSES=${#VIRUSES[@]}
}

# create a random name/surname and save it in the $name_or_surname variable
create_random_name_or_surname()
{
  NAME=""
  RANDOM_LEN=$(shuf -i "${MIN_NAME_SURNAME_LEN}-${MAX_NAME_SURNAME_LEN}" -n '1')
  for (( CH = 0; CH < RANDOM_LEN; CH++ )); do
    RANDOM_CHARACTER=${CHARACTERS[$(shuf -i "0-$((NUM_CHARACTERS - 1))" -n '1')]}
    NAME+=${RANDOM_CHARACTER}
  done
}

# create random status (0 -> "NO", 1 -> "YES")
create_random_status()
{
  if [[ $((${RANDOM} % 2)) -eq 0 ]]; then
    _STATUS="NO"
  else
    _STATUS="YES"
  fi
}

# create a random date of the format: DD-MM-YYYY
create_random_date()
{
  DATE=""

  # create a random day, keep appending 0s until it has 2 characters (e.g. 1 -> 01)
  RANDOM_DAY=$(shuf -i "${MIN_DAY}-${MAX_DAY}" -n '1')
  while [[ ${#RANDOM_DAY} -ne 2 ]]; do
    RANDOM_DAY='0'${RANDOM_DAY}
  done

  # create a random month, keep appending 0s until it has 2 characters (e.g. 9 -> 09)
  RANDOM_MONTH=$(shuf -i "${MIN_MONTH}-${MAX_MONTH}" -n '1')
  while [[ ${#RANDOM_MONTH} -ne 2 ]]; do
    RANDOM_MONTH='0'${RANDOM_MONTH}
  done

  # create a random year, keep appending 0s until it has 4 characters (e.g. 420 -> 0420)
  RANDOM_YEAR=$(shuf -i "${MIN_YEAR}-${MAX_YEAR}" -n '1')
  while [[ ${#RANDOM_YEAR} -ne 4 ]]; do
    RANDOM_YEAR='0'${RANDOM_YEAR}
  done

  # finally create the date
  DATE="${RANDOM_DAY}-${RANDOM_MONTH}-${RANDOM_YEAR}"
}

# create a record using the above helper functions
create_random_record()
{
  # if we should create a new Record and not use the previous one
  if [[ $1 -ne 0 ]]; then

    # citizen ID
    ID=${CITIZEN_ID}

    # name and surname
    create_random_name_or_surname
    OG_NAME=${NAME}
    create_random_name_or_surname
    SURNAME=${NAME}

    # country
    COUNTRY=${COUNTRIES[$(shuf -i "0-$((NUM_COUNTRIES - 1))" -n '1')]}

    # age
    AGE=$(shuf -i "${MIN_AGE}-${MAX_AGE}" -n '1')

  fi

  # virus
  VIRUS=${VIRUSES[$(shuf -i "0-$((NUM_VIRUSES - 1))" -n '1')]}

  # status: "NO" or "YES"
  create_random_status
  STATUS=${_STATUS}

  # create the record
  RECORD="${ID} ${OG_NAME} ${SURNAME} ${COUNTRY} ${AGE} ${VIRUS} ${STATUS}"

  # if status is "YES", add a date also
  if [[ ${STATUS} == "YES" ]]; then
    create_random_date
    RECORD+=" ${DATE}"
  fi

  # if specified by an argument, the citizen ID should be incremented by 1
  if [[ $1 -ne 0 ]]; then
    CITIZEN_ID=$((CITIZEN_ID + 1))
  fi
}

# creates the target file
create_target_file()
{
  if [[ -e ${DEFAULT_TARGET_FILEPATH} ]]; then
    rm -rf ${DEFAULT_TARGET_FILEPATH}
  fi

  touch ${DEFAULT_TARGET_FILEPATH}
}

# add the records to the file
create_dataset()
{
  # array to store all records, then shuffle it and insert it in the target file
  ALL_RECORDS=()

  # write numLines record in the data file
  for (( REC = 0; REC < NUM_LINES; REC++ )); do

    # if duplicates should be added, add them
    if [[ DUPLICATES_ALLOWED -ne 0 ]]; then
      RANDOM_DUPLICATE_NUM=$(shuf -i "1-$((MAX_DUPLICATES - 1))" -n '1')
      for (( DUP_REC = 0; DUP_REC < RANDOM_DUPLICATE_NUM - 1 && REC < NUM_LINES - 1; DUP_REC++ )); do
        create_random_record
        ALL_RECORDS+=("${RECORD}")
        REC=$((REC + 1))
      done
    fi

    # now add the record where the ID will change
    create_random_record 1
    ALL_RECORDS+=("${RECORD}")
  done

  # shuffle the records and finally write them in the outfile
  SHUFFLED_RECORDS=()
  while read -r item; do
      SHUFFLED_RECORDS+=("$item")
  done < <(shuf -e "${ALL_RECORDS[@]}")
  for _RECORD in "${SHUFFLED_RECORDS[@]}"
  do
    echo ${_RECORD} >> ${DEFAULT_TARGET_FILEPATH}
  done
}


####################################################################################################
# main driver mechanism

read_viruses_file
read_countries_file
create_target_file
create_random_record 1
create_dataset
