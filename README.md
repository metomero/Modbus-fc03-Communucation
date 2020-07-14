# Modbus Communuction Device

This is a simple Modbus-RTU communication program for Function Code 3 working on PIC18f45K22 microcontroller.

### What is Modbus?
Modbus is a data communications protocol for use with its programmable logic controllers (PLCs). It was developed for industrial applications. Modbus supports communication to and from multiple devices connected to the same cable or Ethernet network. For example, a device that measures temperature and a different device to measure humidity, both of which communicates the measurements to a computer. Modbus is often used to connect a supervisory computer with a remote terminal unit (RTU) in supervisory control and data acquisition (SCADA) systems. 

### What is Function Code ?

A function code (FC) in Modbus is a specific code used in a Modbus request to tell the Modbus slave device what type of memory (i.e. holding registers, input coils, etc) to access and what action to perform on that memory (i.e. reading or writing). 

### FC3
Functon 3 Code means that the slave has read the holding registres and returned a response message containing those records.

For more information about Modbus and FC3 communucation protocols, visit http://www.simplymodbus.ca .

### PIC18F45K22

The 18F45K22 microprocessor is manufactured by the microchip company. It is an advanced microprocessor in terms of its features.

You can access the technical specifications and datasheet from; https://www.microchip.com/wwwproducts/en/PIC18F45K22 .

### Compiling Codes
Codes are written and compiled with MPLAB IDE. You can create new projects by using existing libraries and run the .hex file included in the project with Proteus.

# Features
- Device act as a Modbus Slave device with address=3 and only give proper response for FC3 
- Function code requests other than FC3 responded with exception code (01-illegal function).
- FC3 requests for invalid addresses responded with exception code (02-illegal data address) .
- Modbus table of the registers in the following order;

| Modbus Address | Data |
| ------ | ------ |
| 40001 | PORTB and PORTD  |
| 40002 | AN0 (10 bit)|
| 40003 | AN1 (10 bit)|
| 40004 | AN2 (10 bit)|
|

#   Testing

You can run the codes via the given proteus file (proteus_test.pdsprj). Use a virtual port emulator to creat virtual RS232 port and a virtual port terminal program to send a request to the microcontroller. 

* [Virtual Serial Ports] 
* [Realterm: Serial Terminal] 

When you create the virtual port from the emulator and send the request message through the terminal, the system parse the incoming message. It reads the values from analog pins according to requested adrdresses. Then creates the response message and send it.




License
----

MIT

[//]: # 

   [Virtual Serial Ports]:  <https://freevirtualserialports.com>
   [Realterm: Serial Terminal]: <https://realterm.sourceforge.io>
