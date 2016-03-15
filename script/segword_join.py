import os
file_r = open('./tmp.query', 'r')
file_w = open('./segword.join.1kw', 'w')
for line in file_r.readlines():
    column = line.split("\t")
    tmp = "".join(column[2:])
    file_w.write(tmp)
file_r.close()
file_w.close()
