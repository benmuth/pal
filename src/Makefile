NAME    = pal

include Makefile.defs

INCLDIR = -I${prefix}/include `pkg-config --cflags glib-2.0`
LIBDIR  =
LIBS    = `pkg-config --libs glib-2.0` -lreadline -lncurses

ifeq ($(UNITY),1)
      SRC = pal_unity.c
else
      SRC = main.c colorize.c output.c input.c event.c rl.c html.c \
            add.c edit.c del.c remind.c search.c manage.c
endif
OBJ = $(SRC:.c=.o)


$(NAME): $(OBJ)
	@echo " [gcc] $(NAME)"
	@$(CC) $(CFLAGS) $(OBJ) $(LDFLAGS) -o $(NAME)
ifneq ($(DEBUG),1)
	@echo " [strip] $(NAME)"
	@strip $(NAME)
endif

%.o: %.c Makefile Makefile.defs
	@echo " [${CC}] $*.c"
	@$(CC) $(CFLAGS) -c $< $(OUTPUT_OPTION)

debug: clean
	@$(MAKE) DEBUG=1

# Use "install-no-rm" instead of "install" if you don't want to check
# for old files that should be removed before installing the new
# version.  This is useful for automated package managers that
# automatically remove files when a program is uninstalled (ie
# portage, rpm).
install-no-rm: install-man install-doc install-bin install-share

# Install binary file
install-bin:
	@echo " --- Installing binary --- "
	@mkdir -p ${DESTDIR}${prefix}/bin;
	install -m 0755 pal ${DESTDIR}${prefix}/bin;
	install -m 0755 convert/vcard2pal ${DESTDIR}${prefix}/bin;
	@echo

install-share:
	@echo " --- Installing global data --- "
	@mkdir -p ${DESTDIR}${prefix}/share/pal;
	install -m 0644 ../share/*.pal ${DESTDIR}${prefix}/share/pal
	install -m 0644 ../pal.conf ${DESTDIR}${prefix}/share/pal/
	@echo

# "install" will install the files needed by pal and also check to see
# if there are old pal files that should be removed.  If you don't
# want the install process to attempt to remove old files, use
# "install-no-rm".
install: install-no-rm
	@echo " --- Installation complete --- "
	@echo

# Install man page
install-man:
	@echo " --- Installing man page --- "
	cd ../; sed 's/PAL_VERSION/$(PAL_VERSION)/' pal.1.template | gzip -9 > pal.1.gz
	@mkdir -p ${DESTDIR}${prefix}/share/man/man1/
	install -m 0644 ../pal.1.gz ${DESTDIR}${prefix}/share/man/man1/
	install -m 0644 convert/vcard2pal.1 ${DESTDIR}${prefix}/share/man/man1/
	rm ../pal.1.gz
	@echo

# Install documentation
install-doc:
	@echo " --- Installing docs --- "
	@mkdir -p ${DESTDIR}${prefix}/share/doc/pal-$(PAL_VERSION);
	cat ../COPYING | gzip -9 > ../COPYING.gz
	install -m 0644 ../COPYING.gz ${DESTDIR}${prefix}/share/doc/pal-$(PAL_VERSION);
	rm ../COPYING.gz
	cat ../INSTALL | gzip -9 > ../INSTALL.gz
	install -m 0644 ../INSTALL.gz ${DESTDIR}${prefix}/share/doc/pal-$(PAL_VERSION);
	rm ../INSTALL.gz
	cat ../ChangeLog | gzip -9 > ../ChangeLog.gz
	install -m 0644 ../ChangeLog.gz ${DESTDIR}${prefix}/share/doc/pal-$(PAL_VERSION);
	rm ../ChangeLog.gz
	install -m 0644 ../doc/example.css ${DESTDIR}${prefix}/share/doc/pal-$(PAL_VERSION);
	@echo

# try to uninstall pal
uninstall: uninstall-man
	@echo " --- Removing binary --- "
	rm -f ${prefix}/bin/pal;
	@echo
	@echo " --- Removing global data --- "
	rm -rf ${prefix}/share/pal;
	@echo
	@echo " --- Removing doc directory --- "
	rm -rf ${prefix}/share/doc/pal-$(PAL_VERSION);
	@echo
	@echo " --- Removing /etc/pal.conf --- "
	rm -f /etc/pal.conf;
	@echo
	@echo " --- Check for doc directories for other versions of pal --- "
	@echo " NOTE: If you don't have other copies of pal installed, you can"
	@echo " safely delete the directories listed below:"
	@find ${prefix}/share/doc -name pal-[0-9].[0-9].[0-9]\* -type d -maxdepth 1;

# uninstall man page
uninstall-man:
	@echo " --- Removing man page --- "
	rm -f ${prefix}/share/man/man1/pal.1.gz
	@echo

# Generate compile_commands.json for LSP (clangd)
compile_commands.json:
	@echo "Generating compile_commands.json for LSP..."
	@./generate_compile_commands.sh > compile_commands.json

# Remove binary, object files and emacs backup files
clean:
	rm -rf $(NAME) libpal.a *.o *~


cleandep: cleandeps
distclean: cleandeps
clobber: cleandeps

splint:
	splint $(INCLDIR) $(SRC)

# this rule creates a tgz with all of the needed source files for
# distribution
dist:
	@echo
	rm -rf ../pal-$(PAL_VERSION)
	mkdir ../pal-$(PAL_VERSION)
	mkdir ../pal-$(PAL_VERSION)/src
	mkdir ../pal-$(PAL_VERSION)/src/convert
	mkdir ../pal-$(PAL_VERSION)/share
	mkdir ../pal-$(PAL_VERSION)/doc
	cd ../; \
	cp Makefile ChangeLog INSTALL COPYING pal.spec.template pal.1.template pal.conf pal-$(PAL_VERSION); \
	sed 's/Version:.*/Version: $(PAL_VERSION)/' pal.spec.template > pal-$(PAL_VERSION)/pal-$(PAL_VERSION).spec; \
	cp src/*.[ch] src/Makefile src/Makefile.defs pal-$(PAL_VERSION)/src; \
	cp src/convert/*.[ch] src/convert/vcard2pal{,.1} src/convert/Makefile pal-$(PAL_VERSION)/src/convert; \
	cp doc/example.css pal-$(PAL_VERSION)/doc; \
	cp share/*.pal pal-$(PAL_VERSION)/share; \
	@echo
	@echo " --- CREATING tgz file with the files listed below ---"
	cd ../; tar -czvf pal-$(PAL_VERSION).tgz pal-$(PAL_VERSION)
	@echo " --- DONE! (created pal-$(PAL_VERSION).tgz in parent directory) --- "
	rm -rf ../pal-$(PAL_VERSION)

srpm: dist
	rpmbuild -ts ../pal-$(PAL_VERSION).tgz

libpal.a: $(OBJ)
	rm -f libpal.a
	ar rcv libpal.a $(OBJ)
	strip --strip-symbol=main libpal.a
	ar s libpal.a
	ranlib libpal.a

