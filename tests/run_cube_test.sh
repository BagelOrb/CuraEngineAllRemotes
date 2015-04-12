rm build/main.o 
make
./build/CuraEngine -s infillPattern=2 -v -o tests/gcodes/sphere.gcode tests/stls/unit_sphere.stl