import os
import sys


edge_seq_filename=sys.argv[1]


nodes = 0
edge_cap = 0
edge_seq = []
with open(edge_seq_filename, "r") as edge_seq_file:
    
    # first line
    meta = edge_seq_file.readline()[2:].split(" ")

    nodes = int(meta[0])
    edge_cap = int(meta[1])

    while True:
        line = edge_seq_file.readline()

        if not line:
            break


        if line == "":
            continue

        op_edge_seq = line.split(" ")

        op_edge_seq[2] = op_edge_seq[2].split("\n")[0]
        #print(op_edge_seq)

        # 1 in edge_seq = insertion, 0 in edge_seq = deletion
        # 1 in Dynamic-MIS = deletion, 0 in Dynamic-MIS = insertion
        op = ("1" if op_edge_seq[0] == "0" else "0", op_edge_seq[1], op_edge_seq[2])

        edge_seq.append(op)  


# write instance
# all nodes exists and are in MIS

basename=os.path.basename(edge_seq_filename).split(".")[0]
print("writing graph")
with open(os.path.join("Graph", basename + ".graph"), "w") as graph_file:
    graph_file.writelines([str(nodes) + " 0\n"])

print("writing mis")
node_seq = []
for node in range(nodes):
    node_seq.append(str(node)+ "\n")

with open(os.path.join("MIS", basename + ".mis"), "w") as mis_file:
    mis_file.writelines(node_seq)
    
print("writing inst")
inst_seq = []
for [o, s, t] in edge_seq:
    inst_seq.append(str(o) + " " + str(s) + " " + str(t) + "\n")

with open(os.path.join("Inst", basename + ".inst"), "w") as inst_file:
   inst_file.writelines(inst_seq)








