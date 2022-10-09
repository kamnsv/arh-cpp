rm rnd.bin
rm rnd.bin.arh
rm rnd.bin.arh.dearh
sudo ./scripts/rnd_bin.sh
md5sum rnd.bin
sudo ./a.out rnd.bin -arh
md5sum rnd.bin.arh
sudo ./a.out rnd.bin.arh -dearh
md5sum rnd.bin.arh.dearh