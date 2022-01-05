# Nucleo-F429ZI - Environment Parameter Sensing and Logging - Ethernet

<img src="images/setup.jpg" width="500" />

1 - About this project
2 - Required Software
3 - Required Hardware
4 - Hardware setup
5 - Software setup
6 - Code & Settings
7 - Operation
8 - Known Issues
9 - Folder Structure
10 - Reference Sites
11 - Todos

# 1 - About this project
This is a project based on the Nucleo-F429ZI microcontroller board and the SVM30 sensor module. The programming language is C. The aim is to capture several environmental parameters, and send them through the Ethernet to a data-logging system/computer.

 - Tasks
 -- Sensor measures: Temprature, Relative Humidity(RH), Carbon Dioxide equivalent(CO2eq) and Total Volatile Organic Compounds(TVOC).
 -- Board processes raw data to get calculated, readable values.
 -- Board transmit data through the UDP protocol to a designated IP address.

 - Additional features
 -- Dedicated sensor library
 -- Sleep mode for board -> active only during a defined, recurring period of data capture, calculation, and transmission
 -- Real-time data visualization -> Matlab script
 -- Network packet capture -> Wireshark

# 2 - Required Software
| Name | Usage |
| ------ | ------ |
| LwIP | Network stack |
| STM32CubeIDE | Programmer and debugger |
| STM32Cube FW_F4 V1.25.2 | Board firmware package |
| MATLAB2020b | Real-time data monitoring |
| Wireshark | Real-time UDP packet monitoring |

# 3 - Required Hardware
| Name | Usage |
| ------ | ------ |
| SVM30 | Sensor module |
| Nucleo-F429ZI | Microcontroller board |
| Computer | Data-logging |

 - Board alternatives with the following resources: 
  -- I2C bus * 1
  -- Sleep mode capability
  -- Power supply: 5V, 49mA
  -- Ethernet with lwIP stack support    
  -- General purpose interrupt timer * 1

# 4 - Hardware setup
<img src="images/connections.jpg" width="500" />
 | Pin (board) | To Pin (Sensor) |
 | ------ | ------ |
 | CN8 - 5V | 3 - VDD |
 | CN7 - PB_8 | 1 - SCL |
 | CN7 - PB_9 | 4 - SDA |
 | CN7 - GND | 2 - GND |
 
 | Also | To... |
 | ------ | ------ |
 | Ethernet port | Local network |
 | ST-Link USB PWR | Computer usb port for programming|
 |  | Any standard usb port (PC, powerbank) for operation|
 
# 5 - Software setup
 | Method 1 - Complete package |  |
 | ------ | ------ |
 | Step 1 | Extract Clean_UDP_Remote.rar |
 | Step 2 | Import project to CubeIDE |
 | Step 3 | Connect board and run program |

 | Method 2 - Essential files |  |
 | ------ | ------ |
 | Step 1 | Create a new Nucleo-F429ZI project on CubeIDE |
 | Step 2 | Put all essential files into their respective folders according to 8 - Folder Structure |
 | Step 3 | Open .ioc file and generate code through CubeMX |
 | Step 4 | Connect board and run program |

# 6 - Code & Settings

*Below are settings that could be tweaked to quickly adapt to different setup environments. For full settings, please check the files themselves.

- Clock settings - Clean_UDP_Remote.ioc
<img src="images/clock settings.png" width="500" />

- Network settings - Clean_UDP_Remote.ioc
<img src="images/network settings.png" width="500" />

- Packet message settings - main.c
-- All four measurements are sent in one single packet as char. strings.
-- Modify measurement string separators in sprintf() in void doTasks(struct pbuf *pbufPtr), 

# 7 - Operation
 | Start |  |
 | ------ | ------ |
 | Step 1 | Turn on board power supply |
 | Step 2 | Wait for >15s |
 | Step 3 | Alternating blue and red LED -> board is now capturing and sending data |

 | Monitoring |  |
 | ------ | ------ |
 | Data update | Outgoing/Incoming measurement data every 1s |
 | Matlab script | Automatically shows live plots and saves data to a matrix type |
 | Wireshark | Enter "ip.addr == 10.183.93.46" into filter to display packets from the board |

 | End of program |  |
 | ------ | ------ |
 | Board turn-off | Directly turn off power source |
 
 Examples - Wireshark and Matlab
 <img src="images/Wireshark.png" width="500" />
 <img src="images/Matlab.png" width="500" />
 
# 8 - Known issues
 - I2C pin mismatch
   - After every code generation from editing the .ioc file
   - Need to change GPIO_PIN_6 to GPIO_PIN_8 in i2c configurarion file (e.g. i2c.c or main.c)
 - Matlab script performance 
   - After some amount of date accumulation
   - Very slow figure zoom-in and zoom-out

# 9 - Folder Structure
 | Important File | Usage |
 | ------ | ------ |
 | Clean_UDP_Remote.rar | Ready-to-use package |
 | Clean_UDP_Remote.ioc | Board configuration |
 | main.c | Main program |
 | SVM30.h and libSVM30.c | Sensor library |
 | Clean_UDP_RT_plots_multiple.m | Matlab script |

```bash
├───Clean_UDP_Remote.rar
│   └───Clean_UDP_Remote
│       └───Clean_UDP_Remote.ioc
│       └───Core
│           └───Inc
│               └───SVM30.h
│           └───Src
│               └───main.c
│               └───libSVM30.c
├───Essential files
│   └───Clean_UDP_Remote.ioc
│   └───main.c
│   └───SVM30.h
│   └───libSVM30.c
├───Images
├───Clean_UDP_RT_plots_multiple.m
```
- Using STM32CubeIDE, all the other files are automatically generated after importing the .ioc file.

# 10 - Reference sites
| Name | Link |
| ------ | ------ |
| SVM30 datasheet | [Mouser Electronics][PlSVM30] |
| NucleoF429ZI pintout | [Mbed][PlMbed] 
| NucleoF429ZI datasheet | [STMicroelectronics][PlNucleo] 
| LwIP for STM32F4 | [UM1713 User manual][PlLwIP] |

# 11 - Todos
 - Use TCP mode
 - Send HTTP packets

   [PlSVM30]: <https://eu.mouser.com/datasheet/2/682/Sensirion_Gas_Sensors_SVM30_Datasheet-1539043.pdf>
   [PlNucleo]: <https://www.st.com/en/evaluation-tools/nucleo-f429zi.html#documentation>
   [PlMbed]: <https://os.mbed.com/platforms/ST-Nucleo-F429ZI/>
   [PlLwIP]: <https://www.st.com/resource/en/user_manual/dm00103685-developing-applications-on-stm32cube-with-lwip-tcpip-stack-stmicroelectronics.pdf>

