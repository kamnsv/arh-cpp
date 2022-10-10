rm rnd*
sudo ./scripts/rnd_bin.sh
md5sum rnd.bin
sudo ./a.out rnd.bin -arh
md5sum rnd.bin.arh
sudo ./a.out rnd.bin.arh -dearh
md5sum rnd.bin.arh.dearh