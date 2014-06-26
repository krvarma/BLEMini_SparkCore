Spark Core, BLE Mini and [SensorTag][1] demo
---------------------------------------

This is a sample application [Spark Core][2] and [BLE Mini][3] in BLE Central role. In BLE there are two mode peripheral and central mode. A peripheral device has data that can be used by other devices. A central uses this data and and perform various tasks. The BLE Mini from RedBearLab can act as peripheral and central. To act as central we need to update the BLE Mini a [special firmware provided by RedBearLab][4]. For this demo application I am using *HCI_UART_57600bps_20130502* firmware. RedBearLab provides detailed instructions to update the firmware, you can find the information [here][5] (see the *how to upgrade* section.

This sample is heavily based on the python sample *[SensorTag_Central_HCI.py][6]*. For this I updated the firmware with the above mentioned file, taken the python sample as the reference and ported to Spark Core.

The purpose of this sample is just to demonstrate how to use the BLE Mini in Central mode and start device discovery and then connect and wait for the device events. This sample uses HCI commands to achieve these. Basically this application connect to [SensorTag][7] and subscribe for button press notification. Following are the steps to start BLE Mini in Central mode and connect to the sensor.

 1. Initialize the HCI 
 2. Once it is initialized, start device discovery 
 3. If a device is found, establish connection 
 4. Once the connection is established, enable the button press notification and wait for the events

**Screenshots**



**Demo Video**

https://www.youtube.com/watch?v=p51XCAk1nNU
 
  [1]: http://www.ti.com/ww/en/wireless_connectivity/sensortag/index.shtml
  [2]: http://www.spark.io
  [3]: http://redbearlab.com/blemini/
  [4]: https://github.com/RedBearLab/BLE_HCI/tree/master/cc2540_hci_fw
  [5]: http://redbearlab.com/blemini/
  [6]: https://github.com/RedBearLab/BLE_HCI/tree/master/python/SensorTag
  [7]: http://www.ti.com/ww/en/wireless_connectivity/sensortag/index.shtml