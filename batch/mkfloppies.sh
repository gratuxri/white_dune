#!/bin/sh

# This file is free software; the Free Software Foundation    
# gives unlimited permission to copy, distribute and modify it.

DUNE=`dirname $0`/..

DUNENAME=white_dune-`sh $DUNE/batch/getversion.sh` 
export DUNENAME

MKTARGZTMP=/tmp/mktargz_$$
export MKTARGZTMP

mkdir $MKTARGZTMP &&
(cd .. && tar -cf - $DUNENAME) | (cd $MKTARGZTMP && tar -xf -) &&
(cd $MKTARGZTMP/white_dune-`sh batch/getversion.sh` && make -s realclean) &&
cp $MKTARGZTMP/$DUNENAME/src/config.h.nt $MKTARGZTMP/$DUNENAME/src/config.h &&
(cd $MKTARGZTMP && tar -cf /tmp/dune_floppy1.tar $DUNENAME/[C-c]* $DUNENAME/src) &&
gzip -9f /tmp/dune_floppy1.tar &&
ls -l /tmp/dune_floppy1.tar.gz | awk '{ if ($5 > 1440000) exit(1) }' || echo /tmp/dune_floppy1.tar.gz too long 1>&2 &&
mv /tmp/dune_floppy1.tar.gz /tmp/dune_fd1.tgz &&
(cd $MKTARGZTMP && tar -cf /tmp/dune_floppy2.tar $DUNENAME/[C-c]* $DUNENAME/src) &&
gzip -9f /tmp/dune_floppy2.tar &&
ls -l /tmp/dune_floppy2.tar.gz | awk '{ if ($5 > 1440000) exit(1) }' || echo /tmp/dune_floppy2.tar.gz too long 2>&2 &&
mv /tmp/dune_floppy2.tar.gz /tmp/dune_fd2.tgz &&
rm -rf $MKTARGZTMP &&
echo written /tmp/dune_fd1.tgz /tmp/dune_fd2.tgz

