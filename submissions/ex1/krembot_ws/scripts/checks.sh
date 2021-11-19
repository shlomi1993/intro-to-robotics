!/bin/bash 

# Shlomi Ben-Shushan 311408264
# Khen Aharon 307947515

echo "Compiling…" ; echo
mkdir -p build && cd build
cmake ..
make
cd ..

echo "Run simulation…" ; echo
unset ARGOS_PLUGIN_PATH
argos3 -c ./config/mixed_controllers.argos
argos3 -c ./config/porting_example.argos
argos3 -c ./config/program_template.argos
argos3 -c ./config/run_away_by_bumpers.argos
argos3 -c ./config/run_away_by_bumpers_multiple.argos
argos3 -c ./config/sand_timer_test.argos
argos3 -c ./config/simple_demo.argos
argos3 -c ./config/ex1.argos

echo "Checks complete." ; echo
