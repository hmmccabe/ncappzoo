mkdir -P build && rm build/*
g++ cpp/simple_cpp_ncapi2.cpp -o build/simple_cpp -lmvnc
g++ cpp/simple_cpp_withconv_ncapi2.cpp -o build/simple_cpp_with_conv -lmvnc

g++ cpp/globalgetopt1.cpp -o build/globalgetopt1 -lmvnc
g++ cpp/globalgetopt2.cpp -o build/globalgetopt2 -lmvnc
