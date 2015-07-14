#!/bin/bash

#*
#* Author: ENEA TSANAI
#* e-mail: tsanai@ceid.upatras.gr, tsanaienea@gmail.com
#*


mkdir -p logs logs/falhro mobility

# RP=5;
# nodes=100;
# speed=20;

RP=$1;
mobFile="mobility/falhro_ns2mobility.tcl";

echo "Routing Protocol: " $RP

./waf --run "gcvanet_falhro --RP=$RP --mobFile=$mobFile --application=4 --tracing=true --energyTracing=false" > logs/"out_"$RP.txt 2>&1
# ./waf --run "gcvanet --RP=1 --application=4" > logs/out.txt 2>&1


grep -i "TraceDelay TX" logs/"out_"$RP.txt > logs/"Tx_"$RP.txt
grep -i "TraceDelay: RX" logs/"out_"$RP.txt > logs/"Rx"_$RP.txt
# grep -iw "forwarding to" logs/out.txt > logs/hops.txt

# Average delay in ms
cat logs/"Rx"_$RP.txt | tr -d '+','ns' | gawk '{ sum += $17; i +=1 }; END { print (sum / i) / 10^6 }' > logs/falhro/"E2E_rp_"$RP".txt"
cat logs/"Rx"_$RP.txt | tr -d '+','ns' | gawk '{ sum += $17; i +=1 }; END { print (sum / i) / 10^6 }'

# Average energy in jouls
# gawk '{ sum += $2; i +=1 }; END { print (sum / i) }' logs/"rp_"$RP"_energy_logs".txt > logs/falhro/"Energy_rp_"$RP".txt"
# gawk '{ sum += $2; i +=1 }; END { print (sum / i) }' logs/"rp_"$RP"_energy_logs".txt 

PS=$(wc -l < logs/"Tx"_$RP.txt);
PR=$(wc -l < logs/"Rx"_$RP.txt);

echo "Packets Sent: " ${PS}
echo "Packets Received: " ${PR}
echo ${PR} > logs/falhro/"PDR_rp_"$RP".txt"

# numofnodes vs avg speed vs delay
# numofnodes vs avg speed vs PDR