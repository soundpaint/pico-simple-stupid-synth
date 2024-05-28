include common.mk
ROOT=.

all:
	cd images ; make all
	cd slides ; make all

objclean:
	rm -rf $(BUILD)

bkpclean:
	cd images ; make bkpclean
	cd slides ; make bkpclean
	rm -f *~

coreclean:
	rm -f core core.*

clean: objclean

distclean: objclean bkpclean coreclean

tarball: distclean
	@TGZ_DATE=`date +%Y-%m-%d_%H-%M-%S` ; \
	PROJECT_NAME=gpn22 ; \
	PROJECT_PATH=`basename \`pwd\`` ; \
	TGZ_PREFIX=$$PROJECT_NAME\_$$TGZ_DATE ; cd .. ; \
	tar cvf ./$$TGZ_PREFIX.tar.bz2 \
		--exclude=untracked_files \
		--exclude=images \
		--transform=s/$$PROJECT_PATH/$$TGZ_PREFIX/ \
		--bzip2 $$PROJECT_PATH

#  Local Variables:
#    coding:utf-8
#    mode:GNUmakefile
#  End:
