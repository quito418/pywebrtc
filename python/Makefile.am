all-local:
	CFLAGS+="$(PYWRAPPER_EXTRA_CFLAGS)" \
	LDFLAGS+="$(PYWRAPPER_EXTRA_LDFLAGS)" \
	python3 setup.py build

check-local:

clean-local:
	python3 setup.py clean --all
	rm -rf ./pywebrtc/_ext

install-data-local:

install-exec-local:
	CFLAGS+="$(PYWRAPPER_EXTRA_CFLAGS)" \
	LDFLAGS+="$(PYWRAPPER_EXTRA_LDFLAGS)" \
	python3 setup.py install

uninstall-local:

