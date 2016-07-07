This software is released under the MIT License, see LICENSE.txt.

# 1. build

```
> mkdir build
> cmake ..
> make
> ./sample/rsx_app
```

# 2. using sample application

```
# format
# ./build/sample/ics_mem_access  device(ttyUSB|*) device_id(0-*) baudrate(low|mid|high) <ICS30|ICS35> <write|read> servo_id(0-255) start_addr(0x**) size(0-58/62) 0x** ...

# changing baudrate  low --> high of servo (ID:0)
>./build/sample/ics_mem_access  ttyUSB 0 low ICS35 write 0 0x0d 1 0x00

# changing baudrate high -->  low of servo (ID:0)
>./build/sample/ics_mem_access  ttyUSB 0 high ICS35 write 0 0x0d 1 0x0a

# checking baudrate with low baudrate (115200[bps])
>./build/sample/ics_mem_access  ttyUSB 0 low ICS35 read 0 0x0d 1
...
 13
 --
 0a

# checking baudrate with high baudrate (1.25M[bps])
>./build/sample/ics_mem_access  ttyUSB 0 high ICS35 read 0 0x0d 1
...
 13
 --
 00

# change servo id to 13
# *** connect only one Servo to PC.
# *** It is ICS restriction.
./build/sample/ics_set_id ttyUSB 0 high 13

# actuate servo (ID:13) between -135[deg] and 135[deg]
./build/sample/ics_test ttyUSB 0 high 13 -135 135

```
