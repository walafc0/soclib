# Needed for dynamic library 
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SOCLIB/binary/module/streaming_component/tc4200/caba/lib
# Needed for #include search
export CPATH=$CPATH:$SOCLIB/binary/module/streaming_component/tc4200/caba/source/include
# Needed for soclib.conf 
export TC_TC4200=$SOCLIB/binary/module/streaming_component/tc4200/caba/lib
