"""Check the f(12) outputs: graph totals, A == B + n=11 sets, and every witness re-derived with networkx.
usage: python verify.py <dir with A_*.txt B_*.txt> <results_nauty_n11.json from manman4's repo>"""
import glob, json, sys
import networkx as nx

out, n11 = sys.argv[1], sys.argv[2]

def read(prefix):
    sets, graphs = {}, 0
    for f in glob.glob(f"{out}/{prefix}_*.txt"):
        lines = open(f).read().split("\n")
        graphs += int(lines[0].split()[1])
        for l in lines[1:]:
            if l and not l.startswith(">"):
                m, w = l.split()
                sets.setdefault(int(m), w)
    return sets, graphs

def mask(g6):
    G = nx.from_graph6_bytes(g6.encode())
    lengths = {len(c) for c in nx.simple_cycles(G, length_bound=12) if len(c) >= 3}
    return sum(1 << (L - 3) for L in lengths), G.number_of_nodes()

A, a_graphs = read("A")
B, b_graphs = read("B")
small = {sum(1 << (L - 3) for L in c["lengths"]) for c in json.load(open(n11))["values"][0]["cycle_sets"]}
print("A graphs", a_graphs, "A000088(12) match:", a_graphs == 165091172592)
print("B graphs (min degree >= 2)", b_graphs)
print("A sets", len(A), "B sets", len(B), "n=11 sets", len(small))
print("A == B | n11:", set(A) == set(B) | small)
bad = [m for m, w in A.items() if mask(w) != (m, 12)]
print("witnesses ok:", len(A) - len(bad), "/", len(A))
print("f(12) =", len(A))
