lininoIO
==================
* upd: automatic layout board model recognition
* upd: function (sync and async) with optional callback
* upd: update examples project.

* upd: introducing servo lib

* upd: adapting servo and pwm to work in nanoseconds

0.0.6
==================
* upd: use mcu-io technology instead of bridge firmata

* upd: use mcu-io with poll linux api, remove old bridge firmata reference

* upd: remove console.log and upadate unix format end of line

0.0.5.1
==================
* add: automatic connection retry to the bridge

0.0.5
==================
* upd: setted board and htmlboard as class, changing the import mechanism in the applications
* upd: set configuration in costructor for the Board object
* upd: example project
* add: blink-web application example

* fix: board.js if the connection to the bridge fails now stop correctly the bridge-firmata script

* fix: autorun now can manage more depth folders
* fix: projects, folders and files name blank chars replacement
* upd: added serial (port, baudrate) and resolution (for analog pin) parameters for the bridge
* add: demo project: ropocop servotwins and drone
* add: firmata ath0 (.hex and .ino) sketches
* upd: name of template file.
* fix: fixed the mechanism for connect to the bridge

* upd: added sampling period parameter for the bridge

0.0.4.1
==================
* fix: set eventEmitter max listener unlimited
* fix: login.html update a reference to javascript library, from internet to local
* fix: increment the gap of the bridge from 4 to 15 for the analog pins
* add: added the firmata spi sketch (.ino and .hex) and spi library in the ext directory

* upd: every files of ideino with the reference of noide and the mit license

* upd: renamed example files

0.0.4
==================
* fix: fixed client centric callback, to reduce the number of callback.
* upd: change the python bridge command line arguments and the call from the board.
* upd: improve the python bridge with dynamic array for the pin layout.
* fix: fixed server centric htmlboard read function to work with the new client connections.
* add: added firmata_spi.hex file.

* fix: fixing page path in example projects for running at boot (autorun mode).
* upd: added the link to the github page of the authors in some file. 
* upd: changed the names of the sample files.
* add: added function for blinking a led on the board.
* add: added a 2 seconds fast blinking after board connection success.