var board = require('ideino-linino-lib').Board,
    html = require('ideino-linino-lib').Htmlboard,
    fs = require('fs'),
    http = require('http');
    
//local variable 
var pin13   = { pin: board.pin.digital.D13 , value : 0 },
    html_btn_id = 'btnswitch';
    
board.connect(function(){
    //HTTP SERVER
    http.createServer(function(req, res) {
        file = fs.readFileSync('html/page-server-centric-read.html'); 
        res.writeHead(200, {'Content-Type': 'text/HTML'});
        res.end(file);
    }).listen(1337);
  
    board.pinMode(pin13.pin, board.MODES.OUTPUT);    
    
    html.read(html_btn_id,'value',function(data){console.log(data);
        if(data.value == 'ON')
            board.digitalWrite(pin13.pin,board.HIGH);
        else
            board.digitalWrite(pin13.pin,board.LOW);
    });  
});