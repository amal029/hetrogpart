mkdir -p graphs, graphs/$1;
echo " " > graphs/$1/$1.surface.dat

let mincost=99999999
let flag="true"
let previoustile=0

for i in /home/aravind/Work/Heterogeneous-Bin-Packing/Binpacking-verysmall-outputs/*$1* ; do
#	for counter in $(seq 2 2 20)
#    do
#	echo $i
	wholename=$(echo "$i" | awk -F / '{print $7}')
	tile=$(echo "$wholename" | awk -F . '{print $4}')
	counter1=$(echo "$wholename" | awk -F . '{print $6}')
	counter2=$(echo "$wholename" | awk -F . '{print $7}')
#	echo $wholename
	let totalnodes=$counter1*$counter2
	finalstring=$(grep 'FinalCost' $i)
	finalcost=$(echo "$finalstring" | awk -F : '{print $2}')
	if [ "$flag" == "true" ]; then
		$flag="false"
		let previoustile=$tile
	fi
	if [ "$tile" -ne "$previoustile" ]; then
		echo " " >> graphs/$1.surface.dat
	fi
	if [ -z "$finalcost" ]; then
		echo $wholename "--" $tile "--" $totalnodes "--" $finalcost
	else
		echo $wholename "--" $tile "--" $totalnodes "--" $finalcost
		echo $tile $totalnodes $finalcost >> graphs/$1/$1.surface.dat	
	fi
	let previoustile=$tile
#    done
done

echo "Generating gnuplot file..."
echo "reset" > graphs/$1/$1.surface.gnuplot
echo "set term png enhanced" >> graphs/$1/$1.surface.gnuplot
echo "set term png enhanced" >> graphs/$1/$1.surface.gnuplot
#echo "set term post eps enhanced" >> graphs/$1/$1.surface.gnuplot
#echo "set pm3d" >> graphs/$1.surface.gnuplot
echo "set dgrid3d 200,200" >> graphs/$1/$1.surface.gnuplot
echo "set palette gray" >> graphs/$1/$1.surface.gnuplot
echo "set output \"graphs/"$1"/"$1".surface.png\"" >> graphs/$1/$1.surface.gnuplot
echo "set xlabel \"Tile size\" rotate parallel" >> graphs/$1/$1.surface.gnuplot
echo "set ylabel \"Number of nodes in the topology\" rotate parallel" >> graphs/$1/$1.surface.gnuplot
echo "set zlabel \"Execution time in seconds(s)\" rotate parallel" >> graphs/$1/$1.surface.gnuplot
echo "set log z" >> graphs/$1/$1.surface.gnuplot
#echo "splot \"graphs/"$1"/"$1".surface.dat\" with dots" >> graphs/$1/$1.surface.gnuplot
echo "splot \"/home/avinash/graphs/"$1".surface.dat\" with points, \"/home/aravind/Work/Heterogeneous-Bin-Packing/graphs/"$1"/"$1".surface.dat\" with points" >> graphs/$1/$1.surface.gnuplot

echo "Plotting..."
gnuplot graphs/$1/$1.surface.gnuplot
echo "Done!"
