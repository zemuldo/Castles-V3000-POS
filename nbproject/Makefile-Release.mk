#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=arm-brcm-linux-gnueabi-gcc
CCC=arm-brcm-linux-gnueabi-g++
CXX=arm-brcm-linux-gnueabi-g++
FC=g77.exe
AS=as

# Macros
CND_PLATFORM=Gnueabi-Windows
CND_DLIB_EXT=dll
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include NbMakefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/accoutopening.o \
	${OBJECTDIR}/fileman.o \
	${OBJECTDIR}/curlpost.o \
	${OBJECTDIR}/powerautomode.o \
	${OBJECTDIR}/withdraw.o \
	${OBJECTDIR}/balance.o \
	${OBJECTDIR}/paywith.o \
	${OBJECTDIR}/cJSON.o \
	${OBJECTDIR}/cardlessdeposit.o \
	${OBJECTDIR}/settings.o \
	${OBJECTDIR}/appmain.o \
	${OBJECTDIR}/deposit.o \
	${OBJECTDIR}/wub_lib.o \
	${OBJECTDIR}/filemanager.o \
	${OBJECTDIR}/gsm.o \
	${OBJECTDIR}/setpin.o \
	${OBJECTDIR}/systemops.o \
	${OBJECTDIR}/ministatement.o \
	${OBJECTDIR}/utilitypay.o \
	${OBJECTDIR}/agentballance.o \
	${OBJECTDIR}/ussd.o \
	${OBJECTDIR}/moremenu.o \
	${OBJECTDIR}/debug2.o \
	${OBJECTDIR}/Setting.o \
	${OBJECTDIR}/jsonbulder.o \
	${OBJECTDIR}/Transaction.o \
	${OBJECTDIR}/mainmenu.o


# C Compiler Flags
CFLAGS="-I${SDKV3INC}" -fsigned-char -Wundef -Wstrict-prototypes -Wno-trigraphs -Wimplicit -Wformat 

# CC Compiler Flags
CCFLAGS="-I${SDKV3INC}" -fsigned-char -Wundef -Wno-trigraphs -Wimplicit -Wformat 
CXXFLAGS="-I${SDKV3INC}" -fsigned-char -Wundef -Wno-trigraphs -Wimplicit -Wformat 

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-lcaethernet -lcafont -lcafs -lcakms -lcalcd -lcamodem -lcapmodem -lcaprt -lcartc -lcauart -lcauldpm -lcausbh -lcagsm -lcabarcode -lpthread -ldl -lcaclvw -lcatls -lctosapi -lcrypto -lcurl -lssl -lz -lfreetype -lcaclentry -lcaclmdl

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk dist/V3/App/FamDemoV1.0.0.exe

dist/V3/App/FamDemoV1.0.0.exe: ${OBJECTFILES}
	${MKDIR} -p dist/V3/App
	arm-brcm-linux-gnueabi-g++ -L . "-L${SDKV3LIB}" "-L${SDKV3LIBN}" -o dist/V3/App/FamDemoV1.0.0  ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/accoutopening.o: accoutopening.c 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA3000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA3000/include -o ${OBJECTDIR}/accoutopening.o accoutopening.c

${OBJECTDIR}/fileman.o: fileman.c 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA3000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA3000/include -o ${OBJECTDIR}/fileman.o fileman.c

${OBJECTDIR}/curlpost.o: curlpost.c 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA3000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA3000/include -o ${OBJECTDIR}/curlpost.o curlpost.c

${OBJECTDIR}/powerautomode.o: powerautomode.c 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA3000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA3000/include -o ${OBJECTDIR}/powerautomode.o powerautomode.c

${OBJECTDIR}/withdraw.o: withdraw.c 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA3000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA3000/include -o ${OBJECTDIR}/withdraw.o withdraw.c

${OBJECTDIR}/balance.o: balance.c 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA3000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA3000/include -o ${OBJECTDIR}/balance.o balance.c

${OBJECTDIR}/paywith.o: paywith.c 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA3000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA3000/include -o ${OBJECTDIR}/paywith.o paywith.c

${OBJECTDIR}/cJSON.o: cJSON.c 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA3000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA3000/include -o ${OBJECTDIR}/cJSON.o cJSON.c

${OBJECTDIR}/cardlessdeposit.o: cardlessdeposit.c 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA3000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA3000/include -o ${OBJECTDIR}/cardlessdeposit.o cardlessdeposit.c

${OBJECTDIR}/settings.o: settings.c 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA3000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA3000/include -o ${OBJECTDIR}/settings.o settings.c

${OBJECTDIR}/appmain.o: appmain.c 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA3000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA3000/include -o ${OBJECTDIR}/appmain.o appmain.c

${OBJECTDIR}/deposit.o: deposit.c 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA3000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA3000/include -o ${OBJECTDIR}/deposit.o deposit.c

${OBJECTDIR}/wub_lib.o: wub_lib.c 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA3000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA3000/include -o ${OBJECTDIR}/wub_lib.o wub_lib.c

${OBJECTDIR}/filemanager.o: filemanager.c 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA3000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA3000/include -o ${OBJECTDIR}/filemanager.o filemanager.c

${OBJECTDIR}/gsm.o: gsm.c 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA3000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA3000/include -o ${OBJECTDIR}/gsm.o gsm.c

${OBJECTDIR}/setpin.o: setpin.c 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA3000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA3000/include -o ${OBJECTDIR}/setpin.o setpin.c

${OBJECTDIR}/systemops.o: systemops.c 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA3000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA3000/include -o ${OBJECTDIR}/systemops.o systemops.c

${OBJECTDIR}/ministatement.o: ministatement.c 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA3000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA3000/include -o ${OBJECTDIR}/ministatement.o ministatement.c

${OBJECTDIR}/utilitypay.o: utilitypay.c 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA3000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA3000/include -o ${OBJECTDIR}/utilitypay.o utilitypay.c

${OBJECTDIR}/agentballance.o: agentballance.c 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA3000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA3000/include -o ${OBJECTDIR}/agentballance.o agentballance.c

${OBJECTDIR}/ussd.o: ussd.c 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA3000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA3000/include -o ${OBJECTDIR}/ussd.o ussd.c

${OBJECTDIR}/moremenu.o: moremenu.c 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA3000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA3000/include -o ${OBJECTDIR}/moremenu.o moremenu.c

${OBJECTDIR}/debug2.o: debug2.c 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA3000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA3000/include -o ${OBJECTDIR}/debug2.o debug2.c

${OBJECTDIR}/Setting.o: Setting.c 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA3000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA3000/include -o ${OBJECTDIR}/Setting.o Setting.c

${OBJECTDIR}/jsonbulder.o: jsonbulder.c 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA3000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA3000/include -o ${OBJECTDIR}/jsonbulder.o jsonbulder.c

${OBJECTDIR}/Transaction.o: Transaction.c 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA3000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA3000/include -o ${OBJECTDIR}/Transaction.o Transaction.c

${OBJECTDIR}/mainmenu.o: mainmenu.c 
	${MKDIR} -p ${OBJECTDIR}
	$(COMPILE.c) -O2 -I/cygdrive/C/Program\ Files/Castles/VEGA3000/include -I/cygdrive/C/Program\ Files\ \(x86\)/Castles/VEGA3000/include -o ${OBJECTDIR}/mainmenu.o mainmenu.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} dist/V3/App/FamDemoV1.0.0.exe

# Subprojects
.clean-subprojects:
