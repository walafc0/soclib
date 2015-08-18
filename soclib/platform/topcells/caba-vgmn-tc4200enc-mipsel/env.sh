# Needed for dynamic library 
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$SOCLIB/binary/module/streaming_component/tc4200_enc/caba/lib
# Needed for #include search
export CPATH=$CPATH:$SOCLIB/binary/module/streaming_component/tc4200_enc/caba/source/include
# Needed for soclib.conf 
export TC_TC4200_ENC=$SOCLIB/binary/module/streaming_component/tc4200_enc/caba/lib
