FROM gcr.io/oss-fuzz-base/base-builder:v1
COPY . $SRC/DLXEmu
RUN $SRC/DLXEmu/scripts/ci/InstallDependencies.sh
WORKDIR DLXEmu
COPY .clusterfuzzlite/build.sh $SRC/
