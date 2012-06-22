Agent.prototype.createSocket = function(name, host, port, localAddress) {
  this.sockets[name].push(s);
  var onFree = function() {
    self.emit('free', s, host, port, localAddress);
  }
  s.on('agentRemove', onRemove);
  return s;
};



if (a && b) {
};

var a=1;
