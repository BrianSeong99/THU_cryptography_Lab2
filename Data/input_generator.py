from string import ascii_letters, punctuation, digits
from random import choice

string_format = digits # ascii_letters + punctuation + digits
plaintext = "".join(choice(string_format) for x in range(2048))
key = "".join(choice(string_format) for x in range(56))
iv = "".join(choice(string_format) for x in range(64))

f = open("./plaintext.txt", 'rb+')
f.write(plaintext.encode())
f.close()

f = open("./key.txt", 'rb+')
f.write(key.encode())
f.close()

f = open("./iv.txt", 'rb+')
f.write(iv.encode())
f.close()
