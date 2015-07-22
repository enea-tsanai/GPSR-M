#!/bin/bash

# Script that generates the topology with the defined Model

# == ManhattanGrid =====================================================
# Version:      v1.0
# Description:  Application to construct ManhattanGrid mobility scenarios
# Affiliation:  University of Bonn - Institute of Computer Science 4 (http://net.cs.uni-bonn.de/)
# Authors:      University of Bonn
# Contacts:     bonnmotion@lists.iai.uni-bonn.de

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
# ManhattanGrid:
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

rm -r scenario.*
bm="/home/fotis/Documents/RU6/NETWORK-SIMULATOR/bonnmotion-2.1a/bin/bm"
$bm -f scenario ManhattanGrid -n 125 -d 120 -i 3000 -x 1000 -y 1000 -e 1 -m 22.2 -t 0.3 -u 4 -v 4
$bm NSFile -f scenario
