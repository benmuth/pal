# Makefile for building vendored dependencies
# Run 'make' from this directory to build all vendored libraries

VENDOR_ROOT := $(shell pwd)
PREFIX := $(VENDOR_ROOT)/build
NPROC := $(shell nproc 2>/dev/null || echo 4)

export PKG_CONFIG_PATH := $(PREFIX)/lib/pkgconfig:$(PKG_CONFIG_PATH)
export PATH := $(PREFIX)/bin:$(PATH)

.PHONY: all clean pcre2 glib readline ncurses

all: pcre2 ncurses readline glib
	@echo ""
	@echo "===================================="
	@echo "All vendored dependencies built!"
	@echo "Libraries: $(PREFIX)/lib"
	@echo "Headers: $(PREFIX)/include"
	@echo "===================================="

# PCRE2 (required by glib)
pcre2: $(PREFIX)/lib/libpcre2-8.a

$(PREFIX)/lib/libpcre2-8.a:
	@echo "==> Building pcre2-10.47..."
	cd pcre2-10.47 && \
	./configure \
		--prefix=$(PREFIX) \
		--enable-static \
		--disable-shared \
		--enable-pcre2-8 \
		--enable-pcre2-16 \
		--enable-pcre2-32 \
		--disable-dependency-tracking \
		--quiet
	$(MAKE) -C pcre2-10.47 -j$(NPROC) install > /dev/null
	@echo "✓ pcre2 installed"

# Ncurses
ncurses: $(PREFIX)/lib/libncursesw.a

$(PREFIX)/lib/libncursesw.a:
	@echo "==> Building ncurses-6.5..."
	cd ncurses-6.5 && \
	./configure \
		--prefix=$(PREFIX) \
		--enable-static \
		--disable-shared \
		--without-debug \
		--without-ada \
		--enable-widec \
		--with-pkg-config-libdir=$(PREFIX)/lib/pkgconfig \
		--without-tests \
		--without-progs \
		--quiet
	@# Build libraries only, don't install terminfo to avoid system interference
	$(MAKE) -C ncurses-6.5/ncurses -j$(NPROC) > /dev/null
	$(MAKE) -C ncurses-6.5/include -j$(NPROC) > /dev/null
	@# Install only libraries and headers, skip terminfo/data
	$(MAKE) -C ncurses-6.5/ncurses install.libs > /dev/null 2>&1
	$(MAKE) -C ncurses-6.5/include install.includes > /dev/null 2>&1
	@echo "✓ ncurses installed"

# Readline
readline: $(PREFIX)/lib/libreadline.a

$(PREFIX)/lib/libreadline.a:
	@echo "==> Building readline-8.3..."
	cd readline-8.3 && \
	./configure \
		--prefix=$(PREFIX) \
		--enable-static \
		--disable-shared \
		--quiet
	$(MAKE) -C readline-8.3 -j$(NPROC) install > /dev/null
	@echo "✓ readline installed"

# GLib (requires pcre2)
glib: pcre2 $(PREFIX)/lib/libglib-2.0.a

$(PREFIX)/lib/libglib-2.0.a:
	@echo "==> Building glib-2.86.1..."
	@if ! command -v meson >/dev/null 2>&1; then \
		echo "ERROR: meson not found. Install with: pip3 install meson ninja"; \
		exit 1; \
	fi
	cd glib-2.86.1 && \
	meson setup _build \
		--prefix=$(PREFIX) \
		--default-library=static \
		--buildtype=release \
		-Dtests=false \
		-Dintrospection=disabled \
		-Dnls=disabled \
		-Dsysprof=disabled > /dev/null 2>&1
	cd glib-2.86.1/_build && ninja install > /dev/null 2>&1
	@echo "✓ glib installed"

clean:
	rm -rf build/
	cd pcre2-10.47 && $(MAKE) clean 2>/dev/null || true
	cd readline-8.3 && $(MAKE) clean 2>/dev/null || true
	cd ncurses-6.5 && $(MAKE) clean 2>/dev/null || true
	rm -rf glib-2.86.1/_build
