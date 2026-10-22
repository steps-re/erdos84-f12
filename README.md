# Erdős #84: f(12) = 467 by exhaustive enumeration

Independent check of f(12) for [Erdős problem #84](https://www.erdosproblems.com/84)
([teorth/erdosproblems#290](https://github.com/teorth/erdosproblems/issues/290)).
f(n) = number of distinct cycle-length sets {k : G has a k-cycle} over simple graphs on n vertices.

All 165,091,172,592 unlabeled 12-vertex graphs (= A000088(12)) were generated with `nauty-geng 12 res/16000`
(Debian 12 nauty package) and processed by two programs:

- **A** `enum_slice.cpp`: manman4's `enumerate_nauty_fast.cpp`
  ([source](https://github.com/manman4/erdosproblems/tree/compute-problem-84/experiments/problem_84)), changed only
  to allow n = 12 and run one geng slice. Path DFS over all graphs. 467 sets.
- **B** `cycdp.c`: written separately, subset DP over vertex masks, run on `geng -d2` (min degree >= 2, 153,723,380,584 graphs).
  442 sets. Graphs with a vertex of degree <= 1 have the same cycle set as a graph on 11 vertices, so
  A should equal B plus the 247 n=11 sets. It does, exactly.

Every one of the 467 witnesses in `p84_f12_sets.txt` (mask, graph6, lengths) was re-derived with networkx 3.4.2
`simple_cycles`. `python verify.py <outputs> results_nauty_n11.json` reproduces all of this.

Run: 16,000 slices x 2 programs on 6 spot c3d-highcpu-60 VMs (`worker_v2.sh`), about 15.5 h, roughly $50.
Raw outputs (32,000 files) are in the release, with per-file hashes in `MANIFEST.sha256`.

- sha256 of all outputs concatenated in name order: `81ebbea3e2b8c3580617869862102c9cb1a3e8a21052c6129ee7e037dc5f67fe`
- sha256 of `p84_f12_sets.txt`: `8af16fe3855e32154cb8da82ad348077333bfb98cde8976d13a888568bf225bc`

AI-assisted: the code was written and run by an AI agent (Claude Code). The checks above are automated, not by hand.
