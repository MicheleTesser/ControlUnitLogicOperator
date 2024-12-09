#!/bin/sh
 
component_file_h=./components/component.h
dummy_file="./components/.dummy/dummy.h"

help() {
    echo "usage: $0 [command] \<args\>";
    echo -e "command":
    echo -e "\t add: add a new component given a name as args"
    echo -e "\t del: delete an existing component given a name as args if exist, else do nothing"
    echo -e "\t delall: delete all existing components"
    echo -e "\t lst: list all existing components"
}

if [ ! -d ./components -o ! -f ./component_manager.sh ]; then
    echo "run the script inside of the project root directory"
    exit -1
fi

name=$2
upp_case=$(echo $name | tr a-z A-Z)

case $1 in
    "add")
        if [ -z $name ]; then
            echo "invalid input name, name must be non empty"
            exit -1
        fi
        cp -r $dummy_file ./components/${name}.h
        sed -i "s/dummy/$name/g" ./components/$name.h
        sed -i "s/DUMMY/${upp_case}/g" ./components/$name.h

        sed -i "/#endif \/\/ !__RACEUP_BOARD_COMPONENT__/i\#include \"${name}.h\""\
        "$component_file_h"
        ;;
    "del")
        if [ -z $name ]; then
            echo "invalid input name, name must be non empty"
            exit -1
        fi
        rm -rf ./components/$name.h
        sed -i "/#include \"${name}.h\"/d" "$component_file_h"
        ;;
    "delall")
        cmps=$(./component_manager.sh lst)
        for item in $cmps; do
            ./component_manager.sh del $item
        done
        ;;
    "lst") 
        /bin/ls ./components | grep -v "common_idx" | cut -d '.' -f1
        ;;
    *)
        help;
        exit 0;
        ;;
esac
