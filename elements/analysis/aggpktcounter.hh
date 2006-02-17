// -*- mode: c++; c-basic-offset: 4 -*-
#ifndef CLICK_AGGPKTCOUNTER_HH
#define CLICK_AGGPKTCOUNTER_HH
#include <click/element.hh>
#include <click/task.hh>
#include <click/notifier.hh>
#include <click/ipflowid.hh>
CLICK_DECLS

/*
=c

AggregatePacketCounter([I<keywords> PACKETNO])

=s aggregates

counts packets per packet number and aggregate annotation

=d

Maintains counts of how many packets seen for each aggregate value and packet
number.  Elements such as FromCapDump, AggregateIP and AggregateIPFlows set
the aggregate annotation; FromCapDump sets the packet number annotation too.

AggregatePacketCounter may have any number of inputs, but always has the same
number of outputs as inputs.  Packets arriving on input port I<N> are emitted
on output port I<N>.  The element maintains separate counts for each input.
See the example for how this can be used.

Keyword arguments are:

=over 8

=item PACKETNO

Integer.  Defines which packet number annotation to examine.  Must be 0, 1, or
negative; negative means ignore any packet number annotations.

=back

=h count read-only

Returns the total number of packets seen.

=h received I<AGG> read-only

Returns a newline-separated list of packet numbers in aggregate I<AGG> that
were received on any input.

=h undelivered I<AGG> read-only

Returns a newline-separated list of packet numbers in aggregate I<AGG> that
were received on input 0, but not received on input 1.  Only available if the
element has at least two inputs.

=h clear write-only

Resets all counts to zero.

=n

The aggregate identifier is stored in host byte order. Thus, the aggregate ID
corresponding to IP address 128.0.0.0 is 2147483648.

Only available in user-level processes.

=e

This configuration reads sender- and receiver-side packets from 'cap' dumps,
and writes the packet numbers of any undelivered packets to F</tmp/x>.  It
depends on FromCapDump's aggregate, packet number, and paint annotations (note
the use of CheckPaint to ignore acknowledgements).

  sender_trace :: FromCapDump(0.s, STOP true, AGGREGATE 1);
  receiver_trace :: FromCapDump(0.r, STOP true, AGGREGATE 1);
  counter :: AggregatePacketCounter;

  sender_trace -> CheckPaint(0) -> [0] counter [0] -> Discard;
  receiver_trace -> CheckPaint(0) -> [1] counter [1] -> Discard;

  DriverManager(wait_pause, wait_pause,
	save counter.undelivered1 /tmp/x);

=a

AggregateCounter, FromCapDump, AggregateIP, AggregateIPFlows */

class AggregatePacketCounter : public Element { public:

    AggregatePacketCounter();
    ~AggregatePacketCounter();

    const char *class_name() const	{ return "AggregatePacketCounter"; }
    const char *port_count() const	{ return "1-/1-"; }
    const char *processing() const	{ return AGNOSTIC; }
    const char *flow_code() const	{ return "#/#"; }

    int configure(Vector<String> &, ErrorHandler *);
    int initialize(ErrorHandler *);
    void cleanup(CleanupStage);
    void add_handlers();

    void push(int, Packet *);
    Packet *pull(int);

  private:

#ifdef HAVE_INT64_TYPES
    typedef uint64_t packetctr_t;
#else
    typedef int64_t packetctr_t;
#endif
    
    class Flow { public:

	Flow(uint32_t aggregate, int columns);

	uint32_t aggregate() const	{ return _aggregate; }
	Flow *next() const		{ return _next; }
	void set_next(Flow *next)	{ _next = next; }

	packetctr_t column_count(int column) const;
	void received(Vector<uint32_t> &, const AggregatePacketCounter *) const;
	void undelivered(Vector<uint32_t> &, const AggregatePacketCounter *) const;
	
	void add(uint32_t packetno, int column);
	
      private:

	uint32_t _aggregate;
	Flow *_next;

	Vector<uint32_t> *_counts;
	
    };
    
    enum { FLOWMAP_BITS = 10, NFLOWMAP = 1 << FLOWMAP_BITS };
    Flow *_flowmap[NFLOWMAP];
    
    uint32_t _total_flows;
    packetctr_t _total_packets;

    int _packetno;
    
    Flow *find_flow(uint32_t aggregate);
    void end_flow(Flow *, ErrorHandler *);
    inline void smaction(int, Packet *);

    static String read_handler(Element *, void *);
    typedef void (Flow::*FlowFunc)(Vector<uint32_t> &, const AggregatePacketCounter *) const;
    String flow_handler(uint32_t aggregate, FlowFunc func);
    static int thing_read_handler(int, String&, Element*, const Handler*, ErrorHandler*);
    static int write_handler(const String &, Element *, void *, ErrorHandler *);
    
};

CLICK_ENDDECLS
#endif
