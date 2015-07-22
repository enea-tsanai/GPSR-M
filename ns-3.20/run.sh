#!/bin/bash

#*
#* Author: ENEA TSANAI
#* e-mail: tsanai@ceid.upatras.gr, tsanaienea@gmail.com
#*


# Functions

function show_help {
	echo "help!"
}

# Print results to file
function write_results {
	grep -i "TraceDelay TX" logs/"out_"$RP.txt > logs/"Tx_"$RP.txt
	grep -i "TraceDelay: RX" logs/"out_"$RP.txt > logs/"Rx"_$RP.txt
	# grep -iw "forwarding to" logs/out.txt > logs/hops.txt

	echo "------- Results -------"

	# Average delay in ms
	echo -e "*Average Delay*"
	cat logs/"Rx"_$RP.txt | tr -d '+','ns' | gawk '{ sum += $17; i +=1 }; END { print (sum / i) / 10^6 }' > logs/$scen/"E2E_rp_"$RP".txt"	
	cat logs/"Rx"_$RP.txt | tr -d '+','ns' | gawk '{ sum += $17; i +=1 }; END { print (sum / i) / 10^6 }'
	echo -e "-------\n"
	
	# Average energy in jouls
	echo -e "*Average Energy*"
	gawk '{ sum += $2; i +=1 }; END { print (sum / i) }' logs/"EN_rp_"$RP".txt" > logs/$scen/"ENERGY_rp_"$RP".txt"
	gawk '{ sum += $2; i +=1 }; END { print (sum / i) }' logs/"EN_rp_"$RP".txt"
	echo -e "-------\n"

	PS=$(wc -l < logs/"Tx"_$RP.txt);
	PR=$(wc -l < logs/"Rx"_$RP.txt);

	# echo "Packets Sent: " ${PS}
	# echo "Packets Received: " ${PR}
	# echo $(( ($PR / $PS) * 100)) > logs/$scen/"PDR_rp_"$RP".txt"
	

	PDR=$(bc <<< "scale=4;($PR/$PS)*100")
	echo -e "TX: $PS \nRX: $PR \nPDR: $PDR" 
	echo -e "TX: $PS \nRX: $PR \nPDR: $PDR"  > logs/$scen/"PDR_rp_"$RP".txt"

	echo -e "------- **END** ------- \n"
}

# Generates nodes' movement in $scen scenarios
function gen_mobility {
	# App:
	# 	-D print stack trace
	# Scenario:
	# 	-a <attractor parameters (if applicable for model)>
	# 	-c [use circular shape (if applicable for model)]
	# 	-d <scenario duration>
	# 	-i <number of seconds to skip>
	# 	-n <number of nodes>
	# 	-x <width of simulation area>
	# 	-y <height of simulation area>
	# 	-R <random seed>
	# $scen:
	# 	-c <speed change probability>
	# 	-e <min. speed>
	# 	-m <mean speed>
	# 	-o <max. pause>
	# 	-p <pause probability>
	# 	-q <update distance>
	# 	-s <speed standard deviation>
	# 	-t <turn probability>
	# 	-u <no. of blocks along x-axis>
	# 	-v <no. of blocks along y-axis>

	rm -rf scenario.*
	bm="/home/ru6/bonnmotion-2.1.3/bin/bm"
	$bm -f scenario ManhattanGrid -n $numNodes -d $duration -i 3000 -x $MapX -y $MapY -c 0.2 -e 1 -m 11.0 -o 5 -t 0.5 -p 0.5 -q 100 -u $BuildingsInX -v $BuildingsInY
	$bm NSFile -f scenario > /dev/null 2>&1
	echo "Test" $MovementsDIR
	mv scenario* $MovementsDIR
	echo "Regenerated Mobility"
}

