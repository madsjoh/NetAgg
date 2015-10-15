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

If we have a multihomed receiver which is addressed with 10.0.0.11 and 10.0.0.12. The bandwidth to these interfaces could be 7500 Kbit/s and 2500 Kbit/s. To set this up we do as follows:

Sender side

* `:~/NetAgg/build/userprog$ ./netagg -A sender -P 10.0.0.11:1234 -S 10.0.0.12:1234 -R 7500:2500`

Receiver side

* `:~/NetAgg/build/userprog$ ./netagg -A receiver -P 10.0.0.11:1234 -S 10.0.0.12:1234`

Configure multihomed receiver
-----------------------------


