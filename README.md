# PocketCHIP Launcher (Marshmallow Edition)

## Required Packages

```sh
sudo apt-get install \
    git \
    build-essential \
    libasound2-dev \
    libx11-dev \
    libxrandr-dev \
    libxcursor-dev \
    libxft-dev \
    libxinerama-dev \
    libnm-glib-dev \
    network-manager-dev \
    libi2c-dev \
    libssl-dev \
    libnm-gtk-dev
```

## Building
```
git clone --recursive https://github.com/o-marshmallow/PocketCHIP-pocket-home/
make
make devinstall
sudo systemctl restart lightdm
```

## Updating
```
git pull
git submodule update
make
make devinstall
sudo systemctl restart lightdm
```

### Thanks
Big thanks to [Celti](https://github.com/Celti) who made this repo updated to new JUCE version and easier to compile