# Generates Buildings in $scen scenarios
function gen_buildings {
	# Clear Output files
	echo "">$buildingsFile
	
	# x1=$(($RoadWidth / 2))
	x1=$((($RoadWidth / 2 + 10))) #Change the starting point because bonnmotion puts an initial offset to node mobility != 0
	y1=$x1
	BuildingWidth=$(($RoadLength - $RoadWidth))

	for ((  i=0;  i<$BuildingsInX;  i++  ))
	do
		for ((  j=0;  j<$BuildingsInY;  j++  ))
		do	
			# - - - 
			Ax1_=$(($x1 + $j * $BuildingWidth + $j * RoadWidth))
			Ay1_=$(($y1 + $i * $BuildingWidth + $i * RoadWidth))
			Ax2_=$(($Ax1_ + $BuildingWidth)) 
			Ay2_=$Ay1_
			echo -e $Ax1_ $Ay1_ $Ax2_ $Ay2_>> $buildingsFile

			# _ _ _
			Bx1_=$Ax2_
			By1_=$Ay2_
			Bx2_=$Ax2_
			By2_=$(($Ay2_ + $BuildingWidth))
			echo -e $Bx1_ $By1_ $Bx2_ $By2_>> $buildingsFile

			# [ [ [ 
			Cx1_=$Ax1_  
			Cy1_=$(($Ay1_ + $BuildingWidth))
			Cx2_=$Ax2_ 
			Cy2_=$Cy1_
			echo -e $Cx1_ $Cy1_ $Cx2_ $Cy2_>> $buildingsFile

			# ] ] ] 
			Dx1_=$Ax1_
			Dy1_=$Ay1_
			Dx2_=$Cx1_
			Dy2_=$Cy1_
			echo -e $Dx1_ $Dy1_ $Dx2_ $Dy2_>> $buildingsFile
		done
	done
	echo "Regenerated Buildings"
}

function set_DIRS {
	# Working DIR
	DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
	# Output
	mkdir -p logs logs/$scen
	buildingsFile="scripts/topology/buildings.txt"
	MovementsDIR=$DIR/scripts/mobility/BonMotion
	traceFile=$MovementsDIR/scenario.ns_movements
	# traceFile=$DIR/VanetsJournal/tests/test2.ns_movements
	# traceFile="/home/fotis/Documents/RU6/NETWORK-SIMULATOR/NS3/ns-allinone-3.21/ns-3.21/VanetsJournal/tests/test2.ns_movements"
}

#_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_


# Clear files before writing results to them
set_DIRS


# General Params
$scen="ManhattanGrid";
script="vanet"
duration=40.0
numNodes=50
Anim=false
AnimTest=false	 	#Run the script just for animation
Reset=false			#Reset Mobility and Buildings
logging=false		#Log messages on screen
tracing=false		#Log messages on files


#_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_

# Network Simulation Paramaters
packetSize=256
numPackets=100
interval=0.1
RP=5


#  Get Args
while [ "$#" -gt 0 ]; do
    case $1 in
       -h|-\?|--help)   # Call a "show_help" function to display a synopsis, then exit.
           show_help
           exit
           ;;
    # duration 
       -d|--duration)
           if [ "$#" -gt 1 ]; then
               duration=$2
               echo "duration = " $duration
               shift 2
               continue
           else
               echo 'ERROR: Must specify a non-empty "--duration DURATION" argument.' >&2
               exit 1
           fi
           ;;
       --duration=?*)
           duration=${1#*=}
           ;;
       --duration=)
           echo 'ERROR: Must specify a non-empty "--duration DURATION" argument.' >&2
           exit 1
           ;;
    # Anim
       -a|--Anim)       # Takes an option argument, ensuring it has been specified.
           if [ "$#" -gt 1 ]; then
               Anim=$2
               shift 2
               continue
           else
               echo 'ERROR: Must specify a non-empty "--Anim true|false" argument.' >&2
               exit 1
           fi
           ;;
       --Anim=?*)
           Anim=${1#*=} # Delete everything up to "=" and assign the remainder.
           ;;
       --Anim=)
           echo 'ERROR: Must specify a non-empty "--Anim true|false" argument.' >&2
           exit 1
           ;;
    # AnimTest
       -t|--AnimTest)       # Takes an option argument, ensuring it has been specified.
           if [ "$#" -gt 1 ]; then
               AnimTest=$2
               shift 2
               continue
           else
               echo 'ERROR: Must specify a non-empty "--AnimTest true|false" argument.' >&2
               exit 1
           fi
           ;;
       --AnimTest=?*)
           AnimTest=${1#*=} # Delete everything up to "=" and assign the remainder.
           ;;
       --AnimTest=)
           echo 'ERROR: Must specify a non-empty "--AnimTest true|false" argument.' >&2
           exit 1
           ;;
	# RP 
       -r|--RP)
           if [ "$#" -gt 1 ]; then
               RP=$2
               shift 2
               continue
           else
               echo 'ERROR: Must specify a non-empty "--RP rp" argument.' >&2
               exit 1
           fi
           ;;
       --RP=?*)
           RP=${1#*=}
           ;;
       --RP=)
           echo 'ERROR: Must specify a non-empty "--RP rp" argument.' >&2
           exit 1
           ;;

       --)              # End of all options.
           shift
           break
           ;;
       -?*)
           printf 'WARN: Unknown option (ignored): %s\n' "$1" >&2
           ;;
       *)               # Default case: If no more options then break out of the loop.
           break
   esac

   shift
