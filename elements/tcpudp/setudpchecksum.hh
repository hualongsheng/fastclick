// -*- c-basic-offset: 4 -*-
#ifndef CLICK_SETUDPCHECKSUM_HH
#define CLICK_SETUDPCHECKSUM_HH
#include <click/element.hh>
#include <click/glue.hh>
CLICK_DECLS

/*
 * =c
 * SetUDPChecksum()
 * =s udp
 * sets UDP packets' checksums
 * =d
 * Input packets should be UDP in IP.
 *
 * Calculates the UDP checksum and sets the UDP header's checksum field. Uses
 * IP header fields to generate the pseudo-header.
 *
 * =a CheckUDPHeader, SetIPChecksum, CheckIPHeader, SetTCPChecksum */

class SetUDPChecksum : public Element { public:
    
    SetUDPChecksum();
    ~SetUDPChecksum();
  
    const char *class_name() const	{ return "SetUDPChecksum"; }
    const char *port_count() const	{ return PORTS_1_1; }
    const char *processing() const	{ return AGNOSTIC; }

    Packet *simple_action(Packet *);

};

CLICK_ENDDECLS
#endif
