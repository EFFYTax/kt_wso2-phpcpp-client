#
#	Makefile template
#
#	This is an example Makefile that can be used by anyone who is building
#	his or her own PHP extensions using the PHP-CPP library. 
#
#	In the top part of this file we have included variables that can be
#	altered to fit your configuration, near the bottom the instructions and
#	dependencies for the compiler are defined. The deeper you get into this
#	file, the less likely it is that you will have to change anything in it.
#

#
#	Name of your extension
#
#	This is the name of your extension. Based on this extension name, the
#	name of the library file (name.so) and the name of the config file (name.ini)
#	are automatically generated
#

NAME				=	ktwso2


#
#	Php.ini directories
#
#	In the past, PHP used a single php.ini configuration file. Today, most
#	PHP installations use a conf.d directory that holds a set of config files,
#	one for each extension. Use this variable to specify this directory.
#

INI_DIR				=	/opt/php-5.6/etc	


#
#	The extension dirs
#
#	This is normally a directory like /usr/lib/php5/20121221 (based on the 
#	PHP version that you use. We make use of the command line 'php-config' 
#	instruction to find out what the extension directory is, you can override
#	this with a different fixed directory
#

EXTENSION_DIR		=   /opt/php-5.6/lib/php/extensions/no-debug-non-zts-20131226	


#
#	The name of the extension and the name of the .ini file
#
#	These two variables are based on the name of the extension. We simply add
#	a certain extension to them (.so or .ini)
#

EXTENSION 			=	${NAME}.so
INI 				=	${NAME}.ini


#
#	Compiler
#
#	By default, the GNU C++ compiler is used. If you want to use a different
#	compiler, you can change that here. You can change this for both the 
#	compiler (the program that turns the c++ files into object files) and for
#	the linker (the program that links all object files into the single .so
#	library file. By default, g++ (the GNU C++ compiler) is used for both.
#

COMPILER			=	g++
LINKER				=	g++


#
#	Compiler and linker flags
#
#	This variable holds the flags that are passed to the compiler. By default, 
# 	we include the -O2 flag. This flag tells the compiler to optimize the code, 
#	but it makes debugging more difficult. So if you're debugging your application, 
#	you probably want to remove this -O2 flag. At the same time, you can then 
#	add the -g flag to instruct the compiler to include debug information in
#	the library (but this will make the final libphpcpp.so file much bigger, so
#	you want to leave that flag out on production servers).
#
#	If your extension depends on other libraries (and it does at least depend on
#	one: the PHP-CPP library), you should update the LINKER_DEPENDENCIES variable
#	with a list of all flags that should be passed to the linker.
#

COMPILER_FLAGS		=	-Wall -c -g3 -fvisibility=hidden -Wno-write-strings -I./include -I./include/neethi -I./include/axis2 -I./kt_axis2c/util/src -I/opt/php-5.6/include/php/Zend -I/opt/php-5.6/include/php/TSRM -I/opt/php-5.6/include/php -I/opt/php-5.6/include/php/main -I../kt_sandesha2/include -I./kt_axis2c/include -I./kt_axis2c/util/include -I./kt_axis2c/axiom/include -I./kt_axis2c/neethi/include -I../kt_rampart/include -I/usr/include/boost -I./neethi -std=c++11 -fpic -o
LINKER_FLAGS		=	-shared 
LINKER_DEPENDENCIES	=	-L /usr/lib/x86_64-linux-gnu -L/usr/local/axis2c/lib -lxml2 -lphpcpp -laxis2_parser -laxis2_axiom -laxis2_engine -laxutil -laxis2_http_sender -laxis2_http_receiver -lrampart -lneethi -lsandesha2_client  -laxis2_tcp_receiver -laxis2_tcp_sender -laxis2_xpath   -lguththila     -lsandesha2  -lsavan_client -lsavan_msgreceivers -lsavan_publisher -lsavan_subs_mgr -lsavan_util 


#
#	Command to remove files, copy files and create directories.
#
#	I've never encountered a *nix environment in which these commands do not work. 
#	So you can probably leave this as it is
#

RM					=	rm -f
CP					=	cp -f
MKDIR				=	mkdir -p


#
#	All source files are simply all *.cpp files found in the current directory
#
#	A builtin Makefile macro is used to scan the current directory and find 
#	all source files. The object files are all compiled versions of the source
#	file, with the .cpp extension being replaced by .o.
#

SOURCES				=	src/neethi/options.cpp extension.cpp src/axis2/axis2_core.cpp src/axis2/axis2_client.cpp src/axis2/axis2_message.cpp src/ws_client.cpp src/ws_client_proxy.cpp  src/ws_data.cpp src/ws_fault.cpp src/ws_header.cpp src/ws_message.cpp src/ws_policy.cpp src/ws_security_token.cpp src/ws_utils.cpp src/param.cpp
OBJECTS				=	$(SOURCES:%.cpp=%.o)


#
#	From here the build instructions start
#

all:					${OBJECTS} ${EXTENSION}

${EXTENSION}:			${OBJECTS}
						${LINKER} ${LINKER_FLAGS} -o $@ ${OBJECTS} ${LINKER_DEPENDENCIES}

${OBJECTS}:
						${COMPILER} ${COMPILER_FLAGS} $@ ${@:%.o=%.cpp}

install:		
						${CP} ${EXTENSION} ${EXTENSION_DIR}
						${CP} ${INI} ${INI_DIR}
				
clean:
						${RM} ${EXTENSION} ${OBJECTS}

