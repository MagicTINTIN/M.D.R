debugPath="debug/"
buildPath="build/"
binSubPath="bin/"

binName="mdr"

subPath="$binSubPath$binName"

echo -e "executing binary\n----------"
echo "+ args: $@"

if [ "$debugPath$subPath" -nt "$buildPath$subPath" ]
then
    echo -e "$(tput setaf 2)$(tput bold)> / Debug$(tput sgr0)\n----------"
    $debugPath$subPath
else
    echo -e "$(tput setaf 2)$(tput bold)> / Build$(tput sgr0)\n----------"
    $buildPath$subPath
fi