Ethernet
========

Library for use WIZ550io with Chipkit MAX32 Board

Connect the WIZ550io and the Chipkit MAX32 Board as follow:


                                |-----       -----|
                                |      RJ-45      |
                                |    Connector    |
                                |_________________|
                                  Wiz550io Module
                                     Top view
                                -------------------
                                J1               J2
                  system GND -  1- GND   |  *******
                  system GND -  2- GND   |  3.3V -1 - system 3.3V
              Chipkit PIN 51 -  3- MOSI  |   RDY -2
              Chipkit PIN 50 -  4- MISO  |   RST -3 - system 3.3V
              Chipkit PIN 52 -  5- SLK   |    NC -4
              Chipkit PIN 53 -  6- SS    |   INT -5
                 system 3.3V -  7- 3.3V  |   GND -6 - system GND
                 system 3.3V -  8- 3.3V  |  *******
 --------------------
