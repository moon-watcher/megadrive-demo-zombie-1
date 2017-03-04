tools\7z\7z.exe -y x bin.7z
pushd .
cd demo
cd res
..\..\tools\7z\7z.exe -y x music.7z
popd
