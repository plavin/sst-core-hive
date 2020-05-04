# Building SST-Core and SST-Elements on Hive

These instructions guide supplement the guide found [here](http://sst-simulator.org/SSTPages/SSTBuildAndInstall9dot1dot0SeriesDetailedBuildInstructions/). We will refer to their guide as the official installation guide. 

In this guide will be building from the master branch on Github instead of the tarballs, becauuse SST has recently removed some Python API dependency that breaks on Hive. 

Please read the official build instructions, and refer to this guide when you get to a new section of the official guide, so that you can get the supplemental information found here. The section headers here match the ones on their guide. 

### Before We Start: Pace Modules
We will be building/downloading a lot of our own depenndencies (including Pintool, Autotools, and OpenMPI) so we do not need many modules loaded. You should be able to complete this with only the following modules:
```
Currently Loaded Modules:
  1) pace/2019.08   2) python/3.7.4
```

### Note on Compiler Versions
As I mention throughout this guide, I tried to use `gcc 4.8.5` where I could, but accidentally compiled SST Elements with `8.3.0`. I suggest that users be more careful and make a consistent choice of gcc version, although my version seems to be working fine. 

## Example Build and Install Directories

On Hive, the home directory has a few different folders. After reading the documentation, I believe the best place for you to place any installed programs is in `$HOME/data`. This means you should install programs in `$HOME/data/local`. 


## Building A Basic SST System

While there is a version of OpenMPI installed on Hive, it is compiled with `icc`, which does't play well with SST, according to [this](https://github.com/sstsimulator/sst-core/issues/428) issue. You should follow the instructions in the SST installation guide to install your own copy of OpenMPI 2.1.3. You should make sure that you don't have the `intel` module loaded so that OpenMPI gets built with gcc. I built it with `gcc 4.8.5`, which is will be provided when you have no compile modules loaded. 

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
#!/usr/bin/env bash
set -euo pipefail

#Install location
PREFIX=$HOME/data/local

# Versions to grab
M4_VERSION=1.4.18
AUTOCONF_VERSION=2.69
AUTOMAKE_VERSION=1.16.2
LIBTOOL_VERSION=2.4.6

#Get most recent versions
wget http://ftp.gnu.org/gnu/m4/m4-${M4_VERSION}.tar.gz
wget http://ftp.gnu.org/gnu/autoconf/autoconf-${AUTOCONF_VERSION}.tar.gz
wget http://ftp.gnu.org/gnu/automake/automake-${AUTOMAKE_VERSION}.tar.gz
wget http://ftp.gnu.org/gnu/libtool/libtool-${LIBTOOL_VERSION}.tar.gz

# Decompress
gzip -dc m4-${M4_VERSION}.tar.gz | tar xvf -
gzip -dc autoconf-${AUTOCONF_VERSION}.tar.gz | tar xvf -
gzip -dc automake-${AUTOMAKE_VERSION}.tar.gz | tar xvf -
gzip -dc libtool-${LIBTOOL_VERSION}.tar.gz | tar xvf -

cd m4-${M4_VERSION}
./configure -C --prefix=$PREFIX && make && make install
cd ../autoconf-${AUTOCONF_VERSION}
./configure -C --prefix=$PREFIX && make && make install
cd ../automake-${AUTOMAKE_VERSION}
./configure -C --prefix=$PREFIX && make && make install
cd ../libtool-${LIBTOOL_VERSION}
./configure -C --prefix=$PREFIX && make && make install

```
You should add `$HOME/data/local/bin` to the beginning of your `$PATH` and `$HOME/data/local/lib` to the beginning of your `$LD_LIBRARY_PATH`. 

In step 2, make sure to adjust your paths properly. 

```
export SST_CORE_HOME=$HOME/data/local/sst-core
export SST_CORE_ROOT=$HOME/scratch/src/sst-core
```

Now you are ready to run `./autogen.sh`. After it has run, you can use the regular installation guide to tell you how to run `./configure`. I used `gcc 4.8.5` to build `sst-core`. 

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

## SST Elements 9.1.x Build and Installation
 
 These steps are very similar. First get the code.
 
```
cd $HOME/data/scratch/src
git clone https://github.com/sstsimulator/sst-elements
cd sst-elements
./autogen.sh
```

Before configuring, you will need to install Intel's Pin tool. Instructions for that can be found [here](http://sst-simulator.org/SSTPages/SSTBuildAndInstall9dot1dot0SeriesAdditionalExternalComponents/#intel-pin-tool-214-71313). 

Now you are to run `./configure` with all the proper arguments (including the `--with-pin` argument) as described in the official installation guide. I accidentally compiled with `gcc 8.3.0` (which is available as a module on Hive) instead of `gcc 4.8.5` like I did my other modules. I have not run into issues with this yet. 

After running `./configure` you will again need to remove `-lintl` from the Makefiles. Run the same script as before. 

And that's it! You're ready to run make. 

## Epilogue: Paths
You should have set all of the following paths while completing the guide. I adjusted these to build in `$HOME/data/local`, and removed the version numbers from the sst directories as they do not appear when building from the Github source. 
```
# MPI
export MPIHOME=$HOME/data/local/packages/OpenMPI-2.1.3
export PATH=$MPIHOME/bin:$PATH
export MPICC=mpicc
export MPICXX=mpicxx
export LD_LIBRARY_PATH=$MPIHOME/lib:$LD_LIBRARY_PATH

# AUTOTOOLS
export PATH=$HOME/data/local/bin:$PATH

# PIN
export PIN_HOME=$HOME/data/local/packages/pin-2.14-71313-gcc.4.4.7-linux
export INTEL_PIN_DIRECTORY=$PIN_HOME

# SST-CORE
export SST_CORE_HOME=$HOME/data/local/sst-core
export SST_CORE_ROOT=$HOME/scratch/src/sst-core
export PATH=$SST_CORE_HOME/bin:$PATH

# SST-ELEMENTS
export SST_ELEMENTS_HOME=$HOME/local/sst-elements
export SST_ELEMENTS_ROOT=$HOME/scratch/src/sst-elements
export PATH=$SST_ELEMENTS_HOME/bin:$PATH
```

## Testing
Here is the test command, with our paths in mind. 
```
sst $SST_ELEMENTS_ROOT/src/sst/elements/simpleElementExample/tests/test_simpleRNGComponent_mersenne.py
```

## Coming Soon: Example PBS Script 
