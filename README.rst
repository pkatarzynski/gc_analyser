gC Analyser
===========
This piece of software is capable of performing symbolic analysis of gyrator-capacitor prototype circuits described in VHDL-AMS language. The idea of symbolic analysis 
is presented `here
<http://pe.org.pl/articles/2014/9/13.pdf>`_.

Compilation
-----------

CNU Compiler Collection  (GCC) is needed for compilation There are two makefile scripts provided. Rename appropriate into makefile and issue the make command. This should result in production binary file 
in bin directory. Run the produced binary file and follow the presented instructions. 

Usage
-----

The gc_analyser is a command line tool. Currently it supports two types of work:

* Symbolic analysis. The results are stored in Structural Number (SN) file. Use ``gc_analyser SN file.vhdla`` for that option

* Matlab export. It results in producing Matlab script suitable for processing with symbolic analysis toolbox. Use ``gc_analyser MAT file.vhdla``
