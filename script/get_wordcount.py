import string
def add_words(word, words_dict):
    if word in words_dict:
        words_dict[word] += 1
    else:
        words_dict[word] = 1

def process_line(line, words_dict):
    line = line.rstrip('\n')
    column = line.split("\t")
    words_list = column[2:]
    for word in words_list:
        add_words(word, words_dict)

def print_result(words_dict):
    val_key_list = []
    file_w = open("./result_py_1kw", "w")
    for key, val in words_dict.items():
        val_key_list.append((val, key))
    for val, key in val_key_list:
        line = key + '	' + str(val) + '\n'
        file_w.write(line)
    file_w.close()

def main():
    words_dict = {}
    pFile = open("tmp.query", "r")
    for line in pFile.readlines():
        process_line(line, words_dict)
    print "length of the dictionary:", len(words_dict)
    print_result(words_dict)
    pFile.close()

main()
