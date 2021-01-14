PYTHONPATH=../../../binaries/raspbian/armv7l:../../../python \
LD_LIBRARY_PATH=../../../binaries/raspbian/armv7l:$LD_LIBRARY_PATH \
python3 recognizer.py --image ../../../assets/images/Czech_passport_2005_MRZ_orient1_1300x1002.jpg --assets ../../../assets --backprop False --ielcd False