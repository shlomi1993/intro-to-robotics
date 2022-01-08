!/bin/bash 

echo "Compiling…" ; echo
mkdir -p build && cd build
cmake ..
make
cd ..


for i in {1..15}
do
	cd ./config
	prev=$(expr $i - 1)
	sed -i "s/random_seed=\"$prev\"/random_seed=\"$i\"/" tournament_redteam.argos
	cd ..
	echo "Run experiment $i"
	unset ARGOS_PLUGIN_PATH
	argos3 -c ./config/tournament_redteam.argos > /dev/null 2>&1
	echo "Experiment $i completed!"
done

cd ./config
sed -i "s/random_seed=\"15\"/random_seed=\"0\"/" tournament_redteam.argos
cd ..

echo "Done." ; echo
