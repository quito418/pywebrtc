export CFLAGS="$CFLAGS -I$(pwd)/libwebrtc/out/include"
export LDFLAGS="$LDFLAGS -L$(pwd)/../libwebrtc/out/lib"

export PYTHONPATH=$PYTHONPATH:$(pwd)/python/build/lib.linux-x86_64-3.5/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)/src/.libs/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)/../libwebrtc/out/lib
