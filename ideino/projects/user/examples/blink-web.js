/***
 * file: blink.js
 * version: 1.0
 * author: https://github.com/quasto
 * license: mit
 * description: in this example the board led on digital pin 13 and
 *      a lightbulb image on web page will blink every 1 second .
 ***/
 
var linino = require('ideino-linino-lib'),
    board = new linino.Board(),
    html = new linino.Htmlboard(),
    fs = require('fs'),
    http = require('http'),
    url = require('url'),
    path = require('path');
    
var pin13 = board.pin.digital.D13,
    html_img_id = 'lightbulb',    
    light_off ='"/html/img/lightbulb-off.jpg"',
    light_on = '"/html/img/lightbulb-on.jpg"',
    light = light_off, 
    ctrl = board.LOW;
    
board.connect( function(){  
    //SIMPLE HTTP SERVER
    var file, page;
    http.createServer(function(req, res) {
        if(req.url.indexOf(".jpg") != -1 ){
            page = path.join(__dirname,'html','img',path.basename(req.url));
            file = fs.readFileSync(page);
            res.writeHead(200, {'Content-Type': 'image/jpeg'});
        }
        else{
            page = path.join(__dirname,'html','lightbulb.html');
            file = fs.readFileSync(page); 
            res.writeHead(200, {'Content-Type': 'text/HTML'});
        }
        res.end(file);        
    }).listen(1337);
    
    board.pinMode(pin13, board.MODES.OUTPUT);
    
    setInterval(function(){
        board.digitalWrite(pin13, ctrl);
        html.write(html_img_id,'src', light);
        light = ctrl == board.HIGH ? light_off : light_on;
        ctrl = ctrl == board.HIGH ? board.LOW : board.HIGH;        
    },1000);
});