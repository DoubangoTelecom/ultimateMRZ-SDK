PYTHONPATH=../../../binaries/linux/x86_64:../../../python \
LD_LIBRARY_PATH=../../../binaries/linux/x86_64:$LD_LIBRARY_PATH \
python3 recognizer.py --image ../../../assets/images/Czech_passport_2005_MRZ_orient1_1300x1002.jpg --assets ../../../assets --backprop True --vcheck True --ielcd True