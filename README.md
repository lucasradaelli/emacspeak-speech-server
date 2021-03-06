Emacspeak Speech Server
=======================

This is a C++ implementation of an Emacspeak speech server that connects to the
IBM ViaVoice Text-to-Speech library for speech synthesis.


Building
--------

Basic instructions to build the speech server on Ubuntu 14.04 LTS 64-bit:

  1. Install the necessary system libraries:

     $ sudo apt-get install cmake g++-multilib libasound2-dev:i386 \
            libboost-regex-dev:i386 libboost-program-options-dev:i386

  2. Configure the project:

     $ mkdir build/
     $ cd build/
     $ cmake ..

  3. Build the speech server:

     $ make

     The above command will produce a 'speech_server' executable inside the
     build/ directory.


Contributing
------------

Contributions are always welcome and highly encouraged.
See CONTRIBUTING for more information on how to get started.


License
-------

Apache 2.0 - See LICENSE for more information.
