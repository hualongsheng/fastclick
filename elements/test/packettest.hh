// -*- c-basic-offset: 4 -*-
#ifndef CLICK_PACKETTEST_HH
#define CLICK_PACKETTEST_HH
#include <click/element.hh>

/*
=c

PacketTest()

=s test

runs regression tests for Packet

=io

None

=d

PacketTest runs Packet regression tests at initialization time. It does not
route packets.

=a

CheckPacket */

class PacketTest : public Element { public:

    PacketTest();
    ~PacketTest();

    const char *class_name() const		{ return "PacketTest"; }
    PacketTest *clone() const			{ return new PacketTest; }

    int initialize(ErrorHandler *);

};

#endif
