// -*- c-basic-offset: 4 -*-
#ifndef CLICK_FROMNETFLOWSUMDUMP_HH
#define CLICK_FROMNETFLOWSUMDUMP_HH
#include <click/element.hh>
#include <click/task.hh>
#include <click/notifier.hh>
#include "elements/userlevel/fromfile.hh"
CLICK_DECLS

/*
=c

FromNetFlowSummaryDump(FILE [, I<KEYWORDS>])

=s traces

reads packets from a NetFlow summary file

=d

Reads IP packet descriptors from a file containing NetFlow data, in the ASCII
summary format described below, then creates packets containing info from the
descriptors and pushes them out the output. Optionally stops the driver when
there are no more packets.

FILE may be compressed with gzip(1) or bzip2(1); FromNetFlowSummaryDump will
run zcat(1) or bzcat(1) to uncompress it.

Keyword arguments are:

=over 8

=item STOP

Boolean. If true, then FromNetFlowSummaryDump will ask the router to stop when
it is done reading. Default is false.

=item ACTIVE

Boolean. If false, then FromNetFlowSummaryDump will not emit packets (until
the `C<active>' handler is written). Default is true.

=item ZERO

Boolean. If true (the default), then emitted packet data is zero, except for
data set by the dump. If false, this data is random garbage.

=item MULTIPACKET

Boolean. If true, then generate multiple packets for each flow, according to
the flow's packet count. Packet timestamps vary smoothly from the flow start
timestamp to the flow end timestamp. Default is false.

=item LINK

Sets the interface type used to set the paint annotation.  Can be "input",
"output", or "both", which means the upper 4 bits of the annotation are the
input interface and the lower 4 bits are the output interface.  Too-large
interface numbers are pinned at 255 (or 15, for "both").  Default is "input".

=back

Only available in user-level processes.

=n

Packets generated by FromNetFlowSummaryDump always have IP version 4 and IP
header length 5.  The rest of the packet data is zero or garbage, unless set
by the dump.  Generated packets will usually have incorrect checksums.  The
link annotation is equal to one of the interface numbers, or a combination of
both.

=head1 FILE FORMAT

FromNetFlowSummaryDump reads line-oriented ASCII files. Blank lines, and lines
starting with `C<!>' or `C<#>', are ignored. Data lines contain 15 or more
fields separated by vertical bars `C<|>'. FromNetFlowSummaryDump pays
attention to some of these fields:

  Field  Meaning                       Example
  -----  ----------------------------  ----------
  0      Source IP address             192.4.1.32
  1      Destination IP address        18.26.4.44
  3      Input interface number        14
  4      Output interface number       16
  5      Packet count in flow          5
  6      Byte count in flow            10932
  7      Flow timestamp (UNIX-style)   998006995
  8      Flow end timestamp            998006999
  9      Source port                   3917
  10     Destination port              80
  12     TCP flags (OR of all pkts)    18
  13     IP protocol                   6
  14     IP TOS bits                   0

Generated packets have both their "first timestamp" and timestamp annotations
set. The timestamp annotation is set to the flow end timestamp unless that was
zero, in which case it is set to the flow timestamp. The "first timestamp"
annotation is always set to the flow timestamp.

FromNetFlowSummaryDump is a notifier signal, active when the element is active
and the dump contains more packets.

=h active read/write

Value is a Boolean.

=h encap read-only

Returns `IP'. Useful for ToDump's USE_ENCAP_FROM option.

=h filesize read-only

Returns the length of the NetFlow summary file, in bytes, or "-" if that
length cannot be determined.

=h filepos read-only

Returns FromNetFlowSummaryDump's position in the file, in bytes.

=a

FromDump, FromIPSummaryDump */

class FromNetFlowSummaryDump : public Element { public:

    FromNetFlowSummaryDump();
    ~FromNetFlowSummaryDump();

    const char *class_name() const	{ return "FromNetFlowSummaryDump"; }
    const char *port_count() const	{ return PORTS_0_1; }
    const char *processing() const	{ return AGNOSTIC; }
    void *cast(const char *);

    int configure(Vector<String> &, ErrorHandler *);
    int initialize(ErrorHandler *);
    void cleanup(CleanupStage);
    void add_handlers();

    bool run_task();
    Packet *pull(int);

  private:

    FromFile _ff;

    Vector<int> _contents;
    
    bool _stop : 1;
    bool _format_complaint : 1;
    bool _zero;
    bool _active;
    bool _multipacket;
    uint8_t _link;
    Packet *_work_packet;
    uint32_t _multipacket_length;
    Timestamp _multipacket_timestamp_delta;
    Timestamp _multipacket_end_timestamp;

    Task _task;
    ActiveNotifier _notifier;

    Timestamp _time_offset;

    Packet *read_packet(ErrorHandler *);
    Packet *handle_multipacket(Packet *);

    static String read_handler(Element *, void *);
    static int write_handler(const String &, Element *, void *, ErrorHandler *);
    
};

CLICK_ENDDECLS
#endif
