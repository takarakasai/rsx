g++ -fPIC -Wall -O2 -lboost_python -shared rsx_py.c  -I /usr/include/python2.7/ -I ../../h/rsx/ --std=gnu++11 -o rsx_py.so

