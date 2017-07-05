function detect_platform() {
case "$1" in
"")
    UNAME=$(uname)
    case "${UNAME}" in
    Darwin)
        echo mac
        ;;
    *)
        echo linux
        ;;
    esac
    ;;
*)
    echo $1
    ;;
esac
}

#

detect_platform $1
