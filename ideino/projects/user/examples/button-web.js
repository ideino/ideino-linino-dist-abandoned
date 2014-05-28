/***
 * file: button-web.js
 * releated files: ./html/button-web.html
 * version: 2.0
 * authors: https://github.com/quasto, 
 *          https://github.com/sebba
 * license: mit
 * description: this example create a simple http server that responds with a page
 *              (html/page-server-side-read.html); this page contains button which 
 *              update its param value in the onclick function. The example below 
 *              will read the param value of the button and send the LOW or HIGH 
 *              value to the digital LED 13.
 ***/

var linino = require('ideino-linino-lib'),
    board = new linino.Board(),
    html = new linino.Htmlboard(),
    fs = require('fs'),
    http = require('http'),
    path = require('path');
    
//local variable 
var pin13   = { pin: board.pin.digital.D13 , value : 0 },
    html_btn_id = 'btnswitch';
    
board.connect(function(){
    //SIMPLE HTTP SERVER
    var file, page;
    http.createServer(function(req, res) {
        page = path.join(__dirname,'html','button-web.html');
        file = fs.readFileSync(page);
        res.writeHead(200, {'Content-Type': 'text/HTML'});
        res.end(file);
    }).listen(1337);
  
    board.pinMode(pin13.pin, board.MODES.OUTPUT);    
    
    html.read(html_btn_id,'value',function(data){
        if(data.value == 'ON')
            board.digitalWrite(pin13.pin,board.HIGH);
        else
            board.digitalWrite(pin13.pin,board.LOW);
    });  
});