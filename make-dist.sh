#/bin/bash
TGZ_DATE=`date +%Y-%m-%d_%H-%M-%S`
PROJECT_NAME=simple-stupid-synth
PROJECT_PATH=`basename \`pwd\``
TGZ_PREFIX=$PROJECT_NAME\_$TGZ_DATE
cd ..
tar cvf ./$TGZ_PREFIX.tar.bz2 \
  --transform=s/$PROJECT_PATH/$TGZ_PREFIX/ \
  --bzip \
  --exclude="*~" \
  $PROJECT_PATH/{CMakeLists.txt,*.sh,*.cmake,src/*.{c,h,cpp,hpp}}
