
Overview
=========

Core functionalities to be used by GUI libraries for real time plotting



The license that applies to the whole package content is **CeCILL**. Please look at the license.txt file at the root of this repository.

Installation and Usage
=======================

The detailed procedures for installing the rtplot-core package and for using its components is based on the [PID](http://pid.lirmm.net/pid-framework/pages/install.html) build and deployment system called PID. Just follow and read the links to understand how to install, use and call its API and/or applications.

For a quick installation:

## Installing the project into an existing PID workspace

To get last version :
 ```
cd <path to pid workspace>
pid deploy package=rtplot-core
```

To get a specific version of the package :
 ```
cd <path to pid workspace>
pid deploy package=rtplot-core version=<version number>
```

## Standalone install
 ```
git clone https://github.com/BenjaminNavarro/rtplot-core.git
cd rtplot-core
```

Then run the adequate install script depending on your system. For instance on linux:
```
sh share/install/standalone_install.sh
```

The pkg-config tool can be used to get all links and compilation flags for the libraries defined in the project.

To let pkg-config know these libraries, read the output of the install_script and apply the given command to configure the PKG_CONFIG_PATH.

For instance on linux do:
```
export PKG_CONFIG_PATH=<given path>:$PKG_CONFIG_PATH
```

Then, to get compilation flags run:

```
pkg-config --static --cflags rtplot-core_<name of library>
```

```
pkg-config --variable=c_standard rtplot-core_<name of library>
```

```
pkg-config --variable=cxx_standard rtplot-core_<name of library>
```

To get linker flags run:

```
pkg-config --static --libs rtplot-core_<name of library>
```


About authors
=====================

rtplot-core has been developped by following authors: 
+ Benjamin Navarro (LIRMM)

Please contact Benjamin Navarro (navarro@lirmm.fr) - LIRMM for more information or questions.



