# Building SST-Core and SST-Elements on Hive

These instructions guide supplement the guide found [here](http://sst-simulator.org/SSTPages/SSTBuildAndInstall9dot1dot0SeriesDetailedBuildInstructions/). We will refer to their guide as the official installation guide. 

In this guide will be building from the master branch on Github instead of the tarballs, becauuse SST has recently removed some Python API dependency that breaks on Hive. 

Please read the official build instructions, and refer to this guide when you get to a new section of the official guide, so that you can get the supplemental information found here. The section headers here match the ones on their guide. 

## Example Build and Install Directories

On Hive, the home directory has a few different folders. After reading the documentation, I believe the best place for your to place any installed programs is in `$HOME/data`. This means you should install programs in `$HOME/data/local`. 


## Building A Basic SST System

OpenMPI is already installed on Hive. If it is not already loaded, you can use `module load openmpi` to do so. Sadly, the version on Hive is not 2.1.3 which is what SST asks for. I don't know yet if this will be an issue. 

## SST Core 9.1.x Build and Installation

As mentioned above, we will be building from the Github source instead of the tarball. This guide was written using SST-Core commit `c4008fc` SST-Elements commit `61d11a0`. 

Instead of Step 1 in this section, you should obtain the source as follows:

```
cd $HOME/data/scratch/src
git clone https://github.com/sstsimulator/sst-core
cd sst-core
```

At this point you'll need to run `autogen.sh` but Hive seems to be missing COPYING.LIB or something like that from autotools, so you'll first need your own copy of autotools. They all need to be installed together. The script below will download and install them for you in `$HOME/data/local`. If you run this in some temporary directory, you can then easily delete all the tarballs and source files after the script has finished. 

```
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

```
You should add `$HOME/data/local/bin` to the beginning of your `$PATH` and `$HOME/data/local/lib` to the beginning of your `$LD_LIBRARY_PATH`. 

In step 2, make sure to adjust your paths properly. 

```
export SST_CORE_HOME=$HOME/data/local/sst-core
export SST_CORE_ROOT=$HOME/scratch/src/sst-core
```

Now you are ready to run `./autogen.sh`. After it has run, you can use the regular installation guide to tell you how to run `./configure`. 

If you now run `make all` in Step 5, you'll find that it `ld` fails because it can't find `libintl`. This library is part of Gnu's `gettext` and is suspiciously missing from Hive, even though the header `libintl.h` exists in `/usr/local/include`. The fix for this is simply to remove the the linker commands `-lintl`. The following script will do that for you. Run it from the project root. 

```
#!/bin/bash

for f in `find . -name Makefile`;
do
    sed -i 's/-lintl//g' $f
done
```

If you know a better way to do this, please let me know. 

Now, complete Step 5, set the proper path in Step 6, and test in Step 7. 

# SST Elements 9.1.x Build and Installation
 
 These steps are very similar. First get the code.
 
```
cd $HOME/data/scratch/src
git clone https://github.com/sstsimulator/sst-elements
cd sst-elements
./autogen.sh
```

Before configuring, you will need to install Intel's Pin tool. Instructions for that can be found [here](http://sst-simulator.org/SSTPages/SSTBuildAndInstall9dot1dot0SeriesAdditionalExternalComponents/#intel-pin-tool-214-71313). 

Now you are to run `./configure` with all the proper arguments (including the `--with-pin` argument) as described in the official installation guide. 

After running `./configure` you will again need to remove `-lintl` from the Makefiles. Run the same script as before. 

And that's it! You're ready to run make. 
