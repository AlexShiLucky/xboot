@set ECLIPSE_DIR=C:\Eclipse\eclipse-for-xboot-windows-x86_64
@set TOOLCHAIN_DIR=C:\toolchain
@set GNU_ARM=%TOOLCHAIN_DIR%\gnu_arm
@set GNU_LINARO=%TOOLCHAIN_DIR%\gnu_linaro
@set PATH=%ECLIPSE_DIR%\compiler\bin;%GNU_ARM%\arm-none-eabi\bin;%GNU_ARM%\arm-none-linux-gnueabihf\bin;%GNU_ARM%\aarch64-none-elf\bin;%GNU_ARM%\aarch64-none-linux-gnu\bin;%GNU_LINARO%\arm-eabi\bin;%GNU_LINARO%\arm-linux-gnueabihf\bin;%GNU_LINARO%\aarch64-elf\bin;%GNU_LINARO%\aarch64-linux-gnu\bin;%TOOLCHAIN_DIR%\riscv-none-embed\bin;%PATH%;
@make clean
