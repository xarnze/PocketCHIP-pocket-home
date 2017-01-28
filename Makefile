.PHONY: all archive

ROOT=archive
SECTION=main
ARCHES=armhf
RELEASES=jessie


#all: prep-jessie archive release-jessie
all: $(addprefix prep-,$(RELEASES)) archive $(addprefix release-,$(RELEASES))

prep-%:
	echo "prep for $*"
	test -e apt-release-$*.conf # Bail unless we have a conf for this arch
	mkdir -p archive/dists/$*/main/binary-$(ARCHES)

archive:
	mkdir -p $(ROOT)/pool/$(SECTION)
	cp pocket-home_*.deb $(ROOT)/pool/$(SECTION)
	rm -f /tmp/packages*.db # Cache causes issues when rebuilding
	apt-ftparchive generate apt-ftparchive.conf

release-%:
	apt-ftparchive -c apt-release-$*.conf release archive/dists/$* > archive/dists/$*/Release
	gpg --yes --output archive/dists/$*/Release.gpg -ba archive/dists/$*/Release
