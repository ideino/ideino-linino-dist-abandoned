var board = require('ideino-linino-lib').Board,
    html = require('ideino-linino-lib').Htmlboard,
    fs = require('fs'),
    http = require('http'),
    url = require('url'),
    path = require('path');
    
//local variable 
var pin13   = { pin: board.pin.digital.D13 , value : 0 },
    pin8    = { pin: board.pin.digital.D8 , value : 0 },
    html_img_id = 'lightbulb',
    html_txt_id = 'txtbox1';
    
var light_off ='"/html/img/lightbulb-off.jpg"',
    light_on = '"/html/img/lightbulb-on.jpg"';

board.connect(function(){
    //HTTP SERVER
    http.createServer(function(req, res) {
        var file;
        if(req.url.indexOf(".jpg") != -1 ){
            file = fs.readFileSync('html/img/'+path.basename(req.url));
            res.writeHead(200, {'Content-Type': 'image/jpeg'});
        }
        else{
            file = fs.readFileSync('html/page-server-centric-write.html'); 
            res.writeHead(200, {'Content-Type': 'text/HTML'});
        }
        res.end(file);
        
        
    }).listen(1337);
  
    board.pinMode(pin8.pin, board.MODES.INPUT);   
    board.pinMode(pin13.pin, board.MODES.OUTPUT);
        
    board.digitalRead(pin8.pin, function( data ){
        //html.write(html_txt_id,'value',data.value);
        if(data.value === 0){
            html.write(html_img_id,'src',light_off);
        }
        else{
            html.write(html_img_id,'src',light_on);
        }
        
    });  
});