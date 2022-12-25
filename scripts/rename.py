import os
import itertools
import string








print(string.ascii_uppercase)


#for letter, path in itertools.islice(zip(string.ascii_uppercase, sorted(filter(lambda p: "rename" not in p, os.listdir(".")))), 5):
#    os.system(f"mv {path} {letter}.jpeg")
for letter, path in itertools.islice(zip(' '+string.ascii_uppercase, sorted(filter(lambda p: "rename" not in p, os.listdir(".")))), 6, 100):
    print(letter, path)
    os.system(f"mv {path} {letter}.jpeg")
