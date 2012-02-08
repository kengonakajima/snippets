#!/usr/bin/python
# Copyright (c) 2001-2004 Twisted Matrix Laboratories.
# See LICENSE for details.

#

from twisted.internet.protocol import DatagramProtocol
from twisted.internet import reactor

# Here's a UDP version of the simplest possible protocol
class EchoUDP(DatagramProtocol):
    def datagramReceived(self, datagram, address):
        self.transport.write(datagram, address)

def main():
    reactor.listenUDP(8100, EchoUDP())
    reactor.run()

if __name__ == '__main__':
    main()
