cmake_minimum_required(VERSION 3.10)

set(CMAKE_TOOLCHAIN_FILE Toolchain-arm-gcc.cmake)

# Allow assembler
enable_language(ASM)

project(STM_music)

set(INCLUDE_DIRS
	Core/Inc 
	Drivers/STM32F4xx_HAL_Driver/Inc 
	Drivers/STM32F4xx_HAL_Driver/Inc/Legacy 
	Drivers/CMSIS/Device/ST/STM32F4xx/Include 
	Drivers/CMSIS/Include 
)

set(C_SRCS
	Core/Src/gpio.c
	Core/Src/i2c.c
	Core/Src/i2s.c
	Core/Src/main.c
	Core/Src/stm32f4xx_hal_msp.c
	Core/Src/stm32f4xx_it.c
	Core/Src/syscalls.c
	Core/Src/sysmem.c
	Core/Src/system_stm32f4xx.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c_ex.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2s.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2s_ex.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c
	Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c
)

set(ASM_SRCS
	Core/Startup/startup_stm32f411vetx.s
)

set(GLOBAL_DEFINES
	#-DDEBUG 
	-DUSE_HAL_DRIVER 
	-DSTM32F411xE 
)

set(LINKED_LIBS
	# Without lib prefix and without .a suffix, we dont use it
)

link_directories(

)

include_directories(${INCLUDE_DIRS})
add_definitions(${GLOBAL_DEFINES})

include(compiler_flags.cmake)

add_arm_executable(${CMAKE_PROJECT_NAME} ${CPP_SRCS} ${C_SRCS} ${ASM_SRCS})

arm_link_libraries(${CMAKE_PROJECT_NAME} ${LINKED_LIBS})
