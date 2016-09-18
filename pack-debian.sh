#!/bin/bash
fakeroot dpkg-deb --build debian
mv debian.deb pocket-home_$1-$2_armhf.deb
