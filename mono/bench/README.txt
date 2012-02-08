This is a sample result.

No-IO prime-number benchmark (N=1000):
C++ : 0.01sec
C# : 0.08sec
Java : 0.2sec
Ruby : 1.4sec

No-IO prime-number benchmark (N=15000):

C++ : 1.7sec
C# : 1.6sec
Java : 2.0sec
Ruby : N/A


Fat-IO printing benchmark:

C++ : 1.3sec
C# : 24.5sec
Java : 32sec
Ruby : 36sec


------------------------------------

ringo@win147:/Users/ringo/test/mono/bench % make run
rm -f java_out mono_out cpp_out ruby_out
time ./bench 1000 > cpp_out

real    0m0.014s
user    0m0.008s
sys     0m0.004s
time mono bench.exe 1000 > mono_out

real    0m0.087s
user    0m0.047s
sys     0m0.030s
time java Bench 1000 > java_out

real    0m0.212s
user    0m0.118s
sys     0m0.041s
time ruby bench.rb 1000 > ruby_out

real    0m1.409s
user    0m1.357s
sys     0m0.010s
wc java_out
       1     170     657 java_out
diff java_out cpp_out
diff mono_out cpp_out
diff ruby_out cpp_out
rm -f java_out mono_out cpp_out ruby_out
time ./bench 15000 > cpp_out

real    0m1.703s
user    0m1.680s
sys     0m0.008s
time mono bench.exe 15000 > mono_out

real    0m1.601s
user    0m1.539s
sys     0m0.041s
time java Bench 15000 > java_out

real    0m2.004s
user    0m1.876s
sys     0m0.052s
wc java_out
       1    1756    9113 java_out
diff java_out cpp_out
diff mono_out cpp_out
time ./iob 5000000 > cpp_out

real    0m1.310s
user    0m1.236s
sys     0m0.052s
time mono iob.exe 5000000 > mono_out

real    0m24.203s
user    0m8.403s
sys     0m14.901s
time java Iob 5000000 > java_out

real    0m31.379s
user    0m14.608s
sys     0m15.908s
time ruby iob.rb 5000000 > ruby_out

real    0m36.679s
user    0m35.496s
sys     0m0.157s
wc java_out
       0       1 5000000 java_out
diff java_out cpp_out
diff mono_out cpp_out
diff ruby_out cpp_out
rm -f java_out mono_out cpp_out
ringo@win147:/Users/ringo/test/mono/bench %