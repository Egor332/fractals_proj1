cp compile_pgo1.sh compile_pgo2.sh
cp compile_pgo1.sh compile_pgo3off.sh
patch compile_pgo2.sh pgo_patch.txt
patch compile_pgo3off.sh nopgo_patch.txt
