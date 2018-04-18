#g++ -std=c++14 static_algs_main.cpp -o static_algs.out 2> run.log
if [ -z "$1" ]
  then
    mainfile=static_algs_main.cpp
  else
    mainfile=$1
fi
echo building $mainfile
g++ -std=c++14 $mainfile -o static_algs.out 2> run.log
echo '>>> stderr output:' >> run.log
echo >> run.log
echo '>>> Program output:' >> run.log
echo >> run.log
./static_algs.out >> run.log
cat run.log
