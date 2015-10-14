SHELL:=/bin/bash

prog:
	-rm -rf build
	-mkdir build
	@cd build ;\
	cmake .. ;\
	make ;


run:
	./build/userprog/netagg -A sender -P 10.0.0.11:3333 -S 10.0.0.17:3333 -R 1:1

clean:
	-rm -rfv build/ 
	-rm -fv $(shell find . | grep ~$$)
	-cd kmod/; make clean
