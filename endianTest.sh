#Scott Snow
#!/bin/bash
if [ -f ./results.out ]
then
	rm results.out
fi
touch results.out
for i in 0 1 2 4 8 16 32 64
do
/usr/bin/time -p ./endian_reader_program $i 2> temp.out 

echo "Reading" $i "records" >> results.out

cat temp.out >> results.out

echo "" >> results.out

rm temp.out
done
