# This script is used to edit specific nodes' movements in a generated topology (Bonnmotion).
# We set the positions of the senders and receivers in the current topology

# Some custom paths for the scenario directory
indxdir = '/home/fotis/Documents/RU6/SIMULATIONS/PAPERS/paper1/journal_simulations/scenarios/ManhattanGrid/'
scenario = str(indxdir) + 'scenario.ns_movements'
scenario_mod = str(indxdir) + 'scenario_mod.ns_movements'

# Receiver nodes
Rnodes = [
	['$node_(0)', 350.0, 150.0]
	]

# Sender nodes
Snodes = [
	['$node_(1)', 150.0, 250.0],
	['$node_(2)', 350.0, 480.0],
	['$node_(3)', 110.0, 120.0]
	]

def line_prepender(filename,line):
    with open(filename,'r+') as f:
        content = f.read()
        f.seek(0,0)
        f.write(line.rstrip('\r\n') + '\n' + content)

def scenario_editor(scenario, scenario_mod):
	# scenario: the old scenario filename
	# scenario_mod: the new scenario filename
	with open(str(scenario)) as oldfile, open(str(scenario_mod), 'w') as newfile:
	    for line in oldfile:
	        if (not any(rnode[0] in line for rnode in Rnodes)) and (not any(snode[0] in line for snode in Snodes)):
	            newfile.write(line)

# create the edited scenario with filename: scenario_mod
scenario_editor(scenario, scenario_mod)

Snodes.reverse()
for node in Snodes:
	line_prepender(str(scenario_mod), str(node[0]) + ' set Y_ ' + str(node[2]))
	line_prepender(str(scenario_mod), str(node[0]) + ' set X_ ' + str(node[1]))

Rnodes.reverse()
for node in Rnodes:
	line_prepender(str(scenario_mod), str(node[0]) + ' set Y_ ' + str(node[2]))
	line_prepender(str(scenario_mod), str(node[0]) + ' set X_ ' + str(node[1]))