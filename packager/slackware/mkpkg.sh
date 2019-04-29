#!/bin/sh

# Slackware build script for white_dune

# Redistribution and use of this script, with or without modification, is
# permitted provided that the following conditions are met:
#
# 1. Redistributions of this script must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
# EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

set -e

VERSION=`sh ../../batch/getversion.sh`
cd /tmp
rm -f wdune-$VERSION.tar.bz2
wget ftp://ftp.ourproject.org/pub/wdune/wdune-$VERSION.tar.bz2
MD5SUM=`md5sum wdune-$VERSION.tar.bz2 | awk '{print $1}'` 
cd -

cat > white_dune/white_dune.info << EOT
PRGNAM="white_dune"
VERSION="$VERSION"
HOMEPAGE="https://wdune.ourproject.org/"
DOWNLOAD="ftp://ftp.ourproject.org/pub/wdune/wdune-$VERSION.tar.bz2"
MD5SUM="$MD5SUM"
DOWNLOAD_x86_64=""
MD5SUM_x86_64=""
REQUIRES="CGAL vcglib ffmpeg"
MAINTAINER="J. Scheurich"
EMAIL="mufti11@web.de"
EOT

cat > white_dune/white_dune.SlackBuild << EOT
#!/bin/sh

# Slackware build script for white_dune

# Copyright 2019 J. "MUFTI" Scheurich Stuttgart/Germany
# All rights reserved.
#
# Redistribution and use of this script, with or without modification, is
# permitted provided that the following conditions are met:
#
# 1. Redistributions of this script must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#
#  THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
#  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
#  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO
#  EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
#  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

PRGNAM=white_dune
SRCNAM=wdune
VERSION=\${VERSION:-$VERSION}
BUILD=\${BUILD:-1}
TAG=\${TAG:-_SBo}

if [ -z "\$ARCH" ]; then
  case "\$( uname -m )" in
    i?86) ARCH=i586 ;;
    arm*) ARCH=arm ;;
       *) ARCH=\$( uname -m ) ;;
  esac
fi

CWD=\$(pwd)
TMP=\${TMP:-/tmp/SBo}
PKG=\$TMP/package-\$PRGNAM
OUTPUT=\${OUTPUT:-/tmp}

if [ "\$ARCH" = "i586" ]; then
  SLKCFLAGS="-O2 -march=i586 -mtune=i686"
  LIBDIRSUFFIX=""
elif [ "\$ARCH" = "i686" ]; then
  SLKCFLAGS="-O2 -march=i686 -mtune=i686"
  LIBDIRSUFFIX=""
elif [ "\$ARCH" = "x86_64" ]; then
  SLKCFLAGS="-O2 -fPIC"
  LIBDIRSUFFIX="64"
else
  SLKCFLAGS="-O2"
  LIBDIRSUFFIX=""
fi

set -e

rm -rf \$PKG
mkdir -p \$TMP \$PKG \$OUTPUT
cd \$TMP
rm -rf \$SRCNAM-\$VERSION
tar xvf \$CWD/\$SRCNAM-\$VERSION.tar.bz2
cd \$SRCNAM-\$VERSION
chown -R root:root .
find -L . \\
 \( -perm 777 -o -perm 775 -o -perm 750 -o -perm 711 -o -perm 555 \\
  -o -perm 511 \) -exec chmod 755 {} \; -o \\
 \( -perm 666 -o -perm 664 -o -perm 640 -o -perm 600 -o -perm 444 \\
  -o -perm 440 -o -perm 400 \) -exec chmod 644 {} \;

export CFLAGS=\$SLKCFLAGS
export CXXFLAGS=\$SLKCFLAGS
./configure \\
 --with-wwwbrowser=firefox \\
 --with-helpurl="/usr/doc/\$PRGNAM-\$VERSION/docs" \\
 --with-protobaseurl="/usr/doc/\$PRGNAM-\$VERSION/docs"
make

install -c -d \$PKG/install \$PKG/usr/bin \$PKG/usr/man/man1
cp bin/* /\$PKG/usr/bin
install -c -m 644 ./man/dune.1 /\$PKG/usr/man/man1
install -c -d \$PKG/usr/doc/\$PRGNAM-\$VERSION
cp -fr docs \$PKG/usr/doc/\$PRGNAM-\$VERSION
install -D -m 0644 desktop/kde/dune.desktop \\
        \$PKG/usr/share/applications/dune.desktop
install -D -m 0644 desktop/kde/dune.png \\
        \$PKG/usr/share/pixmaps/dune.png

find \$PKG/usr/man -type f -exec gzip -9 {} \;
for i in \$( find \$PKG/usr/man -type l ) ; do ln -s \$( readlink \$i ).gz \$i.gz ; rm \$i ; done

find \$PKG -print0 | xargs -0 file | grep -e "executable" -e "shared object" | grep ELF \\
  | cut -f 1 -d : | xargs strip --strip-unneeded 2> /dev/null || true

mkdir -p \$PKG/install
cat \$CWD/slack-desc > \$PKG/install/slack-desc

cd \$PKG
/sbin/makepkg -l y -c n \$OUTPUT/\$PRGNAM-\$VERSION-\$ARCH-\$BUILD\$TAG.\${PKGTYPE:-tgz}

EOT
chmod +x white_dune/white_dune.SlackBuild

(
cd white_dune
cp /tmp/wdune-$VERSION.tar.bz2 .
sh white_dune.SlackBuild
)
tar -cvf /tmp/white_dune.tar white_dune
gzip -9f /tmp/white_dune.tar
