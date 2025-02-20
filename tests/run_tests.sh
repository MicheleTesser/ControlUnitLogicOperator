#!/bin/sh

YELLOW="\e[33m"
GREEN="\e[32m"
RED="\e[31m"
MAGENTA="\e[35m"
ENDCOLOR="\e[0m"

test_root=$(pwd)
all_tests=$(/bin/ls -d ./tests/*/ 2>/dev/null )

end_tests() {
    cd $test_root
    ./setup_test_env.sh "close"
}

run_test() {
    echo -e ${MAGENTA}testing $(echo $1 | cut -d'/' -f1) $ENDCOLOR
    cd $1
    echo -e ${YELLOW}building in DEBUG mode $ENDCOLOR
    mkdir build 2>/dev/null
    cd build
    rm -rf ./*
    cmake .. 1>/dev/null
    make build_debug -j8 1>/dev/null
    echo -e ${GREEN}running in DEBUG mode $ENDCOLOR
    cd debug
    ./main
    cd ..
    make clean 1>/dev/null
    echo -e ${YELLOW}building in RELEASE mode $ENDCOLOR
    make build_release -j8 1>/dev/null
    echo -e ${GREEN}running in RELEASE mode $ENDCOLOR
    cd release
    ./main
    cd ..
}

if [ ! -d ./linux_board -o ! -d ./.dummy ]; then
    echo "run the script inside the test directory"
    exit 2
fi

if [ ! -f /usr/lib/libgpiod.so ]; then
  echo "libgpiod is not installed in your system"
  exit 3
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
    ./setup_test_env.sh "init"
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



./setup_test_env.sh "init"
cd tests
for TEST_DIR in $(/bin/ls -d */ 2>/dev/null ); do
  curr=$(echo $TEST_DIR| cut -d'/' -f1)
  if [[ -z $(echo "$skip_tests" | grep -w "${curr}") ]]; then
    run_test $TEST_DIR
  else
    echo -e ${RED}skipping $(echo ${curr} | cut -d'/' -f1) $RED
  fi
  cd $test_root/tests
done

end_tests

exit 0
