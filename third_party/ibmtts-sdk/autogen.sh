#!/bin/sh

aclocal \
&& libtoolize --force --copy \
&& autoheader \
&& automake -ac --add-missing --copy --gnu \
&& autoconf

#EOF
