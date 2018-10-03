#g++ -std=c++14 static_algs_main.cpp -o static_algs.out 2> run.log
#if [ -z "$1" ]
#  then
#    mainfile=static_algs_main.cpp
mainfile=static_algs_main.cpp
#  else
#    mainfile=$1
#fi
echo building $mainfile
g++ -std=c++17 $mainfile -o static_algs.out  
# | tee 2> run.log
# echo '>>> stderr output:' | tee >> run.log
# echo | tee >> run.log
# echo '>>> Program output:' | tee >> run.log
# echo | tee >>  run.log
# ./static_algs.out $@ | tee >> run.log
