echo "Compiling changes..."
make

FILES="tests/stls/calibration/testcube_10mm.stl"
for fullfile in $FILES
do
	filename=$(basename "$fullfile")
	extension="${filename##*.}"
	filename="${filename%.*}"
	echo "Producing $filename gcodes; infillPattern = $1"
	./build/CuraEngine -c build/flex2.cfg -s infillPattern=$1 -s posx=5000 -s posy=5000 -o tests/gcodes/structural_test/$filename.gcode $fullfile 
done

open tests/gcodes/structural_test