# simple makefile to simplify repetitive build env management tasks under posix

PYTHON := $(shell which python)
CYTHON := $(shell which cython)
PYTESTS := $(shell which pytest)

CYTHON_PYX := numpythia/src/_libnumpythia.pyx
CYTHON_CPP := $(CYTHON_PYX:.pyx=.cpp)

all: clean inplace

clean-pyc:
	@find . -name "*.pyc" -exec rm {} \;

clean-so:
	@find numpythia -name "*.so" -exec rm {} \;

clean-build:
	@rm -rf build

clean: clean-build clean-pyc clean-so

.SECONDEXPANSION:
%.cpp: %.pyx $$(filter-out $$@,$$(wildcard $$(@D)/*))
	@echo "compiling $< ..."
	$(CYTHON) -a --cplus --fast-fail --line-directives $<

cython: $(CYTHON_CPP)

clean-cython:
	@rm -f $(CYTHON_CPP)

in: inplace # just a shortcut
inplace:
	@CC="ccache gcc" $(PYTHON) setup.py build_ext -i

test: inplace
	@$(PYTESTS) -s -v numpythia

sdist: clean
	@$(PYTHON) setup.py sdist

valgrind: inplace
	valgrind --log-file=valgrind.log --tool=memcheck --leak-check=full \
		 --suppressions=etc/valgrind-python.supp $(NOSETESTS) -s -v numpythia
