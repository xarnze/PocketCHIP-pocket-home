export CONFIG:=Release

VERSION=0.0.8.9
BUILD=1

PKG_CONFIG:=$(shell which pkg-config)

PKG_CONFIG_PACKAGES = \
  NetworkManager \
  libnm-glib \
	alsa \

export PKG_CONFIG_CFLAGS=$(foreach pkg, $(PKG_CONFIG_PACKAGES), $(shell $(PKG_CONFIG) --cflags $(pkg)))
export PKG_CONFIG_LDFLAGS=$(foreach pkg, $(PKG_CONFIG_PACKAGES), $(shell $(PKG_CONFIG) --libs $(pkg)))



all:
	cd Builds/LinuxMakefile && $(MAKE)

clean:
	cd Builds/LinuxMakefile && $(MAKE) clean

wifitest:
	cd Builds/LinuxMakefile && $(MAKE) -f UnitTests.mk ../../build/$(CONFIG)/wifitest

pack: all
	cp build/Release/pocket-home debian/usr/bin/ && \
	./pack-debian.sh $(VERSION) $(BUILD)

install: pack
	sudo dpkg -i pocket-home_$(VERSION)-$(BUILD)_armhf.deb

devinstall:
	killall pocket-home ;\
	sudo cp build/$(CONFIG)/pocket-home /usr/bin/pocket-home && \
	sudo cp -R assets/* /usr/share/pocket-home
