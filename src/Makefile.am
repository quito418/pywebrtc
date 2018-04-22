AM_CPPFLAGS = $(CXX14_FLAGS) $(LIBWEBRTC_CFLAGS)
AM_CXXFLAGS = $(PICKY_CXXFLAGS) $(LIBWEBRTC_LIBS) -fpic

lib_LTLIBRARIES = libwebrtc_hl.la

libwebrtc_hl_la_SOURCES = ../config.h libwebrtc_hl.cc libwebrtc_hl.hh picojson.h
libwebrtc_hl_la_LDFLAGS = -version-info $(VERSION_INFO)

bin_PROGRAMS = test_webrtc
test_webrtc_SOURCES = test_webrtc.cc 
test_webrtc_LDADD = libwebrtc_hl.la

#noinst_LIBRARIES =  libnn.a
#libnn_a_SOURCES = nn.hh nn.cc \
#                  activation.hh activation.cc \
#                  convolution.hh convolution.cc \
#                  fullyconnected.hh fullyconnected.cc \
#                  pool.hh pool.cc \
#                  normalization.hh normalization.cc 
