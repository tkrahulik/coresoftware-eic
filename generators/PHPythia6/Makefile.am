AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS = -I$(includedir) -I$(OFFLINE_MAIN)/include -I`root-config --incdir`
AM_LDFLAGS = -L$(libdir) -L$(OFFLINE_MAIN)/lib -L$(OPT_PHENIX)/lib

AM_CXXFLAGS = -Wall -Werror

lib_LTLIBRARIES = libPHPythia.la libPHPythiaEventGen.la

include_HEADERS = \
  PHPythiaContainer.h \
  PHPythiaContainerV2.h \
  PHPythiaHeader.h \
  PHPythiaHeaderV1.h \
  PHPythiaHeaderV2.h \
  PHPyOscarOutputManager.h \
  PHPythiaAncestryHelper.h \
  PHPythia.h \
  PHPyCommon.h

libPHPythia_la_LDFLAGS = ${AM_LDFLAGS} `root-config --libs`
libPHPythia_la_LIBADD = \
  -lphool -lSubsysReco -lfun4all -lPythia6 -lEG -lEGPythia6 \
  -lPHHepMCNode

libPHPythia_la_SOURCES = \
  PHPythiaHeader.C \
  PHPythiaHeaderV1.C \
  PHPythiaHeaderV2.C \
  PHPythiaContainer.C \
  PHPythiaContainerV2.C \
  PHPyOscarOutputManager.C \
  PHPythiaAncestryHelper.C \
  PHPythia_Dict.C

libPHPythiaEventGen_la_DEPENDENCIES = libPHPythia.la

libPHPythiaEventGen_la_LIBADD = \
  -lPHPythia

libPHPythiaEventGen_la_SOURCES = \
  PHPythia.C \
  PHPythiaEventGen_Dict.C


BUILT_SOURCES = \
  testexternals.C

noinst_PROGRAMS = \
  testexternals

testexternals_LDADD = \
  libPHPythiaEventGen.la

testexternals.C:
	echo "//*** this is a generated file. Do not commit, do not edit" > $@
	echo "int main()" >> $@
	echo "{" >> $@
	echo "  return 0;" >> $@
	echo "}" >> $@

PHPythia_Dict.C: \
  PHPyOscarOutputManager.h \
  PHPythiaHeaderV1.h \
  PHPythiaHeaderV2.h \
  PHPythiaContainerV2.h \
  PHPythiaAncestryHelper.h \
  PHPythiaLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

PHPythiaEventGen_Dict.C: \
  PHPythia.h \
  PHPythiaEventGenLinkDef.h
	rootcint -f $@ -c $(DEFAULT_INCLUDES) $(AM_CPPFLAGS) $^

clean-local:
	rm -f *Dict*
	rm -f testexternals*

