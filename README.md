NetAgg
======
This is a Linux module used for aggregating IP packets belonging to a TCP connection over two network paths.

Module
------
* To compile the kernel module run: `:~/NetAgg/kmod$ make`
* To install the kernel module run: `:~/NetAgg/kmod$ make install`
* To uninstall the kernel module run: `:~/NetAgg/kmod$ make uninstall`

Tool
----
* To compile the tool run: `:~/NetAgg$ make`
* To run the NetAgg tool: `:~/NetAgg/build/userprog$ ./netagg`

NetAgg tool arguments

* `-A <sender|receiver>` Append new rule to rule chain
* `-F <sender|receiver>` Flush rule chain
* `-P <addr:port>` Primary destination address and port number
* `-S <addr:port>` Secondary destination address and port number
* `-R <wPrimary:wSecondary>` Ratio weight between primary and secondary path

Example

If we have a multihomed receiver which is addressed with 10.0.0.11 and 10.0.0.12, and the bandwidth to these interfaces is 7500 Kbit/s and 2500 Kbit/s. We can set this up as follows:

Sender side

* `:~/NetAgg/build/userprog$ ./netagg -A sender -P 10.0.0.11:1234 -S 10.0.0.12:1234 -R 7500:2500`

Receiver side

* `:~/NetAgg/build/userprog$ ./netagg -A receiver -P 10.0.0.11:1234 -S 10.0.0.12:1234`

Configure hosts
---------------
To enable our receiver to comunicate over multiple interfaces we need to set up a routing table for each interface. We also need to configure ARP and the reverse path filter. We also need to enable forwarding.

ARP
* `sudo sysctl -w net.ipv4.conf.all.arp_filter=1`
* `sudo sysctl -w net.ipv4.conf.<primary interface>.arp_filter=1`
* `sudo sysctl -w net.ipv4.conf.<secondary interface>.arp_filter=1`
 
RP filter
* `sudo sysctl -w net.ipv4.conf.all.rp_filter=2`
* `sudo sysctl -w net.ipv4.conf.<primary interface>.rp_filter=2`
* `sudo sysctl -w net.ipv4.conf.<secondary interface>.rp_filter=2`

IP forwarding
* `sudo sysctl -w net.ipv4.ip_forward=1`

Routing tables
* `sudo ip route add <primary net addr> dev <primary interface> src <primary IP addr> table 1`
* `sudo ip route add <secondary net addr> dev <secondary interface> src <secondary IP addr> table 2`
* `sudo ip route add default via <primary gateway> dev <primary interface> table 1`
* `sudo ip route add default via <secondary gateway> dev <secondary interface> table 2`
* `sudo ip rule add from <primary IP addr> table 1`
* `sudo ip rule add from <secondary IP addr> table 2`

To enable our sender to forward packets we need to enable forwarding.
* `sudo sysctl -w net.ipv4.ip_forward=1`
