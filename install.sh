s="[Desktop Entry]\nName=海天鹰拼图\nComment=支持横拼、竖拼、阵列的拼图程序\nExec=`pwd`/HTYPinTu %u\nIcon=`pwd`/icon.png\nPath=`pwd`\nTerminal=false\nType=Application\nMimeType=image/bmp;image/bmp24;image/ico;image/jpg;image/jpe;image/jpeg;image/jpeg24;image/jng;image/pcd;image/pcx;image/png;image/tga;image/tif;image/tiff;image/tiff24;image/psd;image/xpm;image/dds;image/gif;image/sgi;image/j2k;image/jp2;image/pct;image/webp;image/wdp;image/cr2;image/pef;image/arw;image/nef;image/icb;image/dng;image/vda;image/vst;image/raf;image/orf;image/svg;image/ptif;image/mef;image/mrw;image/xbm;\nCategories=Graphics;"
echo -e $s > HTYPinTu.desktop
cp `pwd`/HTYPinTu.desktop ~/.local/share/applications/HTYPinTu.desktop