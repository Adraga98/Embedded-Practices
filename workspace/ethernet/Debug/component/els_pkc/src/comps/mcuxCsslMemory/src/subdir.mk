################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../component/els_pkc/src/comps/mcuxCsslMemory/src/mcuxCsslMemory_Clear.c \
../component/els_pkc/src/comps/mcuxCsslMemory/src/mcuxCsslMemory_Compare.c \
../component/els_pkc/src/comps/mcuxCsslMemory/src/mcuxCsslMemory_Copy.c \
../component/els_pkc/src/comps/mcuxCsslMemory/src/mcuxCsslMemory_Set.c 

C_DEPS += \
./component/els_pkc/src/comps/mcuxCsslMemory/src/mcuxCsslMemory_Clear.d \
./component/els_pkc/src/comps/mcuxCsslMemory/src/mcuxCsslMemory_Compare.d \
./component/els_pkc/src/comps/mcuxCsslMemory/src/mcuxCsslMemory_Copy.d \
./component/els_pkc/src/comps/mcuxCsslMemory/src/mcuxCsslMemory_Set.d 

OBJS += \
./component/els_pkc/src/comps/mcuxCsslMemory/src/mcuxCsslMemory_Clear.o \
./component/els_pkc/src/comps/mcuxCsslMemory/src/mcuxCsslMemory_Compare.o \
./component/els_pkc/src/comps/mcuxCsslMemory/src/mcuxCsslMemory_Copy.o \
./component/els_pkc/src/comps/mcuxCsslMemory/src/mcuxCsslMemory_Set.o 


# Each subdirectory must supply rules for building sources it contributes
component/els_pkc/src/comps/mcuxCsslMemory/src/%.o: ../component/els_pkc/src/comps/mcuxCsslMemory/src/%.c component/els_pkc/src/comps/mcuxCsslMemory/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_RW612ETA2I -DCPU_RW612ETA2I_cm33_nodsp -DMCUXPRESSO_SDK -DFSL_SDK_DRIVER_QUICK_ACCESS_ENABLE=1 -DPRINTF_ADVANCED_ENABLE=1 -DFSL_FEATURE_PHYKSZ8081_USE_RMII50M_MODE -DSDK_DEBUGCONSOLE=1 -DMCUX_META_BUILD -DBOOT_HEADER_ENABLE=1 -DWIFI_BOARD_RW610 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"C:\repos\embedded_practices\workspace\ethernet\source" -I"C:\repos\embedded_practices\workspace\ethernet\flash_config" -I"C:\repos\embedded_practices\workspace\ethernet\drivers" -I"C:\repos\embedded_practices\workspace\ethernet\CMSIS" -I"C:\repos\embedded_practices\workspace\ethernet\CMSIS\m-profile" -I"C:\repos\embedded_practices\workspace\ethernet\device" -I"C:\repos\embedded_practices\workspace\ethernet\device\periph" -I"C:\repos\embedded_practices\workspace\ethernet\utilities" -I"C:\repos\embedded_practices\workspace\ethernet\utilities\str" -I"C:\repos\embedded_practices\workspace\ethernet\utilities\debug_console_lite" -I"C:\repos\embedded_practices\workspace\ethernet\component\uart" -I"C:\repos\embedded_practices\workspace\ethernet\component\els_pkc" -I"C:\repos\embedded_practices\workspace\ethernet\component\els_pkc\src\comps\mcuxClBuffer\inc" -I"C:\repos\embedded_practices\workspace\ethernet\component\els_pkc\src\comps\mcuxClBuffer\inc\internal" -I"C:\repos\embedded_practices\workspace\ethernet\component\els_pkc\src\comps\mcuxClCore\inc" -I"C:\repos\embedded_practices\workspace\ethernet\component\els_pkc\src\comps\mcuxClEls\inc" -I"C:\repos\embedded_practices\workspace\ethernet\component\els_pkc\src\comps\mcuxClEls\inc\internal" -I"C:\repos\embedded_practices\workspace\ethernet\component\els_pkc\src\comps\mcuxClMemory\inc" -I"C:\repos\embedded_practices\workspace\ethernet\component\els_pkc\src\comps\mcuxClMemory\inc\internal" -I"C:\repos\embedded_practices\workspace\ethernet\component\els_pkc\src\comps\mcuxCsslMemory\inc" -I"C:\repos\embedded_practices\workspace\ethernet\component\els_pkc\src\comps\mcuxCsslMemory\inc\internal" -I"C:\repos\embedded_practices\workspace\ethernet\component\els_pkc\src\comps\mcuxCsslCPreProcessor\inc" -I"C:\repos\embedded_practices\workspace\ethernet\component\els_pkc\src\comps\mcuxCsslDataIntegrity\inc" -I"C:\repos\embedded_practices\workspace\ethernet\component\els_pkc\src\comps\mcuxCsslFlowProtection\inc" -I"C:\repos\embedded_practices\workspace\ethernet\component\els_pkc\src\comps\mcuxCsslParamIntegrity\inc" -I"C:\repos\embedded_practices\workspace\ethernet\component\els_pkc\src\comps\mcuxCsslSecureCounter\inc" -I"C:\repos\embedded_practices\workspace\ethernet\component\els_pkc\src\compiler" -I"C:\repos\embedded_practices\workspace\ethernet\component\els_pkc\src\platforms\rw61x" -I"C:\repos\embedded_practices\workspace\ethernet\component\els_pkc\src\platforms\rw61x\inc" -I"C:\repos\embedded_practices\workspace\ethernet\component\els_pkc\includes\platform\rw61x" -I"C:\repos\embedded_practices\workspace\ethernet\component\phy" -I"C:\repos\embedded_practices\workspace\ethernet\component\silicon_id" -I"C:\repos\embedded_practices\workspace\ethernet\component" -I"C:\repos\embedded_practices\workspace\ethernet\board" -O0 -fno-common -g3 -gdwarf-4 -c -ffunction-sections -fdata-sections -fno-builtin -mcpu=cortex-m33+nodsp -imacros "C:\repos\embedded_practices\workspace\ethernet\source\mcux_config.h" -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m33+nodsp -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-component-2f-els_pkc-2f-src-2f-comps-2f-mcuxCsslMemory-2f-src

clean-component-2f-els_pkc-2f-src-2f-comps-2f-mcuxCsslMemory-2f-src:
	-$(RM) ./component/els_pkc/src/comps/mcuxCsslMemory/src/mcuxCsslMemory_Clear.d ./component/els_pkc/src/comps/mcuxCsslMemory/src/mcuxCsslMemory_Clear.o ./component/els_pkc/src/comps/mcuxCsslMemory/src/mcuxCsslMemory_Compare.d ./component/els_pkc/src/comps/mcuxCsslMemory/src/mcuxCsslMemory_Compare.o ./component/els_pkc/src/comps/mcuxCsslMemory/src/mcuxCsslMemory_Copy.d ./component/els_pkc/src/comps/mcuxCsslMemory/src/mcuxCsslMemory_Copy.o ./component/els_pkc/src/comps/mcuxCsslMemory/src/mcuxCsslMemory_Set.d ./component/els_pkc/src/comps/mcuxCsslMemory/src/mcuxCsslMemory_Set.o

.PHONY: clean-component-2f-els_pkc-2f-src-2f-comps-2f-mcuxCsslMemory-2f-src

