#!/usr/bin/make -f
# COPYRIGHT (C) 2004 Eklectic Ally, Inc.---------------------------{{{#
# See EKLECTIC_LICENSE for information on legal usage of this file.   #
# -----------------------------------------------------------------}}}#

#----------------------------------------------------------------------
# Uncomment and replace the following to set the name of the executable;
# otherwise, the name of the directory will be used.
#---
#MODULE=NAME_OF_EXECUTABLE

#----------------------------------------------------------------------
# Uncoment and set the following to specify which files consitute source
# files to be compiled into objects; otherwise, filenames matching *.cpp
# will be used.
#---
#SRCS=LIST_OF_CPP_FILES

#----------------------------------------------------------------------
# Default mode (exe => create executable, sim => run the simulation)
#---
#DFLT=sim

.PHONY: DEFAULT

DEFAULT:
	$(MAKE) -C .. -f Makefile $(notdir $(shell pwd))

# COPYRIGHT (C) 2004 Eklectic Ally, Inc.---------------------------{{{#
# See EKLECTIC_LICENSE for information on legal usage of this file.   #
# -----------------------------------------------------------------}}}#
#END $Id: Makefile,v 1.2 2004/04/08 15:40:40 dcblack Exp $
