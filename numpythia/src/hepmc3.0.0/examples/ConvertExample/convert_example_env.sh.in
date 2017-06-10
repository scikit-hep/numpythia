if [ -z "${PATH}" ]; then
   PATH=@bindir@; export PATH
else
   PATH=@bindir@:$PATH; export PATH
fi

if [ -z "${LD_LIBRARY_PATH}" ]; then
   LD_LIBRARY_PATH=@libdir@; export LD_LIBRARY_PATH       # Linux
else
   LD_LIBRARY_PATH=@libdir@:$LD_LIBRARY_PATH; export LD_LIBRARY_PATH
fi

if [ -z "${DYLD_LIBRARY_PATH}" ]; then
   DYLD_LIBRARY_PATH=@libdir@; export DYLD_LIBRARY_PATH   # Mac OS X
else
   DYLD_LIBRARY_PATH=@libdir@:$DYLD_LIBRARY_PATH; export DYLD_LIBRARY_PATH
fi

