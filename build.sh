g++ -std=c++14 static_algs_main.cpp -o static_algs.out 2> run.log
echo '\n\n++++++++++++++++++++++++++++++++++++++++++++++++++\n\nProgram output:\n\n' >> run.log
./static_algs.out >> run.log
cat run.log
