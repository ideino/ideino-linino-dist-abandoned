/***
 * file: blink.js
 * version: 2.0
 * author: sergio tomasello - https://github.com/quasto
 * license: mit
 * description: in this example the board led on digital pin 13 will 
 *      blink every 1 second .
 ***/
 
var board = require('ideino-linino-lib').Board;

var pin13 = { pin: board.pin.digital.D13, value : 0 },
    ctrl = board.LOW;
    
board.connect( function(){  
    board.pinMode(pin13.pin, board.MODES.OUTPUT);
    
    setInterval(function(){
        board.digitalWrite(pin13.pin,ctrl);
	    ctrl = ctrl == board.HIGH ? board.LOW : board.HIGH;
    },500);
});