#!/bin/bash

#*
#* Author: ENEA TSANAI
#* e-mail: tsanai@ceid.upatras.gr, tsanaienea@gmail.com
#*


scenToRun="Manhattan"

for i in `seq 1 5`;
do
        cd /home/ru6/ns3-gpsr-m_v1_\(ns3.20\)/ns-3.20/
		./run.sh --duration=40 --AnimTest=false --Anim=false --reset=true --RP=5 --scenToRun=$scenToRun"/"$i #GPSR_V1


		cd /home/ru6/ns3-gpcr/
		./run.sh --duration=40 --AnimTest=false --Anim=false --reset=false --RP=7 --scenToRun=$scenToRun"/"$i #GPSR	
		./run.sh --duration=40 --AnimTest=false --Anim=false --reset=false --RP=6 --scenToRun=$scenToRun"/"$i #GPCR

		cp /home/ru6/ns3-gpsr-m_v1_\(ns3.20\)/ns-3.20/scripts/mobility/BonMotion/scenario.ns_movements /home/ru6/ns3-gpsr-m_v1_\(ns3.20\)/ns-3.20/logs/$scenToRun"/"$i
done   




