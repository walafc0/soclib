To create a ramdisk image containing files to be accessed by the programme:

% mkfs.vfat -C -f 1 -F 32 img.bin 1024
% mcopy     -o -i img.bin FOREMAN.264 ::

List of files:
- FOREMANA  : cif,  1 slice, 25 frames
- FOREMANB  : cif,  2 slice, 25 frames
- FOREMANC  : cif,  3 slice, 25 frames
- FOREMAND  : cif,  4 slice, 25 frames
- FOREMANE  : cif,  6 slice, 25 frames
- FOREMANF  : qcif, 1 slice, 25 frames
- FOREMANG  : qcif, 3 slice, 25 frames
- FOREMANH  : qcif, 1 slice,  2 frames (only used for tests)
