echo "Compiling changes..."
make

FILES="tests/stls/calibration/*"
for fullfile in $FILES
do
	filename=$(basename "$fullfile")
	extension="${filename##*.}"
	filename="${filename%.*}"
	echo "Producing $filename gcodes; infillPattern = $1"
	./build/CuraEngine -c build/flex2.cfg -s infillPattern=$1 -o tests/gcodes/structural_test/$filename.gcode $fullfile 
done