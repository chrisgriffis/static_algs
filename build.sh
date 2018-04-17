g++ -std=c++14 static_algs_main.cpp -o static_algs.out 2> run.log
echo '>>> stderr output:' >> run.log
echo >> run.log
echo '>>> Program output:' >> run.log
echo >> run.log
./static_algs.out >> run.log
cat run.log
