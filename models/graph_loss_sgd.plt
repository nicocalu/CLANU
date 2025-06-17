set key autotitle columnhead
set xrange [0:250]
set yrange [0:3]
plot "/home/grenier/Downloads/Clanu/cpp/models/test_sgd.dat" using 1:2 with lines, "/home/grenier/Downloads/Clanu/cpp/models/test_sgd.dat" using 1:3 with lines  
pause -1

