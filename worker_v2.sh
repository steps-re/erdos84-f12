#!/bin/bash
# Erdős #84 f(12) worker. Owns slices s with s % W == IDX of M; runs prog A (their DFS, all graphs) and
# prog B (our subset DP, min degree >= 2) per slice; skips slices whose output already exists (resume after preemption).
set -u
B=gs://steps-oss-math-61ad1e/p84v2; M=16000
IDX=$(curl -s -H "Metadata-Flavor: Google" http://metadata.google.internal/computeMetadata/v1/instance/attributes/idx)
W=$(curl -s -H "Metadata-Flavor: Google" http://metadata.google.internal/computeMetadata/v1/instance/attributes/w)
apt-get update -qq && apt-get install -y -qq nauty g++ >/dev/null
mkdir -p /work && cd /work && gsutil -q cp "$B/src/*" . && chmod +x geng_slice.sh
c++ -std=c++20 -O3 -march=native -o enum_slice enum_slice.cpp && cc -O3 -march=native -o cycdp cycdp.c || exit 1
gsutil -q ls "$B/out/" > done.txt 2>/dev/null || true
task() { prog=$1; s=$2; f=${prog}_$s
  grep -q "/$f\.txt$" done.txt && return
  if [ $prog = A ]; then
    SLICE=$s/$M ./enum_slice --min-n 12 --max-n 12 --geng ./geng_slice.sh --output $f.json > $f.log 2>&1 && \
      python3 -c "import json;r=json.load(open('$f.json'))['values'][0];print('graphs',r['unlabeled_graph_count']);[print(sum(1<<(L-3) for L in c['lengths']),c['witness_graph6']) for c in r['cycle_sets']]" > $f.txt 2>> $f.log || { echo FAIL >> $f.log; gsutil -q cp $f.log $B/fail/; return; }
  else
    nauty-geng -d2 12 $s/$M 2> $f.geng | ./cycdp > $f.txt && grep -q "$(head -1 $f.txt | cut -d' ' -f2) graphs generated" $f.geng || { echo FAIL >> $f.geng; gsutil -q cp $f.geng $B/fail/; return; }
    cat $f.geng >> $f.txt
  fi
  gsutil -q cp $f.txt $B/out/ && rm -f $f.*
}
export -f task; export B M
for s in $(seq $IDX $W $((M-1))); do echo "A $s"; echo "B $s"; done | shuf | xargs -P $(nproc) -n 2 bash -c 'task "$0" "$1"'
echo done | gsutil -q cp - $B/vmdone/$IDX
shutdown -h now
