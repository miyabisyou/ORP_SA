host = "8"
radix = "3"
seed = "0"
off = "0"
switch_bias = "bias"
swing_bias = "random"

fnameone = "sa_host".host."radix".radix."seed".seed."offset".off."switch(".switch_bias.")swing(".swing_bias.")"
titlename = "AdaptiveSA(".host.", ".radix.") switch(".switch_bias.")swing(".swing_bias.")"
fnametwo = "sa"
 
set title titlename
set xlabel "Number of calculations"  
set ylabel "hASPL"
set y2label "switches"
set y2tics
set xrange [0:1000000]
set yrange [4:5.6]
set y2range [20:200]
set format x "%3.1tx10^{%L}"
set title font"Arial,15"
set xlabel font "Arial,15"
set ylabel font "Arial,15"
set y2label font "Arial,15"
set tics font "Arial,11"
set key font"Arial,11"
plot "./../graph/".fnameone.".txt" using 1:3 axis x1y1 title "ASPL" w l, "./../graph/".fnameone.".txt" using 1:4 axis x1y2 title "switches" w l
set terminal png
set out "./photo/".fnameone.".png"
replot