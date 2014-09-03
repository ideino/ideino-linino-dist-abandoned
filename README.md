# Ideino & linino-lib

Ideino IDE for Linino OS with the linino-lib deployed and ready to use.

Ideino IDE is a project based on [davidjamesstone's noide](http://github.com/davidjamesstone/noide) with some optimizations and more features than the original.

## Installation

Take a look [here](http://wiki.linino.org/doku.php?id=wiki:nodejscript) on the Linino wiki to perform  the installation of Node.js and Ideino with linino-lib.

To use linino-lib, you need to upgrade your Linino OS version to XXX or higher and install the corresponding mcu firmware. You will find the firmware in `/etc/linino/` folder.

Once you have browsed it via SSH connection, install it with the `run-avrdude` tool :

    > cd /etc/linino/
    > run-avrdude bathos-mcuio.hex
    ....
    > reboot

## How to use linino-lib
### Getting started  

Firstly, start your javascript code by importing the linino-lib :

    var linino = require('ideino-linino-lib');

To access the input and output pins of your board, use the following :

    var board = new linino.Board();

To interact with a web object create an **Htmlboard** object:

    var html = new linino.Htmlboard();

Before accessing the input and output pins, you have to make a connection to the board itself. The **callback** function is triggered whenever the board is successfully connected and then your code will be able to interact with the I/O pins.

	board.connect(function(){
		//your code here!
	});

### Board
#### Properties and Functions 
The board object has these main properties :

 - **HIGH**: is the high signal level for digital pins.
 - **LOW**: is the low signal level for digital pins.
 - **MODES**: contains the operating mode of a pin
    - **.OUTPUT** is the output operating mode of a digital pin.
    - **.INPUT**  is the input operating mode of a digital pin (analog are in input mode by default).
    - **.PWM** is the pwm operating mode of a digital pin. 
    - **.SERVO** is the servo operating mode of a digital pin.
 - **pin**: contains all the available pins of the board, grouped by typology :
    - **.digital** 
    - **.analog**
    - **.pwm**
    - **.servo**
 - **logger**: is the log object used by the library with _info_, _debug_ and _error_ level, console and file appender (see advanced configuration)
 - **model**: is the model name of the board

The board object has these embedded functions :

 - **[`connect`](#connect-callback)**: connect to the board and initialize it.
 - **[`pinMode`](#pinmode-pin-mode-pull-up)**: set pin mode, used for digital pin only.
 - **[`digitalWrite`](#digitalwrite-pin-value-callback)**: send HIGH or LOW level signal to a digital pin.
 - **[`digitalRead`](#digitalread-pin-callback)**: get signal level of the specified digital pin.
 - **[`analogWrite`](#analogwrite-pin-value-callback)**: send a value output to specified pwm pin.
 - **[`analogWritens`](#analogwritens-pin-value-callback)**: send a value output to specified pwm pin.
 - **[`analogRead`](#analogread-pin-options-callback)**: get value from analog input pin.
 - **[`servoWrite`](#servowrite-pin-angle-callback)**: send degrees value to specified pwm pin.
 - **[`map`](#map-value-fromlow-fromhigh-tolow-tohigh)**: Maps an integer value from one range to another.
 - **[`tone`](#tone-pin-frequency-duration-callback)**: send tone output to specified pwm pin. 
 - **[`noTone`](#notone-pin-callback)**: send tone pause output to specified pwm pin.

##### **connect (callback)**
It is the first function you must call before start working with your board. You will usually write your code inside the `callback` function that will be triggered once the board it's ready to be used : 

    var linino = require('ideino-linino-lib'),
    	board = new linino.Board();
    
    board.connect(function(){
    	//your code here!
    });


##### **pinMode (pin, mode, [pull-up])**
With pinMode you can set the operating mode of a **digital pin** : _pwm_, _input_ or _output_. Example :
    
    var linino = require('ideino-linino-lib'),
    	board = new linino.Board(),
    	led = board.pin.digital.D13;
    
    board.connect(function(){
    	board.pinMode(led, board.MODES.OUTPUT);	
    });	

The `pull-up` parameter allows you to enable pull-up to the specified **input pin**. It's an optional parameter and is set _false_ by default:

    board.pinMode(board.pin.digital.D5, board.MODES.INPUT, true);	
    
##### **digitalWrite (pin, value, [callback])**
digitalWrite allow you to set HIGH or LOW signal level to the digital output pins :
	
	var linino = require('ideino-linino-lib'),
		board = new linino.Board(),
		led = board.pin.digital.D13;
	
	board.connect(function(){
		board.pinMode(led, board.MODES.OUTPUT);
		board.digitalWrite(led, board.HIGH);
	});	
	
##### **digitalRead (pin, [callback])**
used for reading the values from a generic digital pin. The callback function is triggered whenever the signal level on the specified pin is changed :
	
	var linino = require('ideino-linino-lib'),
		board = new linino.Board(),
		button = board.pin.digital.D10;
	
	board.connect(function(){
		board.pinMode(button, board.MODES.INPUT);
		board.digitalRead(button, function(value){
			if(value == board.HIGH)
				console.log("button pressed");
			else
				console.log("button released");
		});
	});		

##### **analogWrite (pin, value, [callback])**
if a digital pin is set to pwm mode, you can write an nbit value (0-100) to it.
	
	var linino = require('ideino-linino-lib'),
		board = new linino.Board(),
		led = board.pin.pwm.11;
	
	board.connect(function(){
		board.pinMode(led, board.MODES.PWM);
		board.analogWrite(led, 100);
	});	
	
##### **analogWritens (pin, value, period, [callback])**
This function allow you to write a nanoseconds value to specified pwm pin.
In `value` parameter you can set a dutycycle for the specified **pwm pin**.
Pwm pin have the following dutycycle range value:
	- D5,D9,D10,D11:	0-32768000
    - D6:				0-16384000
	- D3: 				0-4000000
	
	var linino = require('ideino-linino-lib'),
		board = new linino.Board(),
		led = board.pin.pwm.11;
	
	board.connect(function(){
		board.pinMode(led, board.MODES.PWM);
		board.analogWritens(led, 10000000, 20000000);
	});	
	
The `period` parameter allows you to set a period for the specified **pwm pin**. It's an optional parameter and is 10000000 default.
pwm pin D3 is period fixed to 4000000 and is not editable.

##### **analogRead (pin, [options], [callback])**
As the digitalRead function, it is used to reading the analog pins values. 

As the previous case, the `callback` function is triggered whenever the the signal level on the specified `pin` change. `

The `options` argument is not mandatory, but if specified you can set the sampling period and the resolution of the signal level for a generic `pin`. 

By default the sampling period and the resolution are specified in the configuration file `config.json` and are the same for all the analog pins. See the [Advanced configuration](#advanced-configuration) paragraph for more details on how to use this options.
	
	var linino = require('ideino-linino-lib'),
		board = new linino.Board(),
		temp = board.pin.analog.A0;
	
	board.connect(function(){
		board.analogRead(temp, function(value){
			console.log('value: ' + value);
		});
	});	
	
##### **servoWrite (pin, angle, [callback])**
This function allow you to write an degrees value between 0-180 on pwm pin specified.
It work on pwm pin D5,D9,D10,D11.

	var linino = require('ideino-linino-lib'),
		board = new linino.Board(),
		stepper = board.pin.servo.S5;
	
	board.connect(function(){	
		board.pinMode(stepper, board.MODES.SERVO);
		//set stepper angle to 180 degree.
		board.servoWrite(stepper, 180);
	});	

##### **tone (pin, frequency, duration, [callback])**
if a digital pin is set to pwm mode, you can write a tone to it.
The `frequency` parameter allows you to set the frequency of tone you want send to the specified **pwm pin**.

	
	var linino = require('ideino-linino-lib'),
		board = new linino.Board(),
		buzzer = board.pin.pwm.11;
	
	board.connect(function(){
		board.pinMode(buzzer, board.MODES.PWM);
		//in this case we send a 41Hz frequency sound to 'buzzer' pin for 120ms
		board.tone(buzzer, 41, 120);
	});	
	
The `duration` parameter allows you to set a duration time in milliseconds to the tone value. It's an optional parameter and is 1 default.

##### **noTone (pin, [callback])**
it send a 'no sound' signal to specidied **pwm pin**.

	var linino = require('ideino-linino-lib'),
	    board = new linino.Board(),
	    buzzer = board.pin.pwm.11;
	 
	    board.connect(function(){
		  board.pinMode(buzzer, board.MODES.PWM);
		  board.noTone(buzzer);
	});
	
##### **blink (delay, duration, led)**
This function allows you to send `HIGH` and `LOW` signal levels, with the specified `delay` and for the specified `duration` value, to the specified digital `pin`:
	
	var linino = require('ideino-linino-lib'),
		board = new linino.Board(),
		led = board.pin.digital.D13;
	
	board.connect(function(){
		board.pinMode(led, board.MODES.OUTPUT);
		//blink led 13, every 100 millis for 10 seconds.
		board.blink(500, 10 * 1000, led);
	});	

	
##### **map (value, fromLow, fromHigh, toLow, toHigh)**
Maps an integer value from one range to another (starting range [fromLow-toLow], final range [fromHigh-toHigh]). Return value is a float. 	
	
	var linino = require('ideino-linino-lib'),
		board = new linino.Board();
 
		board.connect(function(){
		//map value 80 from 0-100 to 0-5. Result is 4
			var val = board.map(80, 0, 100, 0, 5)
		});
	
	
#### Pins
pins configuration is automatically loaded from the layout file, and then they are available in the `board.pin` object. You can access them in different ways, for example consider the digital pin 13 :

     1. board.pin.digital.D13;
     2. board.pin.digital['D13'];
     3. board.pin.digital[13];
     4. board.pin.digital['13'];

In the **Linino One** board, the available pins are:

 - Digital: 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 
 - Analog: 0, 1, 2, 3, 4, 5
 - Pwm: 3, 11
 - Servo: **//TODO**

#### Advanced configuration
*linino-lib* has many advanced configurations which are loaded through a configuration file `config.json`:

    { 	
    	"logger": 	{ 
    					"off":		false,
    					"level":	"info",
    					"handler":	"all"
    				},
    	"resolution":	15,
    	"sampling":		25
    }

you can edit it if you want always use your personal settings, otherwise you can set your custom parameters when invoking the `Board` constructor. In this case it's very important to maintain the same structure of the configuration file. 
	
#### Setting and using the Logger
The library use a logger utility based on the *winston* module. The logger has three logging levels: _debug_, _info_ and _error_, and two appenders: _console_ and _file_. There are three arguments available for the logger:

 - **off** : if `true` the logger is disabled, if `false` (default) the logger is enabled.
 - **level** : is the logger level. As explained above the possible levels are: `info` (default), `debug` and `error`.
 - **handler** : is the appender of the logger. You can set `file` for writing your log on a file (inside the logs directory of linino-lib) or you can set it to `console` if you just want the log on the Ideino console (or system console, if you run it manually). Finally, if you want both the logger appenders, you can specify `all` (default).
	
if you want to switch from _info_  to _debug_ level and viceversa during the development, without changing the configuration files, you can set the logging level in the `Board` constructor, for example :

    var linino = require('ideino-linino-lib'),
    	board = new linino.Board( {logger: {level: 'debug'}} );
	
if you also want to disable the file appender :

	board = new linino.Board( {logger: {level: 'debug', handler: 'console'}} );

After the board has reached the connection status, the `Board` object gives you the chance to declare a *logger* object so, instead of using console.log(), you can leverage the `board.logger` callback. For example:

    var linino = require('ideino-linino-lib'),
    	board = new linino.Board({logger:{level: 'debug', handler: 'console'}}),
    	button = board.pin.digital.D10;
	
	board.connect(function(){
		board.digitalRead(button, function(value){
			if(value == board.HIGH)
				board.logger.debug('button pressed');
			else
				board.logger.debug('button released');
		});
	});	
#### Setting the analog parameters
In the configuration file there are two parameters called `sampling` and `resolution` that are usually associated with the analog pins. We use them to configure the reading of the analog values from the pins. 

**The first one** is used to set the sampling period of the analog values and represents the delay in milliseconds of the polling cycle. **The second one** triggers the signal value change to the user's callback and so represents the smallest change that will be triggered ever. These two parameters can be set, as well as in the configuration file, even in the `Board` constructor or in the `Board` `analogRead` function.

Setting sampling and resolution in the `Board` constructor is quite simple and you can do it as before for the `logger` properties. For example, if you don't need to check immediately the change of temperature detected by a sensor, you can set the `sampling` period to 30 seconds:

    board = new linino.Board( {sampling: 30000} );

Or if you don't need extreme accuracy, you can set the `resolution` to 50, whereas the values varies between 0 and 1023.

	board = new linino.Board( {resolution: 50} );

When you set `resolution` and `sampling` properties through the `Board` constructor, they usually become the default properties for all the used analog pins 

In case you needed an analog pin working with different properties than the other analog ones, you can directly set its `resolution` and `sampling` in the `analogRead` `Board` function for that one :

	board.connect(function(){
		var options = {resolution: 30, sampling: 1000};
        		
		board.analogRead(temp, options, function(value){
			board.logger.info('value: ' + value);
		});
	});	
	
	
	
	
### Htmlboard ###
**//TODO**