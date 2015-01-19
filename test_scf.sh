#!/bin/bash
make clean
make

./tcbvrp -m scf -f instances/tcbvrp_10_1_T240_m2.prob
./tcbvrp -m scf -f instances/tcbvrp_10_1_T480_m2.prob

./tcbvrp -m scf -f instances/tcbvrp_10_2_T240_m2.prob
./tcbvrp -m scf -f instances/tcbvrp_10_2_T480_m2.prob

./tcbvrp -m scf -f instances/tcbvrp_20_1_T240_m3.prob
./tcbvrp -m scf -f instances/tcbvrp_20_1_T480_m2.prob

./tcbvrp -m scf -f instances/tcbvrp_20_2_T240_m4.prob
./tcbvrp -m scf -f instances/tcbvrp_20_2_T480_m2.prob

./tcbvrp -m scf -f instances/tcbvrp_30_1_T240_m4.prob
./tcbvrp -m scf -f instances/tcbvrp_30_1_T480_m2.prob

./tcbvrp -m scf -f instances/tcbvrp_30_2_T240_m4.prob
./tcbvrp -m scf -f instances/tcbvrp_30_2_T480_m2.prob

./tcbvrp -m scf -f instances/tcbvrp_60_1_T360_m6.prob
./tcbvrp -m scf -f instances/tcbvrp_60_1_T480_m4.prob

./tcbvrp -m scf -f instances/tcbvrp_90_1_T480_m8.prob
./tcbvrp -m scf -f instances/tcbvrp_120_1_T480_m8.prob

./tcbvrp -m scf -f instances/tcbvrp_180_1_T720_m10.prob