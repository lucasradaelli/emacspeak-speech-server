#
# RPM Package Manager (RPM) spec file for ibmtts-devel
#
Summary: IBM Text To Speech (TTS) Software Development Kit (SDK)
Name: ibmtts-devel
Version: 6.7
Release: 4.1
Prefix: /opt/IBM
License: BSD
Group: Application/Sound
URL: http://www-3.ibm.com/software/pervasive/products/voice/voice_technologies.shtml
Vendor: IBM Corp
Packager: George Kraft IV <gk4@austin.ibm.com>
AutoReqProv:    0
Provides:       IBM-TTS-SDK
Requires:       ibmtts_rte
ExclusiveArch:  i386
ExclusiveOS:    linux
%description
This is a development package for IBM Text To Speech (TTS).
It is intended to be used to build applications when a licensed 
ibmtts is not available.  Only the ECI ABIs are provided. There 
is no TTS runtime code provided.
%files
/opt/IBM/ibmtts-devel/lib/libibmeci.so
/opt/IBM/ibmtts-devel/include/eci.h
/opt/IBM/ibmtts-devel/share/pkgconfig/ibmtts.pc
/opt/IBM/ibmtts-devel/share/doc/NEWS
# Post-install stuff would go here.
#EOF
