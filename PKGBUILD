pkgname=qv2ray-dev-git
pkgver=2.8.0.8000.r3103.0eeab9e3
pkgver_=2.8.0.8000
pkgrel=1
pkgdesc="Cross-platform V2Ray Client written in Qt (Development Release)"
arch=('x86_64')
url='https://github.com/Qv2ray/Qv2ray'
license=('GPL3')
depends=('hicolor-icon-theme' 'qt6-base' 'grpc' 'qt6-svg')
optdepends=('xray: use system xray core.')
makedepends=('git' 'make' 'qt6-tools' 'which' 'gcc' 'qt6-declarative' 'cmake' 'ninja')
provides=('qv2ray')
conflicts=('qv2ray')

source=()
sha512sums=()


prepare() {
    # dirty trick
    cd "${srcdir}"
    ln -s ../Qv2ray Qv2ray
}

pkgver() {
    cd "${srcdir}/Qv2ray/"
    printf "%s.r%s.%s" $pkgver_ $(git rev-list --count HEAD) $(git rev-parse --short HEAD)
}

build() {
    export _QV2RAY_BUILD_INFO_="Qv2ray for Local"
    export _QV2RAY_BUILD_EXTRA_INFO_="(Official Build) $(uname -a | cut -d " " -f3,13)"

    cd "${srcdir}/Qv2ray"
    if [[ -e build ]];then
        rm -r build
    fi
    mkdir -p build && cd build
    cmake .. \
        -DCMAKE_INSTALL_PREFIX=${pkgdir}/usr \
        -DQV2RAY_TRANSLATION_PATH="/usr/share/qv2ray/lang" \
        -DQV2RAY_DEFAULT_VASSETS_PATH="/usr/share/xray" \
        -DQV2RAY_DEFAULT_VCORE_PATH="/usr/bin/xray" \
        -DQV2RAY_DISABLE_AUTO_UPDATE=on \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_POLICY_VERSION_MINIMUM=3.5 \
        -DQV2RAY_QT6=ON \
        -GNinja
    ninja -j $(nproc)
}

package() {
    cd "${srcdir}/Qv2ray"
    ninja -C "build" install
}
