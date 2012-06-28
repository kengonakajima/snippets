a=1;

/*
Agent.prototype.createSocket = function(name, host, port, localAddress) {

  var self = this;
  var options = util._extend({}, self.options);
  options.port = port;
  options.host = host;
  options.localAddress = localAddress;
  var s = self.createConnection(options);
  if (!self.sockets[name]) {
    self.sockets[name] = [];
  }
  this.sockets[name].push(s);
  var onFree = function() {
    self.emit('free', s, host, port, localAddress);
  }
  s.on('free', onFree);
  var onClose = function(err) {
    self.removeSocket(s, name, host, port, localAddress);
  }
  s.on('close', onClose);
  var onRemove = function() {
    self.removeSocket(s, name, host, port, localAddress);
    s.removeListener('close', onClose);
    s.removeListener('free', onFree);
    s.removeListener('agentRemove', onRemove);
  }
  s.on('agentRemove', onRemove);
  return s;

};

function p() {
    var a=1;
};


if (a && b) {
};

var a=1;
