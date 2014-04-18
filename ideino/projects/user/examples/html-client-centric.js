var board = require('ideino-linino-lib').Board,
    html = require('ideino-linino-lib').Htmlboard,
    fs = require('fs'),
    http = require('http');
    

board.connect(function(){
  //HTTP SERVER
  http.createServer(function(req, res) {
    fs.readFile('html/page-client-centric.html', function(err, contents) {
      if (!err) {
        res.writeHead(200, {'Content-Type': 'text/HTML'});
        res.end(contents);
      } else {
        console.dir(err);
      }
    });
  }).listen(1337);
});