done


# Logs for Network Params
echo -e "Routing:   \t" $RP
echo -e "Nodes:     \t" $numNodes
echo -e "packetSize:\t" $packetSize
echo -e "interval:  \t" $interval


# Map, Roads and buildings

# Roads
RoadsInX=5
RoadsInY=5
RoadLength=150
RoadWidth=20

# Map
MapX=$((($RoadsInY - 1) * $RoadLength))
MapY=$((($RoadsInX - 1) * $RoadLength))

echo "MapX: " $MapX




# Buildings
buildings=false
BuildingsInX=$(($RoadsInX-1))
BuildingsInY=$(($RoadsInY-1))

echo -e "Buildings:  \t" $buildings

# Custom Buildings Generator - Generate Buildings
# if [ $Reset = true ]; then
# 	#gen_buildings	
# fi

# BonnMotion - Generate Manhattan Grid Node Movements
if [ $Reset = true ]; then
	gen_mobility
	# ./waf --run "MobilityGen";
	echo ""
fi

# Logs for Map Params
echo -e "Map:       \t" $MapX"m x" $MapY"m"
if [ $buildings = true ]; then
	echo -e "Buildings: \t" $BuildingsInX "x" $BuildingsInY
fi




# Run Simulation
if [ $Anim = true ]; then
	./waf --run "$script
	--numNodes=$numNodes
	--buildings=$buildings 
	--traceFile=$traceFile 
	--buildingsFile=$buildingsFile 
	--RoadsInX=$RoadsInX 
	--RoadsInY=$RoadsInY 
	--RoadLength=$RoadLength 
	--numPackets=$numPackets 
	--packetSize=$packetSize 
	--interval=$interval 
	--duration=$duration
	--RP=$RP
	--logging=$logging
	--tracing=$tracing
	--AnimTest=$AnimTest" --vis #> logs/"out_"$RP.txt 2>&1
else
	./waf --run "$script
	--numNodes=$numNodes
	--buildings=$buildings 
	--traceFile=$traceFile 
	--buildingsFile=$buildingsFile 
	--RoadsInX=$RoadsInX 
	--RoadsInY=$RoadsInY 
	--RoadLength=$RoadLength 
	--numPackets=$numPackets 
	--packetSize=$packetSize 
	--interval=$interval 
	--duration=$duration
	--RP=$RP
	--logging=$logging
	--tracing=$tracing
	--AnimTest=$AnimTest" > logs/"out_"$RP.txt 2>&1
fi


if [ $AnimTest = false ]; then
	if [ $Anim = false ]; then
		echo "writing results..."
		write_results
	fi
fi

# nohup /home/ru6/ns-allinone-3.21_mod/ns-3.21/run.sh > /home/ru6/ns-allinone-3.21_mod/ns-3.21/scriptOutput.txt 2>&1

# ./waf --run "vanet --numNodes=100--buildings=true --traceFile=/home/fotis/Documents/RU6/NETWORK-SIMULATOR/NS3/ns-allinone-3.21/ns-3.21/VanetsJournal/BonMotion/scenario.ns_movements --buildingsFile=/home/fotis/Documents/RU6/NETWORK-SIMULATOR/NS3/ns-allinone-3.21/ns-3.21/scratch/buildings.txt --RoadsInX=5 --RoadsInY=5 --RoadLength=250 --numPackets=400 --packetSize=500 --interval=0.2 --duration=20 --RP=2 --logging=false --tracing=false" --command-template='gdb --args %s <args>'
# ./waf --run "gcvanet_falhro --RP=$RP --mobFile=$mobFile --application=4 --tracing=true --energyTracing=false" > logs/"out_"$RP.txt 2>&1



