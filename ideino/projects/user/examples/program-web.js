/***
 * file: html-client-side.js
 * releated files: ./html/page-client-centric.html
 * version: 1.0
 * authors: https://github.com/quasto, 
 *          https://github.com/sebba
 * license: mit
 * description: this example create a simple http server that responds with a page
 *              (html/page-client-side.html). This page contains some buttons that 
 *              allows the user to send the common commands directly to the board, 
 *              ie: pinMode, digitalRead, digitalWrite, etc..
 ***/

var board = require('ideino-linino-lib').Board,
    html = require('ideino-linino-lib').Htmlboard,
    fs = require('fs'),
    http = require('http'),
    path = require('path');
    
board.connect(function(){
  //SIMPLE HTTP SERVER
  http.createServer(function(req, res) {
    var page = path.join(__dirname, 'html', 'page-client-side.html');
    fs.readFile(page, function(err, contents) {
      if (!err) {
        res.writeHead(200, {'Content-Type': 'text/HTML'});
        res.end(contents);
      } 
      else {
        console.dir(err);
      }
    });
  }).listen(1337);
});