#!/bin/bash

if [ -z $IMPERAS_HOME ] || [ ! -d $IMPERAS_HOME/bin/$IMPERAS_ARCH ]; then
  echo "IMPERAS_HOME not set of not valid. Please setup Imperas Environment"
else

    if [ -z $RISCV_PREFIX ]; then
        export RISCV_PREFIX=riscv-none-embed-
        echo "Setting RISCV_PREFIX to default ${RISCV_PREFIX} for xPack GNU RISC-V Embedded GCC"
    fi

    toolchain=$(${RISCV_PREFIX}gcc --version)
    declare -i rc=$?
    if [ $rc -ne 0 ]; then
        echo "Cannot find cross compiler $(RISCV_PREFIX)gcc, please ensure suitable cross compiler is installed and added to PATH"
        echo "The Cross Compiler used can be obtained from https://github.com/xpack-dev-tools/riscv-none-embed-gcc-xpack/releases"
        echo "To use this cross compiler RISCV_PREFIX=The Cross Compiler used can be obtained from https://github.com/xpack-dev-tools/riscv-none-embed-gcc-xpack/releases"
    else

        TARGET=$1
        shift
    
        if [ ! -z $TARGET ] && [ $TARGET != "clean" ] && [ $TARGET != "all" ]; then
            echo "Use no argument or 'clean' or 'all'"
        else
            pushd Debug
                make -f makefile $TARGET
                rc=$?
            popd
        fi
    fi
fi

exit $rc

