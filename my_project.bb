DESCRIPTION = "grpc server application"
LICENSE = "CLOSED"
PR ="n0"
SRC_URI += "git://https://github.com/yessine15/gRPC-MQTT-bridge.git;protocol=https"
SRCREV = "ee21c6b038e2b695529e3b95ed0b1a4db68dfa5c"
S = "${WORKDIR}/git"
DEPENDS += "grpc-native grpc protobuf-native protobuf c-ares protobuf-c-native grpc"
DEPENDS += "openssl c-ares zlib openssl gcc"
DEPENDS += " gcc-runtime glibc libpcre2 gflags"
RDEPENDS_${PN} +=" libstdc++"

INSANE_SKIP_${PN} += "ldflags"

#inherit autotools

do_configure(){
        oe_runmake clean
}
do_configure(){
        oe_runmake all
}
do_install(){
 
    install -d ${D}${bindir}
    install -m 0755 ${S}/my_project_client ${D}${bindir}
    install -m 0755 ${S}/my_project_server ${D}${bindir}
}

