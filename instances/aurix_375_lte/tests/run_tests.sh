#!/bin/sh

YELLOW="\e[33m"
GREEN="\e[32m"
RED="\e[31m"
MAGENTA="\e[35m"
ENDCOLOR="\e[0m"

test_root=$(pwd)
all_tests=$(/bin/ls -d ./tests/*/ 2>/dev/null )
flash_board_script=../flash_board.sh
port=$(cat ./board_ports.txt | grep "PORT" | cut -d':' -f2)
baud=$(cat ./board_ports.txt | grep "BAUD" | cut -d':' -f2)

end_tests() {
    cd $test_root
}

build_run_logic_test()
{
  cd host_src
  cargo run ${port} ${baud}
  cd ..
}

run_test() {
    echo -e ${MAGENTA}testing $(echo $1 | cut -d'/' -f1) $ENDCOLOR
    cd $1

    echo -e ${YELLOW}building in DEBUG mode $ENDCOLOR
    cd "hardware_src/TriCore Debug (GCC)"
    wine make --output-sync -j8 all # > /dev/null
    C_tricore-probe basic_aurix_template.elf  # > /dev/null
    echo -e ${GREEN}running in DEBUG mode $ENDCOLOR
    cd - > /dev/null
    build_run_logic_test build_debug

    echo -e ${YELLOW}building in RELEASE mode $ENDCOLOR
    cd "hardware_src/TriCore Release (GCC)"
    wine make --output-sync -j8 all # > /dev/null
    C_tricore-probe basic_aurix_template.elf  # > /dev/null
    echo -e ${GREEN}running in RELEASE mode $ENDCOLOR
    cd - > /dev/null
    build_run_logic_test build_release
}

if [ ! -d ./.dummy -o ! -d ./tests ]; then
    echo "run the script inside the test directory"
    exit 2
fi

skip_tests=""

help ()
{
  echo "usage ${0}:
    -h        : print help
    -s args.. : skip a list of tests 
    -t arg..  : run a list of tests
    --all     : run all tests sequentially
    
    If no argument is passed all test are runned sequentially"
}

print_list()

{
  echo -n "["
  for ARG in "$@"
  do
    echo -n " $ARG "
  done
  echo "]"
}

case "$1" in
  "-h") 
    help
    exit 0
  ;;
  "-s") 
    echo -n "skipping tests: "
    shift
    skip_tests=${@}
    print_list $skip_tests
  ;;
  "-t") echo -n "executing tests: "
    shift
    skip_tests=${@}
    print_list $skip_tests
    cd ./tests/
    for TEST_DIR in $skip_tests; do
      curr=$(echo $TEST_DIR| cut -d'/' -f1)
      if [[ -n $(echo "$all_tests" | grep -w "${curr}") ]]; then
        run_test $TEST_DIR
      else
        echo -e ${RED}test $(echo ${curr} | cut -d'/' -f1) not found $RED
      fi
      cd $test_root/tests
    done
    end_tests

    exit 0
  ;;
  "--all") echo "running all tests"
  ;;
  *) 
    help
    exit 0
  ;;
esac



cd tests
tests_dir=$(pwd)
for TEST_DIR in $(/bin/ls -d */ 2>/dev/null ); do
  curr=$(echo $TEST_DIR| cut -d'/' -f1)
  if [[ -z $(echo "$skip_tests" | grep -w "${curr}") ]]; then
    run_test $TEST_DIR
  else
    echo -e ${RED}skipping $(echo ${curr} | cut -d'/' -f1) $RED
  fi
  end_tests
  cd ${tests_dir}
done

end_tests

exit 0
