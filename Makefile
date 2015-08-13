all: debug

debug:
	cd scripts; ./build_debug.sh

release:
	cd scripts; ./build_release.sh

debugclean:
	$(RM) -r build/debug

releaseclean:
	$(RM) -r build/release 

docclean:
	$(RM) -r documentation/html documentation/index.html

installclean:
	$(RM) -r install

clean: debugclean installclean releaseclean docclean
	$(RM) -r build


doc: documentation/html

documentation/html:
	cd documentation; doxygen
	cd documentation; ln -sf html/index.html index.html
