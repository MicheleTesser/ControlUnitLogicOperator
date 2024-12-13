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

echo "creating can1.h/c"
./dbc/dbcc/dbcc ./dbc/can1.dbc
echo "creating can2.h/c"
./dbc/dbcc/dbcc ./dbc/can2.dbc
