# Makefile for building vendored dependencies
# Run 'make' from this directory to build all vendored libraries

VENDOR_ROOT := $(shell pwd)
PREFIX := $(VENDOR_ROOT)/build
# Detect platform and use appropriate command to get CPU count
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    NPROC := $(shell sysctl -n hw.ncpu 2>/dev/null || echo 4)
else
    NPROC := $(shell nproc 2>/dev/null || echo 4)
endif

export PKG_CONFIG_PATH := $(PREFIX)/lib/pkgconfig:$(PKG_CONFIG_PATH)
export PATH := $(PREFIX)/bin:$(PATH)

.PHONY: all clean pcre2 glib readline ncurses extract

all: extract pcre2 ncurses readline glib
	@echo ""
	@echo "===================================="
	@echo "All vendored dependencies built!"
	@echo "Libraries: $(PREFIX)/lib"
	@echo "Headers: $(PREFIX)/include"
	@echo "===================================="

# Extract source tarballs if not already extracted
extract: pcre2-10.47/configure ncurses-6.5/configure readline-8.3/configure glib-2.86.1/meson.build

pcre2-10.47/configure: pcre2-10.47.tar.gz
	@echo "==> Extracting pcre2-10.47..."
	@tar -xzf pcre2-10.47.tar.gz
	@touch pcre2-10.47/configure

ncurses-6.5/configure: ncurses-6.5.tar.gz
	@echo "==> Extracting ncurses-6.5..."
	@tar -xzf ncurses-6.5.tar.gz
	@touch ncurses-6.5/configure

readline-8.3/configure: readline-8.3.tar.gz
	@echo "==> Extracting readline-8.3..."
	@tar -xzf readline-8.3.tar.gz
	@touch readline-8.3/configure

glib-2.86.1/meson.build: glib-2.86.1.tar.xz
	@echo "==> Extracting glib-2.86.1..."
	@tar -xJf glib-2.86.1.tar.xz
	@touch glib-2.86.1/meson.build

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
ifeq ($(UNAME_S),Darwin)
	@# macOS: Build include first, then ncurses to meet dependencies
	$(MAKE) -C ncurses-6.5/include -j$(NPROC) > /dev/null 2>&1
	$(MAKE) -C ncurses-6.5/ncurses -j$(NPROC) > /dev/null 2>&1
else
	@# Linux: Original build order works fine
	$(MAKE) -C ncurses-6.5/ncurses -j$(NPROC) > /dev/null
	$(MAKE) -C ncurses-6.5/include -j$(NPROC) > /dev/null
endif
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
	rm -rf pcre2-10.47/
	rm -rf readline-8.3/
	rm -rf ncurses-6.5/
	rm -rf glib-2.86.1/
