# PIO interrupt
There is two ways to implement FPGA button PIO  interrupt.

The first one is using altera gpio driver and the user can wirte a high level driver to access the GPIO subsystem and register the ISR. 

The other way is writting a custom driver by accessing the registers and getting the irq number to register the ISR.

This example using the second one.

# Compile the Quartus project
Open the ghrd project and compile the project to generate the soc_system.sof

# Linux distribution
Git clone the SoCFPGA linux source and compile the kernel. Copy the zImage file  to the fat partion in your boot SD card.

# Device tree

## generate the dts file
The PIO button information must be added to the device tree. In the SoCEDS command shell, typing the following command and get the dtb file

*sopc2dts --input soc_system.sopcinfo
  --output socfpga.dts
  --type dts
  --board soc_system_board_info.xml
  --board hps_common_board_info.xml
  --bridge-removal all
  --clocks*

## change the dts 
Open the socfpga.dts and change the button_pio component compatible property:

compatible = "altr,pio-16.0", "altr,pio-1.0",**"terasic,pio-button";**

![mark](http://ogtvbbrfk.bkt.clouddn.com/blog/20170207/153604729.png)

*for the users who use the kernel version newer than 4.3, you should change this line as compatible = "terasic,pio-button";. otherwise the driver will not work*

## generate the dtb file

*dtc -I dts -o dtb -o socfpga.dtb socfpga.dts*

copy the socfpga.dtb file to the fat partion in your boot SD card.

# Compile the driver 
Edit the Makefile to change the correct kenel source directory before compiling the pio_interrupt driver. Copy the pio_interrupt.ko to the /home/root on your boot sd card.

# Compile the test application
Use the Make command in the SoCEDS command shell to compile the application. Copy the button_test file to the  /home/root  on your boot sd card.

# Demo setup
1. Boot you SD card and configure the FPGA with the soc_system.sof file.
2. login the linux system in the putty.
3. insmod pio_interrupt.ko
4. chmod +x button_test
5. ./button_test
6. Press a button and teminal will show which button is pressed.
![mark](http://ogtvbbrfk.bkt.clouddn.com/blog/20170207/154207705.png)
