AUTOMAKE_OPTIONS = gnu

all: pyibmtts.so

pyibmtts.so: src/pyibmtts.pyx src/constants.pxi src/eci.pxi src/support.pxi
	python setup.py build_ext
	
clean:
	python setup.py clean
	rm src/pyibmtts.c
	rm -rf build

install-data-am: pyibmtts.so
	python setup.py install
