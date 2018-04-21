AM_CPPFLAGS = $(CXX14_FLAGS) $(LIBWEBRTC_CFLAGS)
AM_CXXFLAGS = $(PICKY_CXXFLAGS) -fpic

lib_LTLIBRARIES = libwebrtc_hl.la

libwebrtc_hl_la_SOURCES = common.hh libwebrtc_hl.cc libwebrtc_hl.hh
libwebrtc_hl_la_LDFLAGS = -version-info 0:0:0

#noinst_LIBRARIES =  libnn.a
#libnn_a_SOURCES = nn.hh nn.cc \
#                  activation.hh activation.cc \
#                  convolution.hh convolution.cc \
#                  fullyconnected.hh fullyconnected.cc \
#                  pool.hh pool.cc \
#                  normalization.hh normalization.cc 
