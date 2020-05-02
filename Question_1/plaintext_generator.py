from string import ascii_letters, punctuation, digits
from random import choice

string_format = ascii_letters + punctuation + digits
generated_string = "".join(choice(string_format) for x in range(int(16000)))

f = open("./plaintext.txt", 'rb+')
f.write(generated_string.encode())
f.close()
