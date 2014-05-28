/***
 * file: button.js
 * version: 3.0
 * authors: https://github.com/quasto
 * license: mit
 * description: in this example there is a button connected at digital pin 8.
 *      when the button is pressed the led on board (digital pin 13) will turn on, 
 *      when the button is released the led will turn off.
 ***/

var linino = require('ideino-linino-lib'),
    board = new linino.Board({logger: {level:'error'}});

//local variable 
var pin13   = { pin: board.pin.digital.D13 , value : 0 },
    pin8    = { pin: board.pin.digital.D8 , value : 0 };
    
board.connect( function(){
    board.pinMode(pin13.pin, board.MODES.OUTPUT);
    board.pinMode(pin8.pin, board.MODES.INPUT);
 
    board.digitalRead(pin8.pin, function(data){
        board.digitalWrite(pin13.pin,data.value);
    });    
});