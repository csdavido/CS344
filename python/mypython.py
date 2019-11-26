# Author: David Rider
# Class: CS 344
# November 6, 2019
# Citations:
#	https://codereview.stackexchange.com/questions/47529/creating-a-string-of-random-characters
#   https://stackoverflow.com/a/48739135
#   https://stackoverflow.com/a/18256476
# Self-Citation: I submitted this program last semester. The programm is unmodified since then.
# Note: Python3

import random
import string

#CREATE THREE FILES WITH RANDOM CHARACTERS

#OPEN FILES
f1 = open("file1", 'w')
f2 = open("file2", 'w')
f3 = open("file3", 'w')

#POPULATE FILES WITH 10 RANDOM CHARACTERS AND A NEWLINE
str1 = ''.join(random.choice(string.ascii_lowercase) for n in range(10))
f1.write(str1 + "\n")

str2 = ''.join(random.choice(string.ascii_lowercase) for n in range(10))
f2.write(str2 + "\n")

str3 = ''.join(random.choice(string.ascii_lowercase) for n in range(10))
f3.write(str3 + "\n")

#CLOSE FILES FROM WRITING
f1.close()
f2.close()
f3.close()

#PRINT CONTENTS OF THE FILES
#OPEN FILES FOR READING
rfile1 = open("file1", 'r')
rfile2 = open("file2", 'r')
rfile3 = open("file3", 'r')

print(rfile1.read(), end="")
print(rfile2.read(), end="")
print(rfile3.read(), end="")

rfile1.close()
rfile2.close()
rfile3.close()

#CREATE TWO RANDOM NUMBERS BETWEEN 1 AND 42
num1 = random.randint(1, 42)
num2 = random.randint(1, 42)

#DISPLAY RANDOM NUMBERS
print(num1)
print(num2)

#DISPLAY PRODUCT OF RANDOM NUMBERS
print(num1 * num2)