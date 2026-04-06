pkgname=qv2ray-dev-git
pkgver=2.8.0
pkgrel=1
pkgdesc="Cross-platform V2Ray Client written in Qt (Development Release)"
arch=('x86_64')
url='https://github.com/akliuxingyuan/Qv2ray'
license=('GPL3')
depends=('hicolor-icon-theme' 'qt6-base' 'grpc' 'qt6-svg')
optdepends=('xray: use system xray core.')
makedepends=('git' 'make' 'qt6-tools' 'which' 'gcc' 'qt6-declarative' 'cmake' 'ninja')
provides=('qv2ray')
conflicts=('qv2ray')
options=('debug')

source=()
sha512sums=()

prepare() {
    # dirty trick
    cd "${srcdir}"
    ln -sf ../Qv2ray Qv2ray
    cd Qv2ray/3rdparty/SingleApplication
    patch -Np1 < ${srcdir}/Qv2ray/singleapplication.patch
}

pkgver() {
    cd "${srcdir}/Qv2ray/"
    git config --global --add safe.directory "*"
    tag=$(git tag --sort=-version:refname | head -n1)
    count=$(git rev-list --count $tag..HEAD)
    if [[ $count > 0 ]]; then
        printf "%s.r%s.%s" ${tag} ${count} $(git rev-parse --short HEAD)
    else
        printf "%s" ${tag}
    fi
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
        -DQV2RAY_QT6=ON \
        -GNinja
    ninja -j $(nproc)
}

package() {
    cd "${srcdir}/Qv2ray"
    ninja -C "build" install
}
