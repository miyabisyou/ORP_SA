host = "128"
radix = "24"
seed = "0"
type = "reduce"

fnameone = "./../DoNS/(".host.", ".radix.")/sa_".type."_host".host."radix".radix."seed".seed."offset0"
titlename = "AdaptiveSA(".host.", ".radix.") ".type
fnametwo = "sa"
 
set title titlename
set xlabel "Number of calculations"  
set ylabel "Diameter"
set y2label "ASPL"
set y2tics
set xrange [0:100000]
set yrange [-2:10]
set y2range [-0.5:0.5]
set format x "%3.1tx10^{%L}"
set title font"Arial,15"
set xlabel font "Arial,15"
set ylabel font "Arial,15"
set y2label font "Arial,15"
set tics font "Arial,11"
set key font"Arial,11"
plot fnameone.".txt" using 1:2 axis x1y1 title "diameta" w l, fnameone.".txt" using 1:3 axis x1y2 title "ASPL" w l
set terminal png
set out "./photo/sa_".type."_host".host."radix".radix."seed".seed."offset0.png"
replot