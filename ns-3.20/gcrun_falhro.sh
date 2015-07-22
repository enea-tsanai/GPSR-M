#!/bin/bash

#*
#* Author: ENEA TSANAI
#* e-mail: tsanai@ceid.upatras.gr, tsanaienea@gmail.com
#*


mkdir -p logs logs/falhro_area

# RP=5;
# nodes=100;
# speed=20;

RP=$1;
mobFile="scripts/mobility/falhro_area/falhro_ns2mobility.tcl";

echo "Routing Protocol: " $RP

./waf --run "gcvanet_falhro --RP=$RP --mobFile=$mobFile --application=4 --tracing=true --energyTracing=false" > logs/falhro_area/"out_"$RP.txt 2>&1
# ./waf --run "gcvanet --RP=1 --application=4" > logs/falhro_area/out.txt 2>&1


grep -i "TraceDelay TX" logs/falhro_area/"out_"$RP.txt > logs/falhro_area/"Tx_"$RP.txt
grep -i "TraceDelay: RX" logs/falhro_area/"out_"$RP.txt > logs/falhro_area/"Rx"_$RP.txt
# grep -iw "forwarding to" logs/falhro_area/out.txt > logs/falhro_area/hops.txt

# Average delay in ms
cat logs/falhro_area/"Rx"_$RP.txt | tr -d '+','ns' | gawk '{ sum += $17; i +=1 }; END { print (sum / i) / 10^6 }' > logs/falhro_area/"E2E_rp_"$RP".txt"
echo "E2E: "
cat logs/falhro_area/"Rx"_$RP.txt | tr -d '+','ns' | gawk '{ sum += $17; i +=1 }; END { print (sum / i) / 10^6 }'

# Average energy in jouls
# gawk '{ sum += $2; i +=1 }; END { print (sum / i) }' logs/falhro_area/"rp_"$RP"_energy_logs".txt > logs/falhro_area/falhro/"Energy_rp_"$RP".txt"
# gawk '{ sum += $2; i +=1 }; END { print (sum / i) }' logs/falhro_area/"rp_"$RP"_energy_logs".txt 

PS=$(wc -l < logs/falhro_area/"Tx"_$RP.txt);
PR=$(wc -l < logs/falhro_area/"Rx"_$RP.txt);

echo "Packets Sent: " ${PS}
echo "Packets Received: " ${PR}
echo ${PR} > logs/falhro_area/"PDR_rp_"$RP".txt"

# numofnodes vs avg speed vs delay
# numofnodes vs avg speed vs PDR