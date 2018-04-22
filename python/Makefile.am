all-local:
	CFLAGS+="$(PYWRAPPER_EXTRA_CFLAGS) $(LIBWEBRTC_CFLAGS)" \
	LDFLAGS+="$(PYWRAPPER_EXTRA_LDFLAGS) $(LIBWEBRTC_LIBS)" \
	python3 setup.py build

check-local:

clean-local:
	python3 setup.py clean --all
	rm -rf ./build ./pywebrtc/_ext ./pywebrtc.egg-info

install-data-local:

install-exec-local:
	CFLAGS+="$(PYWRAPPER_EXTRA_CFLAGS) $(LIBWEBRTC_CFLAGS)" \
	LDFLAGS+="$(PYWRAPPER_EXTRA_LDFLAGS) $(LIBWEBRTC_LIBS)" \
	python3 setup.py install

uninstall-local:

