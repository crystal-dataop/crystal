# git@branch or git@commit or tarball@root
DEPS=(
https://github.com/facebookresearch/faiss/archive/v1.6.3.zip@faiss-1.6.3:"./configure --without-cuda"
)

mkdir -p _deps && cd _deps
if [ ! -f dep-builder.py ]; then
    curl -L https://github.com/crystal-dataop/dep-builder/tarball/master | tar xz --strip 2 -C .
fi

python dep-builder.py - "${DEPS[@]}"
