 set terminal png size 600, 600
 set output 'map.png'
 set grid

 limit = 0.5

 plot 'log1' using 1:2 with points notitle, 'log2' using 1:2 with points notitle
