textin
======

Installation von MBrola/eSpeak Paketen
i368/amd64:
- sudo apt-get install mbrola espeak

ARMhf:
- sudo apt-get install espeak
- Rasperry Pi Mbrola-Packet runterladen und entpacken
  http://www.tcts.fpms.ac.be/synthesis/mbrola/mbrcopybin.html
- sudo mv mbrola /usr/bin/mbrola

Installation von Sprachpaket
- sudo mkdir /usr/share/mbrola
- sudo mkdir /usr/share/mbrola/voices
- Sprachpacket (z.B. de6) runterladen und entpacken
  http://www.tcts.fpms.ac.be/synthesis/mbrola/mbrcopybin.html
- sudo mv de6/de6 /usr/share/mbrola/voices/de6

Espeak Test z.B. mit
espeak -v mb/mb-de6 -s 90 "lasso"
