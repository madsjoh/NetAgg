SHELL:=/bin/bash

prog:
	-rm -rf build
	-mkdir build
	@cd build ;\
	cmake .. ;\
	make ;


clean:
	-rm -rfv build/ 
	-rm -fv $(shell find . | grep ~$$)
	-cd kmod/; make clean
