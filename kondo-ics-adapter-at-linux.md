
# using ics adapter at ubuntu linux

> lsusb
Bus 003 Device 007: ID 165c:0006 Kondo Kagaku
> sudo modprobe ftdi_sio vendor=0x165C product=0007
> sudo mknod /dev/ttyUSB0 c 188 0
キャラクタ型（バッファなし）
デバイス番号（メジャー：マイナー = 188:0）
> lsusb
Bus 003 Device 007: ID 165c:0006 Kondo Kagaku
> lsmod | grep ftdi
ftdi_sio               48930  0 
usbserial              45014  2 pl2303,ftdi_sio
> ls /dev/ttyUSB*
/dev/ttyUSB0 

> dmesg | tail -n 4
[46995.384402] ftdi_sio: unknown parameter 'vendor' ignored
[46995.384406] ftdi_sio: unknown parameter 'product' ignored
[46995.384580] usbcore: registered new interface driver ftdi_sio
[46995.384588] usbserial: USB Serial support registered for FTDI USB Serial Device

vendor, productの指定がむしされている様です。
これは、linux kernel の 3.12 以降では受付ないようになっているためです。
もともとはデバッグ用に用意されていた口だったのが消された様です.([1])
なので別の方法をとります.
echo <vid> <pid> > /sys/bus/usb-serial/drivers/ftdi_sio/new_id
[VCP driver for force sensor ] 0483:5740

> rmmod ftdi_sio
> modprobe ftdi_sio
> dmesg | tail -n 4
[47522.718939] usb 3-2: Manufacturer: Kondo Kagaku
[47522.718941] usb 3-2: SerialNumber: KOS426SF
[47527.190311] usbcore: registered new interface driver ftdi_sio
[47527.190322] usbserial: USB Serial support registered for FTDI USB Serial Device
> echo 0x165C 0007 | sudo tee /sys/bus/usb-serial/drivers/ftdi_sio/new_id

どうも反応がないですね。ここで気づきましたが、近藤科学のサイト([2])だとプロダクトIDを0007でしていますが、
ICS USB Adapter のIDは0006なので指定し直します.

> sudo modprobe ftdi_sio
> echo 0x165C 0006 | sudo tee /sys/bus/usb-serial/drivers/ftdi_sio/new_id
> dmesg | tail -n 10
[47663.744654] usb 3-2: Product: ICS USB ADAPTER HS
[47663.744657] usb 3-2: Manufacturer: Kondo Kagaku
[47663.744659] usb 3-2: SerialNumber: KOS426SF
[47689.712993] ftdi_sio 3-2:1.0: FTDI USB Serial Device converter detected
[47689.713037] usb 3-2: Detected FT232RL
[47689.713040] usb 3-2: Number of endpoints 2
[47689.713042] usb 3-2: Endpoint 1 MaxPacketSize 64
[47689.713045] usb 3-2: Endpoint 2 MaxPacketSize 64
[47689.713047] usb 3-2: Setting MaxPacketSize 64
[47689.713289] usb 3-2: FTDI USB Serial Device converter now attached to ttyUSB0

> ls /dev/ttyUSB*
/dev/ttyUSB0

こんどはちゃんと認識しました。

>


[1] : https://github.com/torvalds/linux/commit/e17c1aa2e1
[2] : http://kondo-robot.com/faq/usb-adapter-for-linux
