version=$(bk -v)
version_this=$(./bin/c/bk -v)
if [[ $version -eq $version_this ]]; then
    echo "already up-to-date"
else
    sudo cp ./bin/c/bk /opt/
    if [[ $? -eq 0 ]]; then
        echo "bk: installation complete."
    fi
fi
