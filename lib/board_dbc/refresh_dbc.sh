#!/bin/sh

if [ ! -e ./dbc/can2.dbc -o ! -e ./dbc/can1.dbc ];
then
    echo "usage $0"
    echo "run the scrip inside of ControlUnitLogicOperator/lib/board_dbc"
    exit -1
fi
rm -rf ./dbc/
git clone --recursive https://github.com/raceup-electric/dbc.git

cd ./dbc/dbcc/ 

make

cd ../..

rm -f ./can*

create_lib_can_dbc() {
    echo "creating can${1}.h/c"
    ./dbc/dbcc/dbcc ./dbc/can${1}.dbc
    sed -i "s/pack_message/pack_message_can${1}/" ./can${1}.h
    sed -i "s/pack_message/pack_message_can${1}/" ./can${1}.c

    sed -i "s/print_message/print_message_can${1}/" ./can${1}.h
    sed -i "s/print_message/print_message_can${1}/" ./can${1}.c
}

create_lib_can_dbc 1
create_lib_can_dbc 2
