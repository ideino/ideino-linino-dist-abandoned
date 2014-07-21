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

//digital pins
var pin13 = board.pin.digital.D13,
    pin8 = board.pin.digital.D5;
        
board.connect( function(){
    board.pinMode(pin13, board.MODES.OUTPUT);
    board.pinMode(pin8, board.MODES.INPUT);
 
    board.digitalRead(pin8, function(value){
        board.digitalWrite(pin13,value);
    });    
});