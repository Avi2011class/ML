set terminal png size 1200, 800
set output 'stat.png'

plot 'stat.txt' using 1:2 with lines lw 2
