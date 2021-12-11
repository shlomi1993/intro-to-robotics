!/bin/bash 

echo "Compiling…" ; echo
mkdir -p build && cd build
cmake ..
make
cd ..

echo "Run simulation…" ; echo
unset ARGOS_PLUGIN_PATH

if [ "$1" == "1" ]
then
	argos3 -c ./config/ex1.argos
elif [ "$1" == "2" ]
then
	argos3 -c ./config/ex2.argos
elif [ "$1" == "3" ]
then
	argos3 -c ./config/PRM.argos
elif [ "$1" == "all" ]
then
	argos3 -c ./config/PRM.argos
	argos3 -c ./config/mixed_controllers.argos
	argos3 -c ./config/porting_example.argos
	argos3 -c ./config/program_template.argos
	argos3 -c ./config/run_away_by_bumpers.argos
	argos3 -c ./config/run_away_by_bumpers_multiple.argos
	argos3 -c ./config/sand_timer_test.argos
	argos3 -c ./config/simple_demo.argos
	argos3 -c ./config/ex1.argos
	argos3 -c ./config/ex2.argos
	argos3 -c ./config/ex3.argos
fi

echo "Done." ; echo
