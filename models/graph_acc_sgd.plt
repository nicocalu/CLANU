set key autotitle columnhead
set xrange [0:250]
set yrange [20:100]
plot "/home/grenier/Downloads/Clanu/cpp/models/test_sgd.dat" using 1:5 with lines, "/home/grenier/Downloads/Clanu/cpp/models/test_sgd.dat" using 1:6 with lines  
pause -1

