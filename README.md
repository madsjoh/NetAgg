NetAgg
======
This is a Linux module used for aggregating IP packets belonging to a TCP connection over two network paths.

Usage
-----
1. To compile and install module: `cd kmod; make; make install; cd ..;`
2. To compile NetAgg tool that manipulates rule chains in the kernel module, run: `make` in the project root folder
3. To load new rules into the kernel module run: `./build/userprog/netagg <arguments>`
