#!/usr/bin/env python
# -*- coding: utf-8 -*-

# システムライブラリをインポート
import sys

# 通信ライブラリ(rsxpy)をインポート
import rsxpy

# コマンドライン引数を取得
argv = sys.argv

# クラスをインスタンス化
rsx = rsxpy.rsxpy()

# シリアルデバイスを開く
# この場合は /dev/ttyUSB0 を開いている.
rsx.open('ttyUSB', '0', rsxpy.hr_baudrate.HR_B460800, rsxpy.hr_parity.HR_PAR_NONE)

servos = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20]
servo_off   = [0]
servo_on    = [1]
servo_break = [2]

if (len(argv) < 2) :
    print "{0} requires 1 argument".format(argv[0])
    quit()

if (argv[1] == 'off') :
    rsx.lpkt_mem_write_all(servos, rsxpy.RSX_RS30X_MEM_ADDR.RSX_RAM_TRQ_ENABLE, servo_off)
elif (argv[1] == 'on') :
    rsx.lpkt_mem_write_all(servos, rsxpy.RSX_RS30X_MEM_ADDR.RSX_RAM_TRQ_ENABLE, servo_on)
elif (argv[1] == 'break') :
    rsx.lpkt_mem_write_all(servos, rsxpy.RSX_RS30X_MEM_ADDR.RSX_RAM_TRQ_ENABLE, servo_on)
else :
    print "{0} invalid argument : {1}".format(argv[0], argv[1])


