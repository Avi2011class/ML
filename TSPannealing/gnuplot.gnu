set terminal png size 600, 500
set output 'way.png'
set grid
set xrange [0:200]
set yrange [0:200]

plot 'gnu.log' using 1:2 with linespoints lw 1 notitle
