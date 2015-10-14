NetAgg
======
This is a Linux module used for aggregating IP packets belonging to a TCP connection over two network paths.

Module
------
You must first change to the kernel module directory: `cd NetAgg/kmod`

* To compile the kernel module run: `make`
* To install the kernel module run: `make install`
* To uninstall the kernel module run: `make uninstall`

Tool
----
You must first change to the project root directory: `cd NetAgg`

* To compile the tool run: `make`
* To use the tool run: `./build/userprog/netagg`

The NetAgg tool agruments

* `-A <sender|receiver>` Append new rule to rule chain
* `-F <sender|receiver>` Flush rule chain
* `-P <addr:port>` Primary destination address and port number
* `-S <addr:port>` Secondary destination address and port number
* `-R <wPrimary:wSecondary>` Ratio weight between primary and secondary path


