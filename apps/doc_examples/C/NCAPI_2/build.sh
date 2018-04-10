mkdir -p build

#g++ cpp/simple_cpp_ncapi2.cpp -o build/simple_cpp -lmvnc
#g++ cpp/simple_cpp_withconv_ncapi2.cpp -o build/simple_cpp_with_conv -lmvnc

gcc -Wall -Wextra -std=c89 -pedantic -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition ./src/devicegetopt1.c -o ./build/devicegetopt1 -lmvnc
gcc -Wall -Wextra -std=c89 -pedantic -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition ./src/devicegetopt2.c -o ./build/devicegetopt2 -lmvnc
gcc -Wall -Wextra -std=c89 -pedantic -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition ./src/fifogetopt1.c -o ./build/fifogetopt1 -lmvnc
gcc -Wall -Wextra -std=c89 -pedantic -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition ./src/fifogetopt2.c -o ./build/fifogetopt2 -lmvnc
gcc -Wall -Wextra -std=c89 -pedantic -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition ./src/graphgetopt1.c -o ./build/graphgetopt1 -lmvnc
gcc -Wall -Wextra -std=c89 -pedantic -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition ./src/graphgetopt2.c -o ./build/graphgetopt2 -lmvnc
gcc -Wall -Wextra -std=c89 -pedantic -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition ./src/globalgetopt1.c -o ./build/globalgetopt1 -lmvnc
gcc -Wall -Wextra -std=c89 -pedantic -Wmissing-prototypes -Wstrict-prototypes -Wold-style-definition ./src/globalgetopt2.c -o ./build/globalgetopt2 -lmvnc

