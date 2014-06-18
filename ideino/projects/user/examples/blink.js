/***
 * file: blink.js
 * version: 3.0
 * author: https://github.com/quasto
 * license: mit
 * description: in this example the board led on digital pin 13 will 
 *      blink every 1 second .
 ***/
 
var linino = require('ideino-linino-lib'),
    board = new linino.Board();

var pin13 = { pin: board.pin.digital.D13, value : 0 },
    ctrl = true;
    
board.connect( function(){  
    board.pinMode(pin13.pin, board.MODES.OUTPUT);
    
    setInterval(function(){
        if(ctrl){
            board.digitalWrite(pin13.pin, board.HIGH);
            ctrl = false;
        }
        else{
            board.digitalWrite(pin13.pin, board.LOW);
            ctrl = true;
        }
    },1000);
});