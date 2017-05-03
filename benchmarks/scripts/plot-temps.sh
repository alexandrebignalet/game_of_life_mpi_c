#!

taille=$1 nbsteps=$2 prob=$3

gnuplot -persist <<EOF
set terminal png
set output './benchmarks/plots/graphe-temps-$taille.png'
set style line 1 lc rgb "orange" lt 1 lw 2 pt 7
set style line 2 lc rgb "black" lt 1 lw 2 pt 7
set style line 3 lc rgb "red" lw 2 pt 7
set style line 4 lc rgb "blue" lw 2 pt 7
set logscale x
set xlabel "Nombre de procs"
set ylabel "Temps d'exécution"
set title "Temps d'exécution en fonction du nombre de procs pour générer une image $taille"
set xtics (1, 2, 4, 8, 16, 32, 64, 128)
plot [0.9:150][0:$prob] \
	 "./benchmarks/data-files/temps-$taille-$nbsteps-$prob.txt" using 1:(\$1) title "seq" with linespoints ls 1,\
	 "./benchmarks/data-files/temps-$taille-$nbsteps-$prob.txt" using 1:(\$2) title "line block" with linespoints ls 2
EOF