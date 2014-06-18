/***
 * file: lightbulb.js
 * releated files: ./html/lightbulb.html
 * version: 2.0
 * authors: https://github.com/quasto, 
 *          https://github.com/sebba
 * license: mit
 * description: this example create a simple http server that responds with a page
 *              (html/page-server-side-write.html); this page contains an <img> tag
 *              to show a light bulb image. Also there's a button connected at
 *              digital pin 8. When the button will pressed or realesed tha image
 *              of the light bulb on the page will change.
 ***/
 
var linino = require('ideino-linino-lib'),
    board = new linino.Board(),
    html = new linino.Htmlboard(),
    fs = require('fs'),
    http = require('http'),
    url = require('url'),
    path = require('path');
    
//local variable 
var pin8    = { pin: board.pin.digital.D8, value : 0 },
    html_img_id = 'lightbulb',    
    light_off ='"/html/img/lightbulb-off.jpg"',
    light_on = '"/html/img/lightbulb-on.jpg"';

board.connect(function(){
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
  
    board.pinMode(pin8.pin, board.MODES.INPUT);   
    board.digitalRead(pin8.pin, function( data ){
        if(data.value === 0)
            html.write(html_img_id,'src',light_off);
        else
            html.write(html_img_id,'src',light_on);
    });  
});