!/bin/bash 

echo "Compiling…" ; echo
mkdir -p build && cd build
cmake ..
make
cd ..

echo "Run simulation…" ; echo
unset ARGOS_PLUGIN_PATH

if [ "$1" == "warmup" ]
then
	argos3 -c ./config/ex1.argos
elif [ "$1" == "wanderbot" ]
then
	argos3 -c ./config/ex2.argos
elif [ "$1" == "navigation" ]
then
	argos3 -c ./config/PRM.argos
elif [ "$1" == "foraging" ]
then
	if [ "$2" == "r" ]
	then
		argos3 -c ./config/tournament_redteam.argos
	else 
		argos3 -c ./config/tournament.argos
	fi
elif [ "$1" == "mixed_controllers" ]
then
	argos3 -c ./config/mixed_controllers.argos
fi
elif [ "$1" == "porting_example" ]
then
	argos3 -c ./config/porting_example.argos
fi
elif [ "$1" == "run_away_by_bumpers" ]
then
	argos3 -c ./config/run_away_by_bumpers.argos
fi
elif [ "$1" == "run_away_by_bumpers_multiple" ]
then
	argos3 -c ./config/run_away_by_bumpers_multiple.argos
fi
elif [ "$1" == "sand_timer_test" ]
then
	argos3 -c ./config/sand_timer_test.argos
fi
elif [ "$1" == "simple_demo" ]
then
	argos3 -c ./config/simple_demo.argos
fi

echo "Done." ; echo
