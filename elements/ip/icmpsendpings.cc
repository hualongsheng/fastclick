/*
 * icmpsendpings.{cc,hh} -- Send ICMP ping packets.
 * Robert Morris
 *
 * Copyright (c) 1999-2000 Massachusetts Institute of Technology.
 *
 * This software is being provided by the copyright holders under the GNU
 * General Public License, either version 2 or, at your discretion, any later
 * version. For more information, see the `COPYRIGHT' file in the source
 * distribution.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include "icmpsendpings.hh"
#include "confparse.hh"
#include "error.hh"
#include "glue.hh"
#include "click_ip.h"
#include "click_icmp.h"

ICMPSendPings::ICMPSendPings()
    : _timer(this)
{
  add_output();
  _id = 1;
}

ICMPSendPings *
ICMPSendPings::clone() const
{
  return new ICMPSendPings;
}

int
ICMPSendPings::configure(const Vector<String> &conf, ErrorHandler *errh)
{
  return cp_va_parse(conf, this, errh,
                     cpIPAddress, "source IP address", &_src,
                     cpIPAddress, "destination IP address", &_dst,
		     0);
}

int
ICMPSendPings::initialize(ErrorHandler *)
{
  _timer.attach(this);
  _timer.schedule_after_ms(1000);
  return 0;
}

void
ICMPSendPings::uninitialize()
{
  _timer.unschedule();
}

void
ICMPSendPings::run_scheduled()
{
  WritablePacket *q = Packet::make(sizeof(click_ip) +
                                   sizeof(struct icmp_generic));
  memset(q->data(), '\0', q->length());

  click_ip *nip = reinterpret_cast<click_ip *>(q->data());
  nip->ip_v = IPVERSION;
  nip->ip_hl = sizeof(click_ip) >> 2;
  nip->ip_len = htons(q->length());
  nip->ip_id = htons(_id++);
  nip->ip_p = IP_PROTO_ICMP; /* icmp */
  nip->ip_ttl = 200;
  nip->ip_src = _src;
  nip->ip_dst = _dst;
  nip->ip_sum = in_cksum((unsigned char *)nip, sizeof(click_ip));

  icmp_generic *icp = (struct icmp_generic *) (nip + 1);
  icp->icmp_type = ICMP_ECHO;
  icp->icmp_code = 0;

  icp->icmp_cksum = in_cksum((unsigned char *)icp, sizeof(icmp_generic));

  q->set_dst_ip_anno(IPAddress(_dst));
  q->set_fix_ip_src_anno(1);
  q->set_ip_header(nip, sizeof(click_ip));

  output(0).push(q);

  _timer.schedule_after_ms(1000);
}

EXPORT_ELEMENT(ICMPSendPings)
