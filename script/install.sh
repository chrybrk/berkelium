version=$(bk -v)
version_this=$(./bin/c/bk -v)

if [[ "$version" = "$version_this" ]]; then
    echo "already up-to-date"
else
    echo "updating bk"
    cp ./bin/c/bk /opt/
    if [[ $? -eq 0 ]]; then
        echo "bk: installation complete."
    fi
fi
