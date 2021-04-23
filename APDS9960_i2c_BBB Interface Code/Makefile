# Makefile for kernal module user level interface ktimer

APDS9960_i2c_BBB : APDS9960_i2c_BBB.o
	arm-linux-gnueabihf-gcc -o APDS9960_i2c_BBB APDS9960_i2c_BBB.o -static

APDS9960_i2c_BBB.o : APDS9960_i2c_BBB.c
	arm-linux-gnueabihf-gcc -c APDS9960_i2c_BBB.c

clean :
	rm APDS9960_i2c_BBB APDS9960_i2c_BBB.o
