#Install location
myprefix=$HOME/data/local

#Get most recent versions
wget http://ftp.gnu.org/gnu/m4/m4-1.4.18.tar.gz
wget http://ftp.gnu.org/gnu/autoconf/autoconf-2.69.tar.gz
wget http://ftp.gnu.org/gnu/automake/automake-1.16.2.tar.gz
wget http://ftp.gnu.org/gnu/libtool/libtool-2.4.6.tar.gz

# Decompress
gzip -dc m4-1.4.18.tar.gz | tar xvf -
gzip -dc autoconf-2.69.tar.gz | tar xvf -
gzip -dc automake-1.16.2.tar.gz | tar xvf -
gzip -dc libtool-2.4.6.tar.gz | tar xvf -

cd m4-1.4.18
./configure -C --prefix=$myprefix && make && make install
cd ../autoconf-2.69
./configure -C --prefix=$myprefix && make && make install
cd ../automake-1.16.1
./configure -C --prefix=$myprefix && make && make install
cd ../libtool-2.4.6
./configure -C --prefix=$myprefix && make && make install